// Combat Simulator Project - Terrain Tools
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
 * @file dem2dat.h
 *
 * Converts USGS Digital Elevation Map (DEM) data to a custom
 * binary format (DAT).
 *
 * @author Mark Rose <mkrose@users.sourceforge.net>
 */


#ifndef __DEM2DAT__
#define __DEM2DAT__

#include <SimData/Math.h>
#include <SimData/GeoPos.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>

using namespace simdata;


/**
 * Simple text file input class designed specifically for DEM metadata.
 */
class FileBuffer {
public:
	FileBuffer(int buffersize=16384) {
		_buffer = new char[buffersize];
		_size = buffersize;
		_idx = buffersize;
		_fp = 0;
	}
	
	void attach(FILE *fp) {
		_fp = fp;
	}

	~FileBuffer() {
		delete[] _buffer;
	}

	double d2e(int n) {
		char *c = _buffer+_idx;
		assert(_idx + n < _size);
		while (--n >= 0) {
			if (*c == 'D') {
				*c = 'e';
			}
			c++;
		}
	}

	double getd(int n) {
		char *c = _buffer+_idx;
		char fmt[20];
		double v;
		assert(_idx + n < _size);
		sprintf(fmt, "%%%dle", n);
		d2e(n);
		sscanf(c, fmt, &v);
		_idx += n;
		return v;
	}

	void getData(int threshold = -1) {
		assert(_fp != 0);
		assert(_idx <= _size);
		if (threshold < 0) threshold = _size;
		int n = _size - _idx;
		assert(n >= 0);
		if (n > threshold) return;
		if (n>0) {
			assert(n<_size);
			memcpy(_buffer, _buffer+_idx, n);
		}
		fread(_buffer+n, _size-n, 1, _fp);
		_idx = 0;
	}

	inline const char* buffer(int n=0) { 
		char *pos = _buffer+_idx;
		_idx += n;
		return pos;
	}

	inline void advance(int n) {
		_idx += n;
		assert(_idx <= _size);
	}

	inline void eatSpaces() {
		char *b = _buffer + _idx;
		while (1) {
			while ((*b==' ' || *b=='\n' || *b=='\r') && _idx < _size) {
				b++; 
				_idx++;
			}
			if (_idx < _size) break;
			getData();
		}
	}

	inline int getFastInt() {
		char *b = _buffer + _idx;
		int n = 12;
		int v = 0;
		bool s = true;
		assert(_idx + n < _size);
		while (--n >= 0) {
			if (*b != ' ' && *b != '\n' && *b != '\r') {
				v = v * 10 + *b - '0';
				s = false;
			} else {
				if (!s) break;
			}
			b++;
			_idx++;
		}
		//std::cout << n << ":" << v << " ";
		return v;
	}

	inline void eatDigits() {
		char *b = _buffer  + _idx;
		while (1) {
			while ((*b==' ' || *b=='\n' || *b=='\r') && _idx < _size) {
				b++; 
				_idx++;
			}
			if (_idx == _size) getData();
			while (*b>='0' && *b<='9' && _idx < _size) {
				b++; 
				_idx++;
			}
			if (_idx < _size) break;
			getData();
		}
	}

private:
	FILE *_fp;
	char *_buffer;
	int _size;
	int _idx;
};



/**
 * Class encapsulating DEM metadata, providing conversion to DAT format.
 */
