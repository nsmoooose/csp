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
	char m_Set[256];
	struct Channel {
		std::string name;
	};
	std::vector<Channel> m_Channels;
	int m_Limit, m_Count;
	bool m_Enabled;
	float m_ElapsedTime;
	FILE *m_File;

	enum {TIME=250, PAUSE=251, RESUME=252, END=255};

public:
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
	DataRecorder(std::string const &filename, int cache=20000) {
		m_File = (FILE *) fopen(filename.c_str(), "wb");
		// XXX improve the error handling
		if (m_File == 0) throw "unable to open flight data recorder output";
		m_Cache.resize(cache);
		m_Channels.reserve(16);
		m_Count = 0;
		m_Limit = cache;
		m_ElapsedTime = 0.0;
		m_Enabled = true;
	}
	
	/**
	 * Add a new output channel to the recorder.
	 *
	 * Up to 250 output channels can be created.  The name parameter
	 * will be stored in the output file, and is used by external tools
	 * to identify the different data streams.  Internally, the integer
	 * value returned by this method is used to refer to the channel
	 * during subsequent recording.   Channel names are only used for 
	 * later analysis, so multiple channels can have the same name and
	 * will be independent (although there is probably no good reason to
	 * do this).
	 *
	 * @param name The name of the data channel to add.
	 * @returns The id number of the new channel for use with record().
	 *          Returns -1 if channel creation failed (e.g. too many
	 *          channels).
	 */
	int addChannel(std::string const &name) {
		int n = int(m_Channels.size());
		if (n >= 250) return -1;
		Channel channel;
		channel.name = name;
		m_Channels.push_back(channel);
		return n;
	}

	/**
	 * Mark the current data stream time.  The recorder monitors only
	 * elapsed time, not absolute time, so the argument here is a time
	 * interval.  The elapsed time is managed internally.  This method
	 * will insert a time marker into the data stream, and also reset
	 * the channel recording flags.  Each channel can only record one
	 * value between time stamps, so it is important to call this method
	 * regularly.
	 *
	 * @param dt The time interval since the last time stamp.
	 */
	void timeStamp(float dt) {
		m_ElapsedTime += dt;
		_record(TIME, m_ElapsedTime);
		memset(m_Set, 0, 256);
	}

	/**
	 * Record the value of an output channel.  Only one call to record() 
	 * can be made per channel after each time stamp.  Multiple recordings
	 * to the same channel between time stamps will be silently ignored.
	 *
	 * @param id The channel id returned by addChannel()
	 * @param value The value to record.
	 */
	void record(int id, float value) {
		if (id >= 0 && id <= int(m_Channels.size())) {
			// only record one entry per channel per timestamp
			if (m_Set[id]==0) {
				_record(id, value);
				m_Set[id] = 1;
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
			size_t channel_start = ftell(m_File);
			int n = m_Channels.size();
			fwrite(&n, sizeof(n), 1, m_File);
			for (size_t i = 0; i < m_Channels.size(); i++) {
				char channel_name[64];
				memset(channel_name, 0, 64);
				strncpy(channel_name, m_Channels[i].name.c_str(), 63);
				fwrite(channel_name, 64, 1, m_File);
			}
			fwrite(&channel_start, sizeof(channel_start), 1, m_File);
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
	inline void _record(int id, float value) {
		if (m_Enabled && m_File != 0) {
			m_Cache[m_Count] = DataEntry(static_cast<unsigned int>(id), value);
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


/**
 * RecorderInterface provides access a DataRecorder and maintains a
 * list of local output channels.  When multiple classes access the
 * same DataRecorder, each one should use a separate RecorderInterface
 * to manage output channels.  Simply bind the DataRecorder to the
 * RecorderInterface and add local channels.  Unlike the raw 
 * DataRecorder interface, fixed channel identifiers (0-249) can
 * be used since the RecorderInterface keeps an internal translation
 * table.  Typically a class will define fixed channel identifiers
 * as protected enums, starting at zero.
 */
class RecorderInterface {
protected:
	// FIXME should use a weakref here
	simdata::Ref<DataRecorder> m_Recorder;
	std::vector<int> m_Channels;

public:
	/**
	 * Bind to an existing DataRecorder.  Channels must be added
	 * (or readded) after this method is called.
	 *
	 * @param recorder The DataRecorder instance, or NULL to unbind.
	 */
	void bind(DataRecorder *recorder) { 
		m_Recorder = recorder; 
		m_Channels.clear();
	}

	/**
	 * Returns true if a recorder is bound and is enabled.
	 */
	inline bool isEnabled() const {
		return (m_Recorder.valid() && m_Recorder->isEnabled());
	}

	/**
	 * Add a new output channel.
	 *
	 * @param id A fixed identifier for use with the record() method. 
	 *           Legal values are 0 to 249.
	 * @param name The label the output channel.
	 */
	void addChannel(int id, std::string const &name) {
		if (!m_Recorder || id >= 250) return;
		if (id >= int(m_Channels.size())) {
			m_Channels.resize(id+1, -1);
		}
		m_Channels[id] = m_Recorder->addChannel(name);
	}

	/**
	 * Record a data value to an output channel.
	 *
	 * @param id The channel id.
	 * @param value The value to record.
	 */
	void record(int id, float value) {
		if (m_Recorder.valid() && id >= 0 && id < int(m_Channels.size())) {
			int channel = m_Channels[id];
			m_Recorder->record(channel, value);
		}
	}

	/**
	 * Mark the current data stream time.  See DataRecorder::stampTime()
	 * for details.
	 *
	 * @param dt The time interval since the last time stamp.
	 */
	void timeStamp(float dt) {
		if (m_Recorder.valid()) {
			m_Recorder->timeStamp(dt);
		}
	}
};


#endif // __DATARECORDER_H__

