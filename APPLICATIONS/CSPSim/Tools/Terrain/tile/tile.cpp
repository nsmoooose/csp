// Combat Simulator Project - Terrain Tools
// Copyright (C) 2002, 2003 The Combat Simulator Project
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
 * @file tile.cpp
 *
 * A utility for generating terrain tiles from digital elevation data.
 *
 **/


#include <SimData/GeoPos.h>
#include <SimData/Matrix3.h>
#include <SimData/Random.h>
#include <SimData/Math.h>
#include <SimData/FileUtility.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <zlib.h>

using namespace simdata;


FILE *LOG;

void ERROR(std::string const &msg) {
	std::cerr << msg << std::endl;
	::exit(1);
}




class Restriction {
public:
	virtual bool ok(double lat, double lon) const = 0;
	virtual bool ok(UTM const &utm) const = 0;
};


class GeodesicRestriction: public Restriction {
	double _lat0, _lat1;
	double _lon0, _lon1;
	GeodesicRestriction();
public:
	GeodesicRestriction(double lat0, double lon0, double lat1, double lon1) {
		_lat0 = simdata::toRadians(lat0);
		_lon0 = simdata::toRadians(lon0);
		_lat1 = simdata::toRadians(lat1);
		_lon1 = simdata::toRadians(lon1);
		assert(lat1 >= lat0);
		assert(lon1 >= lon0);
	}
	bool ok(double lat, double lon) const {
		return (lat >= _lat0 && lat < _lat1 && lon >= _lon0 && lon < _lon1);
	}
	bool ok(UTM const &utm) const {
		LLA lla(utm, GeoRef::NAD27);
		return ok(lla.latitude(), lla.longitude());
	}
};


/**
 * ElevationFinder compiles multiple elevation samples to produce
 * a local average elevation.  This is primarily intended to deal
 * with grid irregularities at quad edges, but can be extended to
 * do subsampling.
 */
class ElevationFinder {
	double lat, lon;
	int n_parts;
	int hint[9];
	bool hinted;
	double distance_squared_scale;
	std::vector<double> e_part;
	std::vector<double> d2_part;
	UTM utm, adjusted;
public:
	/**
	 * Construct a new ElevationFinder.
	 *
	 * @param utm_ the UTM coordinates of the point of interest.
	 * @param length the range of influence for each elevation sample.
	 */
	ElevationFinder(UTM const &utm_, double length) {
		utm = utm_;
		n_parts = 0;
		for (int i = 0; i < 9; i++) hint[i] = 0;
		hinted = false;
		distance_squared_scale = 1.0 / (length*length);
	}

	/**
	 * Get the UTM coordinate of the finder, adapted to the UTM
	 * zone of the current quad.  The resulting coordinates may
	 * not be strictly valid, but are useful in dealing with
	 * quad edges at zone boundaries.
	 */
	UTM const &getUTM(int zone) { 
		if (zone != utm.zone()) {
			LLA lla(utm, GeoRef::NAD27);
			adjusted = UTM(lla, GeoRef::NAD27, zone);
			return adjusted;
		} else {
			return utm; 
		}
	}

	/**
	 * Returns true if this quad contains the point.  Returns
	 * false during calls to neighboring (hinted) quads.
	 */
	bool isPrimary() const {
		return !hinted;
	}

	/**
	 * Get the elevation by computing a weighted sum of elevation
	 * samples from surrounding points.
	 */
	double getElevation() {
		int i;
		double scale = 0.0;
		double e = 0.0;
		bool all_far = false;
		if (n_parts == 0) {
			std::cout << "no samples @ " << lat << ", " << lon << "\n";
			return 0.0;
		}
		//std::cout << ":> ";
		for (i = 0; i < n_parts; i++) {
			if (e_part[i] <= 0.0) continue;
			if (e_part[i] > 5000.0) continue;
			double d = distance_squared_scale * d2_part[i];
			double weight = 1.0 / (1.0 + d);
			scale += weight;
			e += e_part[i] * weight;
			if (d <= 1.0) all_far = false;
		//	std::cout << e_part[i] << "," << weight << "; ";
		}
		//std::cout << "\n";
		//::exit(1);
		if (all_far) {
			for (i = 0; i < n_parts; i++) {
				std::cout << e_part[i] << "   " << d2_part[i]*distance_squared_scale << "\n";
			}
			std::cout << "----\n";
		}
		if (scale == 0.0) return 0.0;
		return e / scale;
	}

	/**
	 * Add an elevation sample.
	 *
	 * @param e the elevation in meters
	 * @praam d2 the squared distance from the elevation sample to the 
	 *           point.
	 */
	void setPartialElevation(double e, double d2) {
		//std::cout << "setE " << e << ", " << d2 << "\n";
		assert(e_part.size() == n_parts);
		e_part.push_back(e);
		d2_part.push_back(d2);
		n_parts++;
	}

	/**
	 * Suggest neighhboring quads to check for elevation samples.  This 
	 * is called by the primary quad when the point is near an edge and 
	 * may be close to samples in neighboring quads.
	 */
	void setHint(int dx, int dy) {
		if (hinted) return;
		assert(dx >= -1 && dx <= 1 && dy >= -1 && dy <= 1);
		hint[dx+3*dy+4]++;
	}

	/**
	 * Gets the next hint of other quads to check for elevation samples. 
	 *
	 * @param dx the eastward position of the next hinted quad relative 
	 *           to the primary quad.
	 * @param dy the northward position of the next hinted quad relative 
	 *           to the primary quad.
	 * @returns true if no more hints remain.
	 */
	bool getHint(int &dx, int &dy) {
		hinted = true;
		int i;
		for (i = 0; i < 9; i++) {
			if (hint[i]>0 && hint[i]<100 && i!=4) {
				hint[i] = 100;
				dx = (i % 3) - 1;
				dy = (i / 3) - 1;
				return true;
			}
		}
		return false;
	}
};