class MetaDEM {
protected:
	enum { BIAS = 1000 };
	std::string _title;
	std::string _region;
	std::string _source_file;
	int _source_index;
	int _level, _pttrn, _coord, _zone;
	std::vector<double> _proj;
	std::vector<double> _xcor;
	std::vector<double> _ycor;
	int _gnd_units, _elev_units, _sides;
	double _min_value, _max_value, _phi;
	double _bbox0, _bbox1, _bbox2, _bbox3;
	int _accu;
	std::vector<double> _resolu;
	int _rows, _cols;
	int _datum;
	double _lat, _lon;
	int _scale;
	int _bias;
	std::string _fn;

protected:
	MetaDEM(): _proj(15), _xcor(4), _ycor(4), _resolu(3) {
		_bias = BIAS;
		_dirty = true;
		_scale = 0;
		_lat = _lon = 0.0;
		_datum = 0;
		_rows = _cols = 0;
		_accu = 0;
		_bbox0 = _bbox1 = _bbox2 = _bbox3 = 0.0;
		_min_value = _max_value = 0.0;
		_phi = 0.0;
		_gnd_units = _elev_units = _sides = 0;
		_level = _pttrn = _coord = _zone = 0;
		_source_index = 0;
	}

public:
	inline int scale() const { return _scale; }
	inline double latitude() const { return _lat; }
	inline double longitude() const { return _lon; }
	inline double datum() const { return _datum; }
	inline ReferenceEllipsoid const &getReferenceEllipsoid() const {
		switch (_datum) {
			case 1: return GeoRef::NAD27; 
			case 2: return GeoRef::WGS72;
			case 3: return GeoRef::WGS84;
			// case 4: return GeoRef::MAX; // FIXME
		}
		std::cerr << "WARNING: unrecognized reference ellipsoid (" << _datum << "), using WGS84\n";
		return GeoRef::WGS84;
	}
	inline int rows() const { return _rows; }
	inline int cols() const { return _cols; }
	inline double res_x() const { return _resolu[0]; }
	inline double res_y() const { return _resolu[1]; }
	inline double res_z() const { return _resolu[2]; }
	inline int accuracy() const { return _accu; }
	inline void getBounds(double &b0, double &b1, double &b2, double &b3) {
		b0 = _bbox0;
		b1 = _bbox1;
		b2 = _bbox2;
		b3 = _bbox3;
	}

	inline int groundUnits() const { return _gnd_units; }
	inline std::string getGroundUnitString() const { return getUnitString(_gnd_units); }
	inline int elevationUnits() const { return _elev_units; }
	inline std::string getElevationUnitString() const { return getUnitString(_elev_units); }

	static std::string getUnitString(int unit) {
		switch (unit) {
			case 0: return "";  // XXX fixme
			case 1: return "ft";
			case 2: return "m";
		}
		return ""; // XXX fixme
	}

	inline int sides() const { return _sides; }
	inline int level() const { return _level; }
	inline int pattern() const { return _pttrn; }
	inline int coord() const { return _coord; }
	inline int zone() const { return _zone; }
	inline int sourceIndex() const { return _source_index; }
	inline std::string const & sourceFile() const { return _source_file; }
	inline std::string const & region() const { return _region; }
	inline std::vector<double> const & projection() const { return _proj; }
	inline std::vector<double> const & xCorners() const { return _xcor; }
	inline std::vector<double> const & yCorners() const { return _ycor; }

	inline double phi() const { return _phi; }
	inline double getMinimumElevation() const { return scaleElevation(_min_value); }
	inline double getMaximumElevation() const { return scaleElevation(_min_value); }

	static inline double meterScale(int unit) {
		switch (unit) {
			case 1: return 12.0 * 0.0254;
			case 2: return 1.0;
		}
		return 1.0;
	}
		
	inline double scaleElevation(double e) const {
		return (e - BIAS) * _resolu[2] * meterScale(_elev_units);
	}
	inline double getElevationScale() const {
		return meterScale(_elev_units) * _resolu[2];
	}
	inline double getBias() const { return _bias; }

	#define DUMP(a) std::cout << #a << " = " << (a) << std::endl;
	#define DUTM(a, b, c) std::cout << #a << " = " << (b) << ", " << (c) << std::endl;
	void dump() {
		DUMP(_title);
		DUMP(_region);
		DUMP(_source_file);
		DUMP(_source_index);
		DUMP(_level);
		DUMP(_pttrn);
		DUMP(_coord);
		DUMP(_zone);
		DUTM(_cor0, _xcor[0], _ycor[0]);
		DUTM(_cor1, _xcor[1], _ycor[1]);
		DUTM(_cor2, _xcor[2], _ycor[2]);
		DUTM(_cor3, _xcor[3], _ycor[3]);
		DUMP(_rows);
		DUMP(_cols);
		DUMP(_datum);
		DUMP(_scale);
		DUMP(_lat);
		DUMP(_lon);
		DUMP(_resolu[0]);
		DUMP(_resolu[1]);
		DUMP(_resolu[2]);
	}
	#undef DUMP
	#undef DUTM

protected:
	mutable UTM _ref_utm;
	mutable LLA _ref_lla;
	mutable bool _dirty;

