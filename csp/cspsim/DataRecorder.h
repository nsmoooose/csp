// Combat Simulator Project
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

#ifndef __CSPSIM_DATARECORDER_H__
#define __CSPSIM_DATARECORDER_H__

#include <csp/csplib/util/Referenced.h>
#include <csp/csplib/util/ScopedPointer.h>
#include <csp/csplib/data/Vector3.h>

#include <cstdio>

namespace csp {

class Bus;
class DataChannelBase;

/**
 * A very preliminary flight data recorder class aimed primarily at flight
 * model testing and validation rather than anything approaching a full acmi
 * recorder.
 */
class DataRecorder: public Referenced {
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
	DataRecorder(std::string const &filename, unsigned char level=LEVEL_VEHICLE, int cache=20000);
	
	unsigned char getLevel() const { return m_Level; }

	bool addSource(Ref<Bus> bus, std::string const &name);
	bool addSource(Ref<const DataChannelBase> channel);

	/**
	 * Mark the current data stream time.  The recorder monitors only elapsed
	 * time, not absolute time, so the argument here is a time interval.  The
	 * elapsed time is managed internally.  This method will insert a time
	 * marker into the data stream and store all data channels that have
	 * changed since the last update.
	 *
	 * @param dt The time interval since the last time stamp.
	 */
	void timeStamp(float dt);

	/**
	 * Enable or disable the recorder.  Calls to record() and timeStamp() will
	 * be silently ignored when the recorder is disabled.
	 *
	 * @param on True for enabled, false for disabled.
	 */
	void setEnabled(bool on);

	/**
	 * Test whether the recorder is open and enabled.
	 */
	inline bool isEnabled() const { return m_Enabled && !isClosed(); }

	/**
	 * Test whether the recorder has been closed.  Once closed, no further
	 * output to the recorder is possible.
	 */
	inline bool isClosed() const { return !m_File; }

	/**
	 * Close the recorder, flushing the cache to disk and finalizing the output
	 * file.  Once closed a DataRecorder instance cannot be reopened and will
	 * not record any further data.
	 */
	void close();

protected:

	/**
	 * Write an output channel entry to the cache.  Does nothing if
	 * the recorder is disabled or closed, and takes care to flush
	 * the cache to disk when it is full.
	 */
	void _record(unsigned int id, float value);
	
	/**
	 * Flush the internal cache to disk.
	 */
	inline void _flush();

	/**
	 * Close the recorder output if it is open before destruction.
	 */
	virtual ~DataRecorder();

private:
	struct DataEntry;
	std::vector<DataEntry> m_Cache;

	class DataSource;
	class SingleSource;
	class VectorSource;

	std::vector<Ref<DataSource> > m_Sources;

	unsigned char m_Level;
	int m_Limit, m_Count;
	bool m_Enabled;
	float m_ElapsedTime;

	struct File;
	ScopedPointer<File> m_File;

	enum {TIME=250, PAUSE=251, RESUME=252, END=255};
};


} // namespace csp

#endif // __CSPSIM_DATARECORDER_H__