/**
 * Digital Elevation Model class.
 *
 * Reads DEM data from a DAT file and provides elevation sampling and edge
 * matching methods.
 */
class DEM {
	class DEMcol {
	public:
		static int DEMcol_count;
		DEMcol() {
			elev = 0;
			ref = 0;
		}
		DEMcol(DEMcol const &x) {
			*this = x;
		}
		DEMcol const &operator=(DEMcol const &x) {
			E = x.E;
			N = x.N;
			n = x.n;
			elev = x.elev;
			ref = x.ref;
			if (ref != 0) (*ref)++;
			return *this;
		}
		~DEMcol() {
			if (ref != 0) {
				(*ref)--;
				if (*ref == 0) {
					assert(elev);
					DEMcol_count--;
					delete[] elev;
					delete ref;
				}
			}
		}
		void setSize(int count) {
			n = count;
			assert(elev==0);
			assert(n<32768);
			elev = new unsigned short[n];
			//std::cout << n << "\n";
			ref = new int(1);
			DEMcol_count++;
		}
		double E, N;
		int n;
		unsigned short *elev;
		int *ref;
	};

	DEM();

	static int DEM_count;
public:
	#define READD(a) gzread(fp, &(a), sizeof(double));
	#define READI(a) gzread(fp, &(a), sizeof(int));
	/**
	 * Construct a new DEM, loading data from the appropriate DAT file.
	 *
	 * @la latitude contained in the quad (in degrees)
	 * @lo longitude contained in the quad (in degrees)
	 */
	DEM(double la, double lo, std::string const &datpath): zero(true) {
		DEM_count++;
		//std::cout << DEM_count << "\n";
		char fn[80];
		//sprintf(fn, "%d.dat.complete", i);
		//FILE *exists = (FILE*) fopen(fn, "rt");
		//if (exists==0) return;
		//fclose(exists);
		int lat_idx = int((la+90) * 8);
		int lon_idx = int((lo+180) * 8); 
		sprintf(fn, "%04d_%04d.dat.gz", lat_idx, lon_idx);
		bool show = false;
		//std::cout << fn << "\n";
		filename = simdata::ospath::join(datpath, fn);
		gzFile fp = gzopen(filename.c_str(), "rb");
		if (fp==0) {
			fprintf(LOG, "missing quad %s (%f %f)\n", filename.c_str(), la, lo);
			return;
		}
		int source_index;
		char buffer[256];
		gzread(fp, buffer, 256);
		READI(source_index);
		gzread(fp, buffer, 2*sizeof(double));
		READD(lat);
		//std::cout << la << ", " << lo << " ok\n";
		READD(lon);
		//std::cout << lat << ", " << lon << " got\n";
		READI(datum);
		READI(zone);
		gzread(fp, buffer, 4*sizeof(int));
		READD(cor0.x());
		READD(cor1.x());
		READD(cor2.x());
		READD(cor3.x());
		READD(cor0.y());
		READD(cor1.y());
		READD(cor2.y());
		READD(cor3.y());
		//std::cout << cor0.x() << ", " << cor0.y() << " utm\n";
		READD(res.x());
		READD(res.y());
		READD(res.z());
		READI(gnd_units);
		READI(elev_units);
		READI(bias);
		READI(scale);
		int n_row;
		READI(n_row);
		int n_col;
		READI(n_col);
		if (res.x() != 30.0 || res.y() != 30.0 || res.z() != 1.0) {
			//std::cout << "10m\n";
			//gzclose(fp);
			//return;
		}
		cols.reserve(n_col);
		//std::cout << "COLUMNS = " << n_col << "\n";
		for (int i = 0; i < n_col; i++) {
			DEMcol d;
			int n;
			READD(d.E);
			READD(d.N);
			READI(n);
			if (n > 10000) {
				std::cout << "Column count exceeds 10000\n";
				std::cout << "  > " << d.E << " " << d.N << " " << n << "\n";
				std::cout << "  > " << i << " of " << n_col << "\n";
				std::cout << "  > " << filename << "\n";
				std::cout << "  > " << res.x() << " " << res.y() << " " << res.z() << "\n";
			}
			if (n > 0) {
				//std::cout << "ROWS = " << n << " " << d.E << " " << res.x() << "\n";
				d.setSize(n);
				gzread(fp, d.elev, n * sizeof(unsigned short));
			}
			cols.push_back(d);
			//for (int j=0; j<n; j++) std::cout << d.elev[j] << " ";
			//std::cout << "\n";
		}
		gzclose(fp);
		if (lat_idx == 1028 && lon_idx == 483) {
			//std::cout << "EASTING0 = " << cols.begin()->E << "\n";
		}
		if (lat < -90.0 || lat > 90.0 || fabs(lat - la) > 1.0) {
			std::cout << "nomatch\n";
			return;
		}
		//std::cout << "OK\n";
		zero = false;
		if (show) std::cout << "loaded ok\n";
		gscale = 1.0;
		if (gnd_units == 1) gscale *= 12 * 0.0254;
		if (fabs(cols.begin()->E - cor0.x()) >= 500.0) {
			std::cout << "Problem with DAT input for quad " << filename << "\n";
			std::cout << "Corner0.x = " << cor0.x() << "\n";
			std::cout << "Cols[0].E = " << cols.begin()->E << "\n";
			std::cout << "Differ by more than 500 m, possible zone mismatch?\n";
			::exit(1);
		}
	}
	~DEM() {
		DEM_count--;
		//std::cout << DEM_count << " " << DEMcol::DEMcol_count << "\n";
	}

	inline int getZone() const { return zone; }
	inline bool isZero() const { return zero; }
	inline double getLatitude() const { return lat; }
	inline double getLongitude() const { return lon; }