	void checkValid() const {
		if (_dirty) {
			_ref_utm.set(_xcor[0], _ycor[0], _zone, 'X');
			_ref_lla = LLA(_ref_utm, getReferenceEllipsoid());
			_dirty = false;
		}
	}

	inline UTM const &getReferenceUTM() const {
		checkValid();
		return _ref_utm;
	}			

	inline LLA const &getReferenceLLA() const {
		checkValid();
		return _ref_lla;
	}			

	FILE *openDAT(const char* opts) {
		char fn[64];
		UTM ref_utm = getReferenceUTM();
		LLA ref_lla = getReferenceLLA();
		int l0 = int((ref_lla.latitude()*180.0/G_PI+90)*8+0.5);
		int l1 = int((ref_lla.longitude()*180.0/G_PI+180)*8+0.5);
		sprintf(fn, "%04d_%04d.dat", l0, l1);
		_fn = fn;
		return (FILE*) fopen(fn, opts);
	}

	#define WRITED(x) fwrite(&(x), sizeof(double), 1, fp);
	#define WRITEI(x) fwrite(&(x), sizeof(int), 1, fp);
	#define WRITES(x) fwrite(&(x), sizeof(unsigned short), 1, fp);
	void writeDAT(FILE *fp) {
		int n;
		n = _region.size();
		char buffer[256];
		strncpy(buffer, _region.c_str(), 63);
		buffer[63] = 0;
		fwrite(buffer, 64, 1, fp);
		strncpy(buffer, _source_file.c_str(), 191);
		buffer[191] = 0;
		fwrite(buffer, 192, 1, fp);
		WRITEI(_source_index);
		checkValid();
		double d;
		d = _ref_lla.latitude();
		WRITED(d);
		d = _ref_lla.longitude();
		WRITED(d);
		WRITED(_lat);
		WRITED(_lon);
		WRITEI(_datum);
		WRITEI(_zone);
		WRITEI(_level);
		WRITEI(_pttrn);
		WRITEI(_coord);
		WRITEI(_sides);
		WRITED(_xcor[0]);
		WRITED(_xcor[1]);
		WRITED(_xcor[2]);
		WRITED(_xcor[3]);
		WRITED(_ycor[0]);
		WRITED(_ycor[1]);
		WRITED(_ycor[2]);
		WRITED(_ycor[3]);
		WRITED(_resolu[0]);
		WRITED(_resolu[1]);
		WRITED(_resolu[2]);
		WRITEI(_gnd_units);
		WRITEI(_elev_units);
		WRITEI(_bias);
		WRITEI(_scale);
		WRITEI(_rows);
		WRITEI(_cols);
		FILE *log = (FILE*) fopen("dem2dat.log", "at");
		fprintf(log, "%s\t%d\t%f\t%f\t%d\t%d\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%f\t%f\t%f\t%d\t%d\t%d\t%d\n", _fn.c_str(), _source_index, _lat, _lon, _datum, _zone, _xcor[0], _ycor[0], _xcor[1], _ycor[1], _xcor[2], _ycor[2], _xcor[3], _ycor[3], _resolu[0], _resolu[1], _resolu[2], _gnd_units, _elev_units, _scale, _cols);
		fclose(log);
	}
	#undef WRITED
	#undef WRITEI
	#undef WRITES

};


struct ElevationRow {  // un-nested from ElevationMap for swig...
	std::vector<unsigned short> elev;
	double E, N;
};


/**
 * Class encapsulating the elevation map data of a DEM file, providing
 * conversion to DAT format.
 */
class ElevationMap {
public:
	std::vector<ElevationRow> const &getMap() const { return map; }
	typedef std::vector<ElevationRow>::const_iterator MapIterator;
protected:
	std::vector<ElevationRow> map;
};

class DEM: public MetaDEM, public ElevationMap {
public:
	DEM() {}
	
	void open(std::string fn, int index) {
		FILE *fp = (FILE*) fopen(fn.c_str(), "rt");
		assert(fp);
		fb.attach(fp);
		parse();
		fclose(fp);
		derived();
		_source_index = index;
		_source_file = fn;
	}

