// Combat Simulator Project - CSPSim
// Copyright (C) 2002 The Combat Simulator Project
// http://csp.sourceforge.net
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


/**
 * @file DataRecorder.h
 *
 **/


#ifndef __DATARECORDER_H__
#define __DATARECORDER_H__


#include <cstdio>

#include "SimData/Vector3.h"

#include <Bus.h>


/**
 * A very preliminary flight data recorder class aimed
 * primarily at flight model testing and validation rather
 * than anything approaching a full acmi recorder.
 */
class DataRecorder: public simdata::Referenced {
	struct DataEntry {
		unsigned int id;
		float value;
		DataEntry() {}
		DataEntry(unsigned int id_, float value_): id(id_), value(value_) {}
	};
	std::vector<DataEntry> m_Cache;


	class DataSource: public simdata::Referenced {
	protected:
		mutable bool m_Dirty;
		std::string m_Name;
		float m_Last;
		void update(float value) {
			m_Dirty = (value != m_Last);
			if (m_Dirty) {
				m_Last = value;
			}
		}
	public:
		typedef simdata::Ref<DataSource> Ref;
		DataSource(): m_Dirty(false), m_Last(0.0) {}
		virtual ~DataSource() {}
		virtual void refresh() = 0;
		std::string const &getName() const { return m_Name; }
		float getValue() const { return m_Last; }
		bool isDirty() const { return m_Dirty; }
	};
	
	class SingleSource: public DataSource {
		DataChannel<double>::CRef m_Channel;
	public:
		SingleSource(DataChannel<double>::CRef const &source): 
			m_Channel(source) 
		{
			m_Name = m_Channel->getName();
		};
		void refresh() {
			float val = static_cast<float>(m_Channel->value());
			update(val);
		}
	};

	class VectorSource: public DataSource {
		DataChannel<simdata::Vector3>::CRef m_Channel;
		int m_Index;
	public:
		VectorSource(DataChannel<simdata::Vector3>::CRef const &source, int idx): 
			m_Channel(source), 
			m_Index(idx) 
		{
			assert(m_Channel.valid());
			assert(idx >= 0 && idx <= 2);
			m_Name = m_Channel->getName() + "." + static_cast<char>('X' + idx);
		}
		void refresh() {
			simdata::Vector3 const &vec = m_Channel->value();
			float val = static_cast<float>(vec[m_Index]);
			update(val);
		}
	};

	std::vector<DataSource::Ref> m_Sources;

	unsigned char m_Level;
	int m_Limit, m_Count;
	bool m_Enabled;
	float m_ElapsedTime;
	FILE *m_File;

	enum {TIME=250, PAUSE=251, RESUME=252, END=255};


public:

	enum { LEVEL_NONE=0, LEVEL_OBJECT, LEVEL_VEHICLE, LEVEL_SYSTEMS, LEVEL_DEBUG };

	/**
	 * Construct a new data recorder, opening the output file
	 * for writing and allocating an internal cache.  The reorder
	 * will be enabled by default, use setEnabled() to disable it
	 * if desired.
	 *
	 * @param filename The output filename for the data recording.
	 * @param cache The number of values that can be recorded before
	 *              flushing the data to the output files.  Each cached
	 *              value currently requires 8 bytes.
	 */
	DataRecorder(std::string const &filename, unsigned char level=LEVEL_VEHICLE, int cache=20000) {
		m_File = (FILE *) fopen(filename.c_str(), "wb");
		// XXX improve the error handling
		if (m_File == 0) throw "unable to open flight data recorder output";
		m_Cache.resize(cache);
		m_Sources.reserve(16);
		m_Count = 0;
		m_Level = level;
		m_Limit = cache;
		m_ElapsedTime = 0.0;
		m_Enabled = true;
	}
	
	unsigned char getLevel() const { return m_Level; }

	bool addSource(Bus::Ref bus, std::string const &name) {
		if (!bus) return false;
		simdata::Ref<const DataChannelBase> channel = bus->getChannel(name);
		return addSource(channel);
	}