	/**
	 * Find the nearest elevation samples within a column and add them to
	 * the finder (along with hints for neighboring quads to check).
	 */
	int addNearest(std::vector<DEMcol>::iterator &i, UTM const &utm, ElevationFinder &finder) {
		double fN = (utm.northing() - i->N) / (res.y()*gscale);
		double dN, dE = (utm.easting() - i->E);
		//std::cout << gscale << " " << fN << "  " << dE << "  <----\n";
		int idx = int(fN);
		//std::cout << ":\n";
		if (idx < -10 || idx >= i->n+10 || fabs(dE) > 10.0*res.x()) {
			//std::cout << "OUT_OF_RANGE " << dE << "dE " << idx << " " << i->E << "E " << utm.asString() << "\n";
			if (idx < -10) return -1;
			if (idx >= i->n+10) return 1;
			return 0;
		}
		if (idx < 0) {
			int xside = cor0.x() + cor3.x() - 2.0*utm.easting() > 0 ? -1 : 1;
			//std::cout << "DOWN\n";
			finder.setHint(0, -1);
			finder.setHint(xside, -1);
			dN = fN * res.y() * gscale;
			//std::cout << "C- " << fN << " " << dN << "  " << dE << std::endl;
			finder.setPartialElevation(_scale(i->elev[0]), dE*dE + dN*dN);
			return -1;
		}
		if (idx >= i->n-1) {
			//std::cout << "UP\n";
			int xside = cor0.x() + cor3.x() - 2.0*utm.easting() > 0 ? -1 : 1;
			finder.setHint(0, +1);
			finder.setHint(xside, +1);
			dN = (fN - (i->n-1)) * res.y() * gscale;
			//std::cout << "C+ " << (fN - (i->n-1)) << "|" << dN << "  " << dE << std::endl;
			finder.setPartialElevation(_scale(i->elev[i->n-1]), dE*dE + dN*dN);
			return +1;
		}
		dN = (fN - idx) * res.y() * gscale;
		//std::cout << "CA " << dN << "  " << dE << std::endl;
		finder.setPartialElevation(_scale(i->elev[idx]), dE*dE + dN*dN);
		dN = (fN - idx - 1.0) * res.y() * gscale;
		//std::cout << "CB " << dN << "  " << dE << std::endl;
		finder.setPartialElevation(_scale(i->elev[idx+1]), dE*dE + dN*dN);
		return -2;
	}

	/**
	 * Scale an elevation sample to meters above mean sea level.
	 */
	inline double _scale(double elevation) {
		double scale = res.z();
		if (elev_units == 1) scale *= 12*0.0254; // feet to meters
		return (elevation - bias) * scale;
	}

	/**
	 * Get elevation samples for the finder.  Currently this just searches
	 * the two nearest columns for at most 4 elevation samples (2 per column).
	 * Near the edges of a quad, fewer samples may be found but hints will be
	 * provided of other nearby quads to check for additional samples.
	 */
	void getElevation(ElevationFinder &finder) {
		bool debug = false; //!finder.isPrimary();
		if (zero) {
			return;
		}
		UTM const &utm = finder.getUTM(zone);
		if (debug) std::cout << utm.asString() << std::endl;
		std::vector<DEMcol>::iterator i, j;
		i = cols.begin();
		j = cols.end();
		assert(i != j);
		int col_idx = 0;
		double E = utm.easting();
		int t_idx = 0;
		for (; i!=j; i++) {
			if (i->E > E) break;
			col_idx++;
			t_idx++;
		}
		int yside = cor0.y() + cor1.y() - 2.0*utm.northing() > 0 ? -1 : 1;
		if (i == j) {
			//std::cout << "END\n";
			finder.setHint(+1, yside);
			finder.setHint(+1,  0);
			--i;
			addNearest(i, utm, finder);
			return;
		}
		if (i == cols.begin()) {
			//std::cout << "BEGIN " << utm.easting() << " FROM " << cols.begin()->E << "\n";
			//std::cout << "      " << utm.northing() << " FROM " << cols.begin()->N << "\n";
			finder.setHint(-1, yside);
			finder.setHint(-1,  0);
			addNearest(i, utm, finder);
			return;
		}
		//std::cout << "MID\n";
		//std::cout << E << " REL " << cols.begin()->E << " TIDX=" << t_idx << " OF " << cols.size() << "\n";
		//std::cout << cols.begin()->E << " " << filename << utm.asString() << "\n";
		int side = (col_idx > cols.size()/2) ? +1 : -1;
		int edge;
		edge = addNearest(i, utm, finder);
		if (edge != -2) {
			finder.setHint(side, 0);
			finder.setHint(side, edge);
		}
		--i;
		edge = addNearest(i, utm, finder);
		if (edge != -2) {
			finder.setHint(side, 0);
			finder.setHint(side, edge);
		}
		//std::cout << "~MID\n";
	}

	Vector3 res;
	Vector3 cor0, cor1, cor2, cor3;
	double scale;
	int zone;
	int datum;
	int bias;
	int gnd_units;
	int elev_units;
	double lat, lon;
	std::vector<DEMcol> cols;
	bool zero;
	double gscale;
	std::string filename;
};

int DEM::DEM_count = 0;
int DEM::DEMcol::DEMcol_count = 0;