	void derived() {
		_bbox0 = std::min(std::min(std::min(_xcor[0], _xcor[1]), _xcor[2]), _xcor[3]);
		_bbox1 = std::min(std::min(std::min(_ycor[0], _ycor[1]), _ycor[2]), _ycor[3]);
		_bbox2 = std::max(std::max(std::max(_xcor[0], _xcor[1]), _xcor[2]), _xcor[3]);
		_bbox3 = std::max(std::max(std::max(_ycor[0], _ycor[1]), _ycor[2]), _ycor[3]);
		int x = _title.find("-");
		assert(x > 0);
		_region = std::string(_title, 0, x);
		double la0=0.0, la1=0.0, lo0=0.0, lo1=0.0;
		_lat = 0.0;
		_lon = 0.0;
		_scale = 0;
		x = _title.find("LAT::");
		sscanf(_title.c_str()+x, "LAT:: %lf %lf %lf", &la0, &la1, &_lat);
		x = _title.find("LONG::");
		sscanf(_title.c_str()+x, "LONG:: %lf %lf %lf", &lo0, &lo1, &_lon);
		x = _title.find("SCALE::");
		sscanf(_title.c_str()+x, "SCALE:: %d", &_scale);
		_lat = la0 + la1/60.0 + _lat / 3600.0;
		_lon = lo0 + lo1/60.0 + _lon / 3600.0;
		_title = std::string(_title, 0, _title.find_last_not_of(" \t\r\n"));
	}

	#define SET(x, y, v) { int index=int(sx*(x-x0))+512*int(sy*(y-y0)); if (index>=0 && index < 512*512) image[index] = v;}
	void dumpPGM() {
		int i, j, k;
		unsigned char *image;
		image = new unsigned char[512*512];
		std::cerr << "P2 512 512 255\n";
		memset(image, 512*512, 0);
		double x0, y0, x1, y1;
		x0 = _bbox0 - 100;
		y0 = _bbox1 - 100;
		x1 = _bbox2 + 100;
		y1 = _bbox3 + 100;
		double sx = 512.0 / (x1 - x0);
		double sy = 512.0 / (y1 - y0);
		SET(_xcor[0], _ycor[0], 255);
		SET(_xcor[1], _ycor[1], 255);
		SET(_xcor[2], _ycor[2], 255);
		SET(_xcor[3], _ycor[3], 255);
		for (j = 0; j < _cols; j++) {
			double x = map[j].E;
			double y = map[j].N;
			std::vector<unsigned short>::iterator u, v;
			u = map[j].elev.begin();
			v = map[j].elev.end();
			for (; u != v; u++) {
				y += _resolu[1];
				SET(x, y, int(0.1* *u) & 255);
			}
		}
		for (i = 0; i < 512*512; i++) {
			std::cerr << int(image[i]) << " ";
			if (i % 6 == 0) std::cerr << std::endl;
		}
		delete[] image;
	}

	#define WRITED(x) fwrite(&(x), sizeof(double), 1, fp);
	#define WRITEI(x) fwrite(&(x), sizeof(int), 1, fp);
	#define WRITES(x) fwrite(&(x), sizeof(unsigned short), 1, fp);
	void saveDAT() {
		FILE *fp;
		fp = openDAT("wb"); // XXX check overwrite
		MetaDEM::writeDAT(fp);
		for (int i = 0; i < _cols; i++) {
			int n = map[i].elev.size();
			WRITED(map[i].E);
			WRITED(map[i].N);
			WRITEI(n);
			std::vector<unsigned short>::iterator itr, end;
			itr = map[i].elev.begin();
			end = map[i].elev.end();
			for (; itr != end; itr++) WRITES(*itr);
		}
		fclose(fp);
	}
	#undef WRITED
	#undef WRITEI
	#undef WRITES

	
protected:

	void parse() {
		int i;
		fb.getData();
		_title = std::string(fb.buffer(144), 0, 144);
		sscanf(fb.buffer(24), "%6d%6d%6d%6d", &_level, &_pttrn, &_coord, &_zone);
		for (i = 0; i < 15; i++) {
			double v = fb.getd(24);
			_proj[i] = v;
		}
		sscanf(fb.buffer(18), "%6d%6d%6d", &_gnd_units, &_elev_units, &_sides);
		for (i = 0; i < 4; i++) {
			double v;
			v = fb.getd(24);
			_xcor[i] = v;
			v = fb.getd(24);
			_ycor[i] = v;
		}
		_dirty = true;
		fb.d2e(24*3);
		sscanf(fb.buffer(24*3+6), "%24le%24le%24le%6d", &_min_value, &_max_value, &_phi, &_accu);
		for (i = 0; i < 3; i++) {
			double v = fb.getd(12);
			_resolu[i] = v;
		}
		sscanf(fb.buffer(12), "%6d%6d", &_rows, &_cols);
		fb.advance(26);
		sscanf(fb.buffer(2), "%2d", &_datum);
		readElevations();
	}

