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
 * @file DataRecorder.cpp
 *
 **/



#include <csp/cspsim/DataRecorder.h>
#include <csp/cspsim/Bus.h>

#include <cstdio>

CSP_NAMESPACE

struct DataRecorder::File {
	File(FILE *file): fptr(file) { }
	~File() { if (fptr) fclose(fptr); }
	FILE *fptr;
};


struct DataRecorder::DataEntry {
	unsigned int id;
	float value;
	DataEntry() {}
	DataEntry(unsigned int id_, float value_): id(id_), value(value_) {}
};


class DataRecorder::DataSource: public Referenced {
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
	DataSource(): m_Dirty(false), m_Last(0.0) {}
	virtual ~DataSource() {}
	virtual void refresh() = 0;
	std::string const &getName() const { return m_Name; }
	float getValue() const { return m_Last; }
	bool isDirty() const { return m_Dirty; }
};


class DataRecorder::SingleSource: public DataRecorder::DataSource {
	DataChannel<double>::CRefT m_Channel;
public:
	SingleSource(DataChannel<double>::CRefT const &source): m_Channel(source) {
		m_Name = m_Channel->getName();
	}
	void refresh() {
		float val = static_cast<float>(m_Channel->value());
		update(val);
	}
};


class DataRecorder::VectorSource: public DataRecorder::DataSource {
	DataChannel<Vector3>::CRefT m_Channel;
	int m_Index;
public:
	VectorSource(DataChannel<Vector3>::CRefT const &source, int idx):
		m_Channel(source),
		m_Index(idx)
	{
		assert(m_Channel.valid());
		assert(idx >= 0 && idx <= 2);
		m_Name = m_Channel->getName() + "." + static_cast<char>('X' + idx);
	}
	void refresh() {
		Vector3 const &vec = m_Channel->value();
		float val = static_cast<float>(vec[m_Index]);
		update(val);
	}
};


DataRecorder::DataRecorder(std::string const &filename, unsigned char level, int cache) {
	FILE *file = (FILE *) fopen(filename.c_str(), "wb");
	if (!file) throw "unable to open flight data recorder output"; // XXX improve the error handling
	m_File.reset(new File(file));
	m_Cache.resize(cache);
	m_Sources.reserve(16);
	m_Count = 0;
	m_Level = level;
	m_Limit = cache;
	m_ElapsedTime = 0.0;
	m_Enabled = true;
}

DataRecorder::~DataRecorder() {
	close();
}

bool DataRecorder::addSource(Ref<Bus> bus, std::string const &name) {
	if (!bus) return false;
	Ref<const DataChannelBase> channel = bus->getChannel(name);
	return addSource(channel);
}

bool DataRecorder::addSource(Ref<const DataChannelBase> channel) {
	if (!channel) return false;
	DataChannel<double>::CRefT dchannel;
	if (dchannel.tryAssign(channel)) {
		m_Sources.push_back(new SingleSource(dchannel));
		return true;
	} else {
		DataChannel<Vector3>::CRefT vchannel;
		if (vchannel.tryAssign(channel)) {
			m_Sources.push_back(new VectorSource(vchannel, 0));
			m_Sources.push_back(new VectorSource(vchannel, 1));
			m_Sources.push_back(new VectorSource(vchannel, 2));
			return true;
		}
	}
	return false;
}

void DataRecorder::timeStamp(float dt) {
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

void DataRecorder::setEnabled(bool on) {
	if (m_Enabled && !on) {
		_record(PAUSE, 0.0);
		m_Enabled = false;
	} else
	if (on && !m_Enabled) {
		m_Enabled = true;
		_record(RESUME, 0.0);
	}
}

void DataRecorder::close() {
	if (m_File.valid()) {
		m_Enabled = true;
		_record(END, 0.0); // end of data
		_flush();
		size_t source_start = ftell(m_File->fptr);
		int n = m_Sources.size();
		fwrite(&n, sizeof(n), 1, m_File->fptr);
		for (size_t i = 0; i < m_Sources.size(); i++) {
			char source_name[100];
			memset(source_name, 0, 100);
			strncpy(source_name, m_Sources[i]->getName().c_str(), 99);
			fwrite(source_name, 100, 1, m_File->fptr);
		}
		fwrite(&source_start, sizeof(source_start), 1, m_File->fptr);
		m_File.reset(0);
	}
}

void DataRecorder::_record(unsigned int id, float value) {
	if (m_Enabled && m_File.valid()) {
		m_Cache[m_Count] = DataEntry(id, value);
		if (++m_Count >= m_Limit) {
			_flush();
		}
	}
}

void DataRecorder::_flush() {
	fwrite(&(m_Cache[0]), sizeof(DataEntry), m_Count, m_File->fptr);
	m_Count = 0;
}

CSP_NAMESPACE_END