class OutputHeightMap {
public:
	OutputHeightMap() {
		setDimensions(0, 0, 0.0, 0.0);
		setZ();
		_i = _j = 0;
		_left = _right = _bottom = _top = 0.0;
		_flip_x = _flip_y = false;
		_overlap = 0;
	}
	virtual ~OutputHeightMap() {}
	virtual void setDimensions(int num_rows, int num_cols, double width, double height) {
		_width = width;
		_height = height;
		_num_rows = num_rows;
		_num_cols = num_cols;
	}
	virtual void setOverlap(int overlap) {
		_overlap = overlap;
	}
	virtual void setZ(double z_scale = 1.0, double z_offset = 0.0) {
		_z_scale = z_scale;
		_z_offset = z_offset;
	}
	virtual void start(std::string const &prefix, int i, int j) {
		_i = i;
		_j = j;
		_prefix = prefix;
	}
	virtual void setExtentDegrees(double left, double right, double bottom, double top) {
		_left = simdata::toDegrees(left);
		_right = simdata::toDegrees(right);
		_bottom = simdata::toDegrees(bottom);
		_top = simdata::toDegrees(top);
	}
	virtual void setExtentMeters(double left, double right, double bottom, double top) {
		_left = left;
		_right = right;
		_bottom = bottom;
		_top = top;
	}
	virtual void setFlip(bool x, bool y) {
		_flip_x = x;
		_flip_y = y;
	}
	virtual void getCounts(int &lines, int &samples_per_line) {
		lines = _num_rows;
		samples_per_line = _num_cols;
	}
	virtual void finish() {}
	virtual void setSample(double z) = 0;
	double getWidth() const { return _width; }
	double getHeight() const { return _height; }
	virtual void getStart(double &x, double &y) {
		x = (_i - 0.5) * _width;
		y = (_j - 0.5) * _height;
		x += _flip_x ? _width : 0.0;
		y += _flip_y ? _height : 0.0;
	}
	virtual void getSampleStride(double &dx, double &dy) = 0;
	virtual void getLineStride(double &dx, double &dy) = 0;
	virtual std::string getFileName(std::string const &suffix = "") const {
		char idx[80];
		sprintf(idx, "%d-%d", _i, _j);
		return _prefix + idx + suffix;
	}
protected:
	double _left, _right, _bottom, _top;
	double _width, _height;
	int _num_rows, _num_cols;
	double _z_scale, _z_offset;
	int _i, _j;
	bool _flip_x, _flip_y;
	int _overlap;
	std::string _prefix;
};

class OutputPNM: public OutputHeightMap {
public:
	OutputPNM() { _image = 0; _idx = 0; _size = 0; }
	~OutputPNM() { if (_image) delete[] _image; }
	void getLineStride(double &dx, double &dy) {
		dx = 0.0;
		dy = _height / (_num_rows-_overlap);
		if (!_flip_y) dy = -dy;
	}
	void getSampleStride(double &dx, double &dy) {
		dx = _width / (_num_cols-_overlap);
		dy = 0.0;
		if (_flip_x) dx = -dx;
	}
	void start(std::string const &prefix, int i, int j) {
		OutputHeightMap::start(prefix, i, j);
		int n = _num_rows * _num_cols;
		assert(n > 0);
		if (_image && n != _size) {
			delete[] _image;
			_image = 0;
		}
		_size = n;
		if (!_image) _image = new int[n];
		_idx = _image;
		_count = 0;
	}
	void setSample(double z) {
		assert(++_count <= _size);
		*_idx++ = static_cast<int>(z*_z_scale + _z_offset);
	}
	virtual void getStart(double &x, double &y) {
		x = (_i - 0.5) * _width;
		y = (_j - 0.5) * _height;
		x += _flip_x ? _width : 0.0;
		y += !_flip_y ? _height : 0.0;
	}
protected:
	int *_image, *_idx;
	int _size, _count;
};


class OutputPGM: public OutputPNM {
public:
	void finish() {
		FILE *fp = (FILE*) fopen(getFileName(".pgm").c_str(), "wt");
		if (fp) {
			fprintf(fp, "P2 %d %d %d\n", _num_rows, _num_cols, 65535);
			_idx = _image;
			for (int n = _size; --n >=0; ) {
				fprintf(fp, "%d ", *_idx++);
				if (n % 8 == 0) { 
					fprintf(fp, "\n");
				}
			}
			fclose(fp);
		}
	}
};

class OutputPPM: public OutputPNM {
public:
	void outputSample(double z) {
		assert(++_count <= _size);
		int v = static_cast<int>(z*_z_scale + _z_offset);
		unsigned char *c = reinterpret_cast<unsigned char*>(_idx);
		*c++ = (v >> 16) & 0xff;
		*c++ = (v >> 8) & 0xff;
		*c++ = v & 0xff;
		_idx = reinterpret_cast<int*>(c);
	}
	void finish() {
		FILE *fp = (FILE*) fopen(getFileName(".ppm").c_str(), "wb");
		if (fp) {
			fprintf(fp, "P6 %d %d %d\n", _num_rows, _num_cols, 255);
			//fwrite(_image, _size, 3, fp);
			_idx = _image;
			unsigned char *buffer = new unsigned char[_size*3];
			assert(buffer != 0);
			unsigned char *out = buffer;
			bool warned = false;
			for (int n = _size; --n >=0; ) {
				int v = *_idx++;
				if (v<0 && !warned) {
					std::cerr << "WARNING: negative elevations not supported by Demeter.\n";
					warned = true;
				}
				if (v<0) v = 0; // XXX
				*out++ = (v>>16) & 0xff;
				*out++ = (v>>8) & 0xff;
				*out++ = v & 0xff;
			}
			fwrite(buffer, _size*3, 1, fp);
			delete[] buffer;
			fclose(fp);
		}
	}
};
class OutputBT: public OutputHeightMap {
public:
	struct Header {
		char magic[11];
		int cols;
		int rows;
		short data_size;
		short fp_flag;
		short h_units;
		short utm_zone;
		short datum;
		double left, right, bottom, top;
		short external_projection;
		float z_scale;
		Header() {
			strcpy(magic, "binterr1.1");
			cols = rows = 1;
			data_size = 2;
			fp_flag = 0;
			h_units = 1;
			utm_zone = 0;
			datum = 0;
			left = right = bottom = top = 0.0;
			z_scale = 1.0;
		}
		void write(FILE *fp) {
			fwrite(magic, 10, 1, fp);
			fwrite(&cols, 4, 1, fp);
			fwrite(&rows, 4, 1, fp);
			fwrite(&data_size, 2, 1, fp);
			fwrite(&fp_flag, 2, 1, fp);
			fwrite(&external_projection, 2, 1, fp);
			fwrite(&utm_zone, 2, 1, fp);
			fwrite(&datum, 2, 1, fp);
			fwrite(&left, 8, 1, fp);
			fwrite(&right, 8, 1, fp);
			fwrite(&bottom, 8, 1, fp);
			fwrite(&top, 8, 1, fp);
			char pad[196];
			memset(pad, 196, 0);
			fwrite(pad, 196, 1, fp);
		}
	};