	void readElevations() {	
		fb.getData();
		map.reserve(_cols);
		for (int c = 0; c < _cols; c++) {
			int _r, _c, _n;
			fb.eatSpaces();
			fb.getData(256);
			sscanf(fb.buffer(), "%d", &_r);
			fb.eatDigits();
			sscanf(fb.buffer(12), "%6d%6d", &_c, &_n);
			fb.advance(6);
			fb.d2e(24*5);
			ElevationRow e;
			e.elev.reserve(_n);
			sscanf(fb.buffer(2*24), "%24le%24le", &e.E, &e.N);
			fb.advance(3*24);
			for (int j = 0; j < _n; j++) {
				int z = fb.getFastInt() + BIAS;
				//sscanf(buff+idx, "%6d", &z);
				if (z < 0) z = 0;
				e.elev.push_back(static_cast<unsigned short>(z));
				if (j % 5 == 0) fb.getData(128);
			}
			map.push_back(e);
		}
	}

	FileBuffer fb;
};



/**
 * Class encapsulating a DAT file.
 */
class DAT: public MetaDEM, public ElevationMap {
	double _ref_lat;
	double _ref_lon;

public:
	DAT() {}

	static std::string DATname(double lat, double lon) {
		int l0 = int((RadiansToDegrees(lat)+90)*8+0.5);
		int l1 = int((RadiansToDegrees(lon)+180)*8+0.5);
		char fn[60];
		sprintf(fn, "%04d_%04d.dat", l0, l1);
		return fn;
	}
	
	bool read(std::string fn) {
		FILE *fp = (FILE*) fopen(fn.c_str(), "rb");
		if (!fp) return false;
		_read(fp);
		fclose(fp);
	}

	bool read(double lat, double lon) {
		read(DATname(DegreesToRadians(lat), DegreesToRadians(lon)));
	}

	inline double getReferenceLatitude() const { return _ref_lat; }
	inline double getReferenceLongitude() const { return _ref_lon; }

protected:
	#define READD(x) fread(&(x), sizeof(double), 1, fp);
	#define READI(x) fread(&(x), sizeof(int), 1, fp);
	#define READS(x) fread(&(x), sizeof(unsigned short), 1, fp);
	#define READN(x,n) fread(x, sizeof(char), n, fp);
	void _read(FILE *fp) {
		char buffer[256];
		READN(buffer, 64);
		_region = buffer;
		_title = _region;
		READN(buffer, 192);
		_source_file = buffer;
		READI(_source_index);
		READD(_ref_lat);
		READD(_ref_lon);
		READD(_lat);
		READD(_lon);
		READI(_datum);
		READI(_zone);
		READI(_level);
		READI(_pttrn);
		READI(_coord);
		READI(_sides);
		READD(_xcor[0]);
		READD(_xcor[1]);
		READD(_xcor[2]);
		READD(_xcor[3]);
		READD(_ycor[0]);
		READD(_ycor[1]);
		READD(_ycor[2]);
		READD(_ycor[3]);
		READD(_resolu[0]);
		READD(_resolu[1]);
		READD(_resolu[2]);
		READI(_gnd_units);
		READI(_elev_units);
		READI(_bias);
		READI(_scale);
		READI(_rows);
		READI(_cols);
		map.resize(_cols);
		for (int i = 0; i < _cols; i++) {
			int n;
			READD(map[i].E);
			READD(map[i].N);
			READI(n);
			std::vector<unsigned short>::iterator itr, end;
			map[i].elev.resize(n);
			itr = map[i].elev.begin();
			end = map[i].elev.end();
			for (; itr != end; itr++) READS(*itr);
		}
	}
	#undef READD
	#undef READI
	#undef READS
	#undef READN

};

#endif // __DEM2DAT__