	bool addSource(simdata::Ref<const DataChannelBase> channel) {
		if (!channel) return false;
		DataChannel<double>::CRef dchannel = channel;
		if (dchannel.valid()) {
			m_Sources.push_back(new SingleSource(dchannel));
			return true;
		} else {
			DataChannel<simdata::Vector3>::CRef vchannel = channel;
			if (vchannel.valid()) {
				m_Sources.push_back(new VectorSource(vchannel, 0));
				m_Sources.push_back(new VectorSource(vchannel, 1));
				m_Sources.push_back(new VectorSource(vchannel, 2));
				return true;
			}
		}
		return false;
	}

	/**
	 * Mark the current data stream time.  The recorder monitors only
	 * elapsed time, not absolute time, so the argument here is a time
	 * interval.  The elapsed time is managed internally.  This method
	 * will insert a time marker into the data stream and store all
	 * data channels that have changed since the last update.
	 *
	 * @param dt The time interval since the last time stamp.
	 */
	void timeStamp(float dt) {
		if (!isEnabled()) return;
		m_ElapsedTime += dt;
		_record(TIME, m_ElapsedTime);
		unsigned int idx = 0;
		unsigned int n = m_Sources.size();
		for (; idx < n; ++idx) {
			m_Sources[idx]->refresh();
			if (m_Sources[idx]->isDirty()) {
				_record(idx, m_Sources[idx]->getValue());
			}
		}
	}

	/**
	 * Enable or disable the recorder.  Calls to record() and timeStamp()
	 * will be silently ignored when the recorder is disabled.
	 *
	 * @param on True for enabled, false for disabled.
	 */
	void setEnabled(bool on) {
		if (m_Enabled && !on) {
			_record(PAUSE, 0.0);
			m_Enabled = false;
		} else
		if (on && !m_Enabled) {
			m_Enabled = true;
			_record(RESUME, 0.0);
		}
	}

	/**
	 * Test whether the recorder is open and enabled.
	 */
	inline bool isEnabled() const { return m_Enabled && !isClosed(); }

	/**
	 * Test whether the recorder has been closed.  Once closed,
	 * no further output to the recorder is possible.
	 */
	inline bool isClosed() const { return m_File == 0; }

	/**
	 * Close the recorder, flushing the cache to disk and finalizing
	 * the output file.  Once closed a DataRecorder instance cannot
	 * be reopened and will not record any further data.
	 */
	void close() {
		if (m_File != 0) {
			m_Enabled = true;
			_record(END, 0.0); // end of data
			_flush();
			size_t source_start = ftell(m_File);
			int n = m_Sources.size();
			fwrite(&n, sizeof(n), 1, m_File);
			for (size_t i = 0; i < m_Sources.size(); i++) {
				char source_name[100];
				memset(source_name, 0, 100);
				strncpy(source_name, m_Sources[i]->getName().c_str(), 99);
				fwrite(source_name, 100, 1, m_File);
			}
			fwrite(&source_start, sizeof(source_start), 1, m_File);
			fclose(m_File);
			m_File = 0;
		}
	}

protected:

	/**
	 * Write an output channel entry to the cache.  Does nothing if
	 * the recorder is disabled or closed, and takes care to flush
	 * the cache to disk when it is full.
	 */
	inline void _record(unsigned int id, float value) {
		if (m_Enabled && m_File != 0) {
			m_Cache[m_Count] = DataEntry(id, value);
			if (++m_Count >= m_Limit) {
				_flush();
			}
		}
	}
	
	/**
	 * Flush the internal cache to disk.
	 */
	inline void _flush() {
		fwrite(&(m_Cache[0]), sizeof(DataEntry), m_Count, m_File);
		m_Count = 0;
	}

	/**
	 * Close the recorder output if it is open before destruction.
	 */
	virtual ~DataRecorder() {
		close();
	}
};



#endif // __DATARECORDER_H__