	OutputBT() { 
		_size = 0; 
		_buffer_size = 32768;
		_buffer = new short[_buffer_size];
		assert(_buffer);
		_bindex = 0;
	}
	~OutputBT() { 
		if (_buffer != 0) {
			delete[] _buffer;
			_buffer = 0;
		}
		finish();
	}
	void getLineStride(double &dx, double &dy) {
		dx = _width / (_num_cols - _overlap);
		dy = 0.0;
		if (_flip_x) dx = -dx;
	}
	void getSampleStride(double &dx, double &dy) {
		dx = 0.0;
		dy = _height / (_num_rows - _overlap);
		if (_flip_y) dy = -dy;
	}
	virtual void getCounts(int &lines, int &samples_per_line) {
		lines = _num_cols;
		samples_per_line = _num_rows;
	}
	void start(std::string const &prefix, int i, int j) {
		OutputHeightMap::start(prefix, i, j);
		_size = _num_rows * _num_cols;
		_count = 0;
		_fp = (FILE*) fopen(getFileName(".bt").c_str(), "wb");
		Header h;
		h.rows = _num_rows;
		h.cols = _num_cols;
		h.left = _left;
		h.right = _right;
		h.bottom = _bottom;
		h.top = _top;
		h.write(_fp);
	}
	void finish() {
		if (_fp == 0) return;
		if (_bindex > 0) {
			fwrite(_buffer, sizeof(short), _bindex, _fp);
			_bindex = 0;
		}
		fclose(_fp);
		_fp = 0;
	}
	void setSample(double z) {
		assert(++_count <= _size);
		//short v = static_cast<short>(z*_z_scale + _z_offset);
		//fwrite(&v, sizeof(short), 1, _fp);
		_buffer[_bindex] = static_cast<short>(z*_z_scale + _z_offset);
		if (++_bindex >= _buffer_size) {
			fwrite(_buffer, sizeof(short), _bindex, _fp);
			_bindex = 0;
		}
	}
protected:
	int _size, _count;
	FILE *_fp;
	short *_buffer;
	int _bindex, _buffer_size;
};

/**
 * Tiler generates images of digital elevation data.  
 *
 * The source data is projected onto a plane using a secant 
 * gnomonic projection, and the resulting map is subdivided into 
 * a grid of smaller elevation maps saved in 16-bit PGM format.
 */
class Tiler {

public:
	enum {PGM, PPM, BT};

	Tiler() {
		prefix = "tile";
		center.setDegrees(0.0, 0.0);
		width = 10000.0;
		height = 10000.0;
		x_tiles = 1;
		y_tiles = 1;
		tile_x_size = 256;
		tile_y_size = 256;
		restrict = 0;
		z_scale = 10.0;
		z_offset = 32767;
		relief_depth = 0.0;
		output_format = PGM;
		flip_x = flip_y = 0;
		subsamples = 1;
		overlap = 0;
	}

	~Tiler() {
		setRestriction(0);
	}

	/**
	 * Read configuration setting from an ini file.
	 */
	bool initialize(char const *fn, char const *datpath, bool quiet=true) {
		_datpath = datpath;
		_quiet = quiet;
		if (fn == 0) return false;
		FILE *f = (FILE*) fopen(fn, "rt");
		if (f == 0) return false;
		char option[64];
		char value[128];
		float center_lat = 0.0;
		float center_lon = 0.0;
		bool ok = true;
		while (!feof(f)) {
			int n = fscanf(f, "%63s = %127s", option, value); 
			if (n == 2) {
				if (!strcmp(option, "center_latitude")) {
					center_lat = atof(value);
				} else
				if (!strcmp(option, "center_longitude")) {
					center_lon = atof(value);
				} else 
				if (!strcmp(option, "width")) {
					width = atof(value);
					if (width <= 0.0) {
						ERROR("width must be positive");
					}
				} else
				if (!strcmp(option, "height")) {
					height = atof(value);
					if (height <= 0.0) {
						ERROR("height must be positive");
					}
				} else 
				if (!strcmp(option, "tile_x_size")) {
					tile_x_size = atoi(value);
					if (tile_x_size <= 0) {
						ERROR("tile_x_size must be positive");
					}
				} else 
				if (!strcmp(option, "tile_y_size")) {
					tile_y_size = atoi(value);
					if (tile_y_size <= 0) {
						ERROR("tile_y_size must be positive");
					}
				} else 
				if (!strcmp(option, "x_tiles")) {
					x_tiles = atoi(value);
					if (x_tiles <= 0) {
						ERROR("x_tiles must be positive");
					}
				} else 
				if (!strcmp(option, "y_tiles")) {
					y_tiles = atoi(value);
					if (y_tiles <= 0) {
						ERROR("y_tiles must be positive");
					}
				} else 
				if (!strcmp(option, "z_scale")) {
					z_scale = atof(value);
					if (z_scale <= 0) {
						ERROR("z_scale must be positive");
					}
				} else 
				if (!strcmp(option, "z_offset")) {
					z_offset = atoi(value);
				} else 
				if (!strcmp(option, "relief_depth")) {
					relief_depth = atof(value);
				} else 
				if (!strcmp(option, "subsamples")) {
					subsamples = atoi(value);
				} else 
				if (!strcmp(option, "output_format")) {
					if (!strcmp(value, "PPM") || !strcmp(value, "ppm")) { 
						output_format = PPM;
					} else 
					if (!strcmp(value, "PGM") || !strcmp(value, "pgm")) { 
						output_format = PGM;
					} else 
					if (!strcmp(value, "BT") || !strcmp(value, "bt")) { 
						output_format = BT;
					} else {
						ERROR("unknown output format");
					}
				} else 
				if (!strcmp(option, "restrict")) {
					float a, b, c, d;
					int n = sscanf(value, "%f,%f,%f,%f", &a, &b, &c, &d);
					if (n == 4) {
						setRestriction(new GeodesicRestriction(a, b, c, d));
					} else {
						std::cerr << "Syntax error in restrict option (should be 'lat0,lon0,lat1,lon1').  No restriction set.\n";
					}
				} else 
				if (!strcmp(option, "prefix")) {
					prefix = value;
				} else 
				if (!strcmp(option, "flip_x")) {
					flip_x = atoi(value);
				} else 
				if (!strcmp(option, "flip_y")) {
					flip_y = atoi(value);
				} else 
				if (!strcmp(option, "overlap")) {
					overlap = atoi(value);
				} else {
					std::cerr << "Unrecognized ini option '" << option << "'\n";
					ok = false;
				}
			}
		}
		fclose(f);
		if (!ok) {
			std::cerr << "Aborted.\n";
			::exit(1);
		}
		// compute various projection parameters
		center.setDegrees(center_lat, center_lon);
		tile_width = width / x_tiles;
		tile_height = height / y_tiles;
		tile_dx = tile_width / (tile_x_size-overlap);
		tile_dy = tile_height / (tile_y_size-overlap);
		Matrix3 Ry, Rz;
		Rz.makeRotate(center.longitude(), Vector3::ZAXIS);
		Ry.makeRotate(-center.latitude(), Vector3::YAXIS);
		R_center = Rz * Ry; 
		R = 6371010.0; // nominal radius of the earth
		double angle = 0.5 * std::max(width, height) / R;
		double scale = 1.0 / cos(angle) / cos(angle);
		scale = 0.5 * (1.0 + scale) / scale;
		R *= scale; // secant correction
		prepareSampling();
		return true;
	}

	void setRestriction(Restriction *r) {
		if (restrict) delete restrict;
		restrict = r;
	}

	void prepareSampling() {
		simdata::random::Gauss g;
		g->setDistribution(0.0, 1.0);
		int i;
		double avg_x = 0.0;
		double avg_y = 0.0;
		if (subsamples < 1) subsamples = 1;
		for (i = 0; i < subsamples; i++) {
			double x = g.sample();
			double y = g.sample();
			x_subsamples.push_back(x);
			y_subsamples.push_back(y);
			avg_x += x;
			avg_y += y;
		}
		avg_x /= subsamples;
		avg_y /= subsamples;
		for (i = 0; i < subsamples; i++) {
			x_subsamples[i] -= avg_x;
			y_subsamples[i] -= avg_y;
		}
	}

	/**
	 * Generate the tiles.
	 */
	void run() {
		int i, j;
		int n = x_tiles * y_tiles;
		int idx = 0;
		tile = new short[tile_x_size*tile_y_size];
		OutputHeightMap *output = 0;
		switch (output_format) {
			case PGM:
				output = new OutputPGM();
				break;
			case PPM:
				output = new OutputPPM();
				break;
			case BT:
				output = new OutputBT();
				break;
			default:
				std::cerr << "Output image format not supported.\n";
				return;
		}
		output->setZ(z_scale, z_offset);
		output->setDimensions(tile_x_size, tile_y_size, tile_width, tile_height);
		output->setOverlap(overlap);
		output->setFlip(flip_x, flip_y);
		double left, right, bottom, top;
		project(-width*0.5, -height*0.5, left, bottom);
		project(+width*0.5, +height*0.5, right, top);
		//output->setExtentDegrees(left, right, bottom, top);
		output->setExtentMeters(-width*0.5, +width*0.5, -height*0.5, height*0.5);
		bool verbose = (n < 9) && !_quiet;
		for (i = 0; i < x_tiles; i++) {
			for (j = 0; j < y_tiles; j++) {
				if (!_quiet) {
					std::cout << "generating tile " << (idx+1) << " of " << n << std::endl;
				}
				output->start(prefix, i, j);
				//output->setExtent();
				generateTile(i, j, output, verbose);
				output->finish();
				/*
				switch (output_format) {
					case PGM:
						writeTilePGM(i, j, z_scale, z_offset);
						break;
					case PPM:
						writeTilePPM(i, j, z_scale, z_offset);
						break;
					default:
						std::cerr << "Output image format not supported.\n";
				}
				if (relief_depth > 0.0) {
					relief(relief_depth);
					writeTilePGM(i, j, 1.0, 32767, ".relief");
				}
				*/
				idx++;
			}
		}
		if (output != 0) delete output;
		//delete[] tile;
	}

private:

	/**
	 * Save a tile as a 16-bit portable graymap (PGM) image.
	 */
	void writeTilePGM(int i, int j, double scale, int offset, char const *id="") {
		char suffix[128];
		sprintf(suffix, "%d-%d%s.pgm", i, j, id);
		FILE *fp = (FILE*) fopen((prefix+suffix).c_str(), "wt");
		fprintf(fp, "P2 %d %d 65535\n", tile_x_size, tile_y_size);
		int n = tile_x_size*tile_y_size;
		int nl = 0;
		short *d = tile;
		while (--n >= 0) {
			int v = int(scale* *d++) + offset;
			fprintf(fp, "%d ", v);
			if (++nl%10==0) fprintf(fp, "\n");
		}
		fclose(fp);
	}

	/**
	 * Save a tile as a 24-bit portable pixmap (PPM) image, where
	 * elevations are represented as RED << 16 | GREEN << 8 | BLUE.
	 * Currently Demeter doesn't seem to support negative elevations,
	 * so make sure the offset is large enough to prevent these from
	 * occuring.
	 */
	void writeTilePPM(int i, int j, double scale, int offset, char const *id="") {
		char suffix[128];
		sprintf(suffix, "%d-%d%s.ppm", i, j, id);
		FILE *fp = (FILE*) fopen((prefix+suffix).c_str(), "wt");
		fprintf(fp, "P6 %d %d 255\n", tile_x_size, tile_y_size);
		bool warned = false;
		int n = tile_x_size*tile_y_size;
		int cnt = n;
		short *d = tile;
		unsigned char *buffer = new unsigned char[n*3];
		assert(buffer != 0);
		unsigned char *out = buffer;
		while (--cnt >= 0) {
			int v = int(scale* *d++) + offset;
			if (v<0 && !warned) {
				std::cerr << "WARNING: negative elevations not supported by Demeter.\n";
				warned = true;
			}
			if (v<0) v = 0; // XXX
			*out++ = (v>>16) & 0xff;
			*out++ = (v>>8) & 0xff;
			*out++ = v & 0xff;
		}
		fwrite(buffer, n*3, 1, fp);
		fclose(fp);
		delete[] buffer;
	}

	/**
	 * Generate a relief map (in-place) from the 16-bit elevation data
	 */
	void relief(double depth) {
		int n = tile_x_size*tile_y_size;
		int nl = 0;
		short *d = tile;
		short *e = d++;
		while (--n >= 1) {
			*e++ = short((*d++ - *e) * depth);
		}
	}

	/**
	 * Secant gnomonic (inverse) projection from the (x, y) plane 
	 * to latitude and longitude.
	 */
	void project(double x, double y, double &lat, double &lon) {
		Vector3 pos(R, x, y);
		pos = (R_center * pos).normalized();
		lon = atan2(pos.y(), pos.x());
		lat = asin(pos.z());
	}

	/**
	 * Get the DEM quad that contains a given latitude and longitude.  
	 * Quads are cached to minimize uncompressing and loading of the 
	 * data.
	 */
	DEM* getDEM(double lat, double lon) {
		lat *= 180.0/simdata::PI;
		lon *= 180.0/simdata::PI;
		int index = int((lat+90)*8)*10000 + int((lon+180)*8);
		//std::cout << lat << " " << lon << " " << index << std::endl;
		DEMmap::iterator i = dem.find(index);
		if (i != dem.end() && i->second != NULL) {
			DEM *d = i->second;
			// double check indexing
			UTM corner;
			corner.set(d->cor0.x(), d->cor0.y(), d->zone, 'X');
			LLA cor0(corner, GeoRef::NAD27);
			corner.set(d->cor2.x(), d->cor2.y(), d->zone, 'X');
			LLA cor2(corner, GeoRef::NAD27);
			if (!d->isZero() && !(lon+0.0001 >= cor0.longitude()*180.0/simdata::PI && lon-0.0001 <= cor2.longitude()*180.0/simdata::PI)) {
				std::cout << cor0.asString() << cor2.asString() << "\n";
				std::cout << lat << " " << lon << "\n";
				std::cout << d->filename << "\n";
				::exit(0);
			}
			return d; 
		}
		used.push_front(index);
		if (used.size() > 100) {
			i = dem.find(used.back());
			delete i->second;
			//dem[used.back()] = NULL;
			dem.erase(i);
			used.pop_back();
		}
		DEM *d = new DEM(lat, lon, _datpath);
		dem[index] = d;
		return d;
	}

	/**
	 * Get the elevation above sea level (in meters) for a given 
	 * point on the (x,y) plane.
	 */
	double getElevation(double x, double y) {
		int dx, dy;
		double lat, lon;
		project(x, y, lat, lon);
		if (restrict != 0 && !restrict->ok(lat, lon)) return 0.0;
		LLA lla(lat, lon);
		DEM* dem = getDEM(lat, lon);
		if (dem->isZero()) return 0.0;
		UTM utm(lla, GeoRef::NAD27);//, dem->getZone());
		ElevationFinder finder(utm, 60.0); // XXX fixme
		//std::cout << "----\n";
		dem->getElevation(finder);
		int quads = 1;
		while (finder.getHint(dx, dy)) {
			//std::cout << dx << " | " << dy << std::endl;
			dem = getDEM(lat + dy * 0.125 * simdata::PI / 180.0, lon + dx * 0.125 * simdata::PI / 180.0);
			//std::cout << "Looking for more " << lon << " - " << dx << ":" << dy << "\n";
			dem->getElevation(finder);
			quads++;
		}
		if (finder.getElevation() == 10000.0) {
			std::cout << "Got elevation = 10000.0\n";
			std::cout << quads << "\n";
			std::cout << utm.asString() << "\n";
			std::cout << lla.asString() << "\n";
			::exit(0);
		}
		return finder.getElevation();
	}

	/**
	 * Generate the projected elevation data for one tile.
	 */
	void generateTile(int tx, int ty, OutputHeightMap *output, bool verbose) {
		int i, j;
		double x, y, z;
		double x0, y0;
		double dx, dy;
		double dx0, dy0;
		int lines, samples_per_line;
		output->getStart(x0, y0);
		output->getSampleStride(dx, dy);
		output->getLineStride(dx0, dy0);
		output->getCounts(lines, samples_per_line);
		for (int line=0; line < lines; line++) {
			x = x0;
			y = y0;
			for (int s=samples_per_line; --s >= 0; ) {
				z = 0.0;
				int samples = 0;
				for (int k = 0; k < subsamples; k++) {
					double e = getElevation(x+x_subsamples[k]*tile_dx, y+y_subsamples[k]*tile_dy);
					if (e > -1000.0 && e < 9000.0) {
						z += e;
						samples++;
					}
				}
				if (samples > 0) z /= samples;
				output->setSample(z);
				x += dx;
				y += dy;
			}
			x0 += dx0;
			y0 += dy0;
			if (verbose && ((line+1) % 10 == 0)) {
				std::cout << "  line " << (line+1) << " of " << lines << std::endl;
			}
		}
	}

	void generateTile(int tx, int ty) {
		int i, j;
		double x, y, z;
		double x0;
		x0 = tx * tile_width - 0.5*width;;
		y = ty * tile_height - 0.5*height;
		int idx = 0;
		int row = tile_x_size * (tile_y_size-1);
		if (flip_y) row = 0;
		for (j = 0; j < tile_y_size; j++) {
			x = x0;
			idx = row;
			if (flip_y) {
				row += tile_x_size;
			} else {
				row -= tile_x_size;
			}
			if (flip_x) idx += tile_x_size;
			for (i = 0; i < tile_x_size; i++) {
				z = 0.0;
				int samples = 0;
				for (int k = 0; k < subsamples; k++) {
					double e = getElevation(x+x_subsamples[k]*tile_dx, y+y_subsamples[k]*tile_dy);
					if (e > -1000.0 && e < 9000.0) {
						z += e;
						samples++;
					}
				}
				if (samples > 0) z /= samples;
				if (flip_x) {
					tile[--idx] = (short) z;
				} else {
					tile[idx++] = (short) z;
				}
				x += tile_dx;
			}
			//std::cout << "line " << j << std::endl;
			y += tile_dy;
		}

	}
		
	Matrix3 R_center;
	std::string _datpath;
	bool _quiet;
	short *tile;
	int x_tiles, y_tiles;
	int tile_x_size, tile_y_size;
	double tile_dx, tile_dy;
	double tile_width, tile_height;
	LLA center;
	double width;
	double height;
	double z_scale;
	int z_offset;
	double R;
	typedef std::map<int, DEM*> DEMmap;
	DEMmap dem;
	std::list<int> used;
	std::string prefix;
	Restriction *restrict;
	int subsamples;
	std::vector<double> x_subsamples;
	std::vector<double> y_subsamples;
	int output_format;
	double relief_depth;
	int overlap;
	int flip_x, flip_y;
};

void usage(const char* arg0) {
	std::cerr << "Usage: " << arg0 << " [--help] [--datpath=path] [--quiet] [ini]\n";
}

void help(const char *arg0) {
	std::cerr << "\n";
	std::cerr << "CSP TerrainTiler (pre-version)\n";
	std::cerr << "Copyright 2003 The Combat Simulator Project <http://csp.sourceforge.net>\n";
	std::cerr << "\n";
	std::cerr << "This program generates projected terrain tiles from digital elevation data.\n";
	std::cerr << "The source files must be generated using 'dem2dat' and stored in the current\n";
	std::cerr << "directory.  The output tiles are saved as either 16-bit PGM format, PPM, or\n";
	std::cerr << "BT format.  Only one type of projection is currently supported, namely a\n";
	std::cerr << "secant gnomonic projection.\n";
	std::cerr << "\n";
	usage(arg0);
	std::cerr << "\n";
	std::cerr << "Options:\n";
	std::cerr << "             --help                 Display this message\n";
	std::cerr << "             --datpath=path         Path to the input quads\n";
	std::cerr << "\n";
	std::cerr << "The ini file contains lines of the form 'option = value'.  Options include:\n";
	std::cerr << "   center_latitude (in degrees)\n";
	std::cerr << "   center_longitude (in degrees)\n";
	std::cerr << "   width (in meters)\n";
	std::cerr << "   height (in meters)\n";;
	std::cerr << "   tile_width (integer, usually a power of 2)\n";
	std::cerr << "   tile_height (integer, usually a power of 2)\n";
	std::cerr << "   z_scale (float)\n";
	std::cerr << "   z_offset (integer in meters)\n";
	std::cerr << "   relief_depth (float)\n";
	std::cerr << "   output_format (pgm, ppm, or bt)\n";
	std::cerr << "   flip_x (0 or 1)\n";
	std::cerr << "   flip_y (0 or 1)\n";
	std::cerr << "   overlap (0 or 1)\n";
	std::cerr << "   subsamples (integer)\n";
	std::cerr << "   dat_path (path to input dat files)\n";
	std::cerr << "   prefix (the base name for output files)\n";
	std::cerr << "   restrict (latitude and longitude restrictions, in degrees)\n";
	std::cerr << "       e.g. '36.0,-120.0,42.0,-114.0'\n";
	std::cerr << "\n";
}

int main(int argc, char **argv) {
	const char *datpath = ".";
	bool quiet = false;
	LOG = (FILE*) fopen("tile.log", "wt");
	char *ini = 0;
	while (--argc >= 1) {
		char *arg = argv[argc];
		if (arg[0] == '-') {
			if (!strcmp(arg, "--help")) {
				help(argv[0]); 
				::exit(0);
			} else
			if (!strncmp(arg, "--datpath=", 10)) {
				datpath = arg+10;
				continue;
			} else
			if (!strcmp(arg, "--quiet")) {
				quiet = true;
				continue;
			}
			usage(argv[0]);
			::exit(1);
		} else {
			ini = arg;
		}
	}
	if (ini == 0) {
		usage(argv[0]);
		::exit(0);
	}
	Tiler tiler;
	if (tiler.initialize(ini, datpath, quiet)) {
		tiler.run();
	}
	if (LOG != 0) fclose(LOG);
}


