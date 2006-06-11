/* Combat Simulator Project
 * Copyright 2006 Mark Rose <mkrose@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

// Computes a static texture map for the reflection of the cockpit from
// the canopy.  Takes as input a text file with geometric data for the
// canopy reflection computed by map_reflection.  This data defines the
// canopy surface position and reflection vector from the pilot's view
// point for each pixel in the output texture.  The cockpit 3d model is
// rendered from each of these positions to determine the texel color.

#include <osg/Image>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgUtil/Optimizer>
#include <osgProducer/Viewer>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

// rendered texture size per output texel
const int IMAGE_SIZE = 16;


class Reflection {
public:
	Reflection(osgProducer::Viewer *viewer, std::string const &datfile) {
		viewer_ = viewer;
		loadDat(datfile);
	}

	void generate(std::string const &output_file, std::string const &preload, int continue_offset) {
		assert(continue_offset >= 0);

		Producer::Camera *camera = viewer_->getCamera(0);
		Producer::RenderSurface *pbuffer = camera->getRenderSurface();
		pbuffer->waitForRealize();

		osg::ref_ptr<osg::Image> image = new osg::Image;
		osg::ref_ptr<osg::Image> output;

		// if preload is set, load the specified file into the output image before
		// starting.  this, together with the continue_offset option  allows the
		// rendering to be resumed after being interupted.
		if (!preload.empty()) {
			output = osgDB::readImageFile(preload);
		}
		if (output.valid() && output->s() == _width && output->t() == _height) {
			std::cout << "using preloaded image\n";
		} else {
			output = new osg::Image;
			output->allocateImage(_width, _height, 1, GL_RGB, GL_UNSIGNED_BYTE);
			memset(output->data(), 0, _width * _height * 3);
		}

		std::cout << "starting rendering loop: " << points_.size() << " total frames, " << (points_.size() - continue_offset) << " frames to render\n";
		for (unsigned i = continue_offset; i < points_.size(); ++i) {
			Point const &p = points_[i];
			if (p.x >= _width || p.y >= _height) continue;

			// set up the camera for the current point.  the camera is placed
			// outside of the cockpit along the reflection vector, at the same
			// distance from the canopy surface as the pilot's viewpoint.
			Producer::Vec3 surface(p.px, p.py, p.pz);
			Producer::Vec3 direction(p.dx, p.dy, p.dz);
			direction.normalize();
			float distance = (surface - _eye).length();
			camera->setLensPerspective(p.fov * IMAGE_SIZE/2, p.fov * IMAGE_SIZE/2, 0.1, 10.0);
			Producer::Vec3 eye(surface - direction * distance);
			Producer::Vec3 look(surface);
			Producer::Vec3 up = (look ^ Producer::Vec3(0, 0, 1.0)) ^ look;
			Producer::Matrix vm;
			vm.makeLookAt(eye, look, up);
			viewer_->setViewByMatrix(vm);

			// render the cockpit
			viewer_->update();
			viewer_->frame();
			viewer_->sync();

			// compute the output color as an average over several pixels in the rendered image
			image->readPixels(0, 0, IMAGE_SIZE, IMAGE_SIZE, GL_RGB, GL_UNSIGNED_BYTE);
			{
				const int i0 = IMAGE_SIZE / 2;
				const int i1 = i0 - 1;
				// average over 4 pixels at the center of the rendered image.
				int r = int(*(image->data(i0,i0)+0)) + int(*(image->data(i1,i1)+0)) + int(*(image->data(i1,i0)+0)) + int(*(image->data(i0,i1)+0));
				int g = int(*(image->data(i0,i0)+1)) + int(*(image->data(i1,i1)+1)) + int(*(image->data(i1,i0)+1)) + int(*(image->data(i0,i1)+1));
				int b = int(*(image->data(i0,i0)+2)) + int(*(image->data(i1,i1)+2)) + int(*(image->data(i1,i0)+2)) + int(*(image->data(i0,i1)+2));
				*(output->data(p.x, p.y)+0) = r / 4;
				*(output->data(p.x, p.y)+1) = g / 4;
				*(output->data(p.x, p.y)+2) = b / 4;
			}

			if ((i % 10000) == 0) {
				std::cout << i << " frames\n";
			}
			if ((i % 40000) == 0) {
				std::cout << "saving intermediate output to " << output_file << "\n";
				osgDB::writeImageFile(*output, output_file);
			}
		}

		// clamp edge
		std::vector<int> edge(_height, _width - 1);
		for (unsigned i = 0; i < points_.size(); ++i) {
			Point const &p = points_[i];
			if (p.x >= _width || p.y >= _height) continue;
			if (edge[p.y] > p.x) edge[p.y] = p.x;
		}

		for (int i = 0; i < _height; ++i) {
			unsigned char *p = output->data(edge[i], i);
			for (int j = 0; j < edge[i]; ++j) {
				memcpy(output->data(j, i), p, 3);
			}
		}

		std::cout << "saving output to " << output_file << "\n";
		osgDB::writeImageFile(*output, output_file);
		std::cout << "done!\n";
	}

private:
	struct Point {
		int x, y;  // pixel coordinates in the output texture
		float px, py, pz;  // point on the canopy surface
		float dx, dy, dz;  // reflection direction
		float fov;  // approimate field of view in degrees
	};

	// the dat file format is:
	//   eye_x eye_y eye_z
	//   width height
	//   texel_x texel_y point_x point_y point_z dir_x dir_y dir_z fov
	//   texel_x texel_y point_x point_y point_z dir_x dir_y dir_z fov
	//   ...
	// where eye_* is the viewpoint in model coordinates, width and height
	// are the size of the output texture in pixels, texel_* are the
	// coordinates in the output texture [0,width), [0,height), point_*
	// is the corresponding point on the canopy surface, dir is the
	// reflection direction, and fov is an upper bound on the field of view
	// of the texel in degree.
	void loadDat(std::string const &file) {
		std::cout << "reading input data from " << file << "\n";
		std::ifstream dat(file.c_str());
		assert(!!dat);
		dat >> _eye.x() >> _eye.y() >> _eye.z();
		dat >> _width >> _height;
		assert(_width > 0 && _height > 0);
		while (!!dat) {
			Point p;
			dat >> p.x >> p.y >> p.px >> p.py >> p.pz >> p.dx >> p.dy >> p.dz >> p.fov;
			points_.push_back(p);
		}
		std::cout << "read " << points_.size() << " points\n";
	}

	osgProducer::Viewer *viewer_;
	std::vector<Point> points_;
	Producer::Vec3 _eye;
	int _width;
	int _height;
};


int main( int argc, char **argv ) {
    osg::ArgumentParser args(&argc, argv);
	args.getApplicationUsage()->setCommandLineUsage(args.getApplicationName() + " [option] model.osg");
	args.getApplicationUsage()->addCommandLineOption("--input <filename>", "data file generated by map_reflection (default reflect.dat)");
	args.getApplicationUsage()->addCommandLineOption("--continue <offset>", "continue rendering starting at the specified pixel number");
	args.getApplicationUsage()->addCommandLineOption("--preload <filename>", "preload an existing reflection image; for use with --continue");
	args.getApplicationUsage()->addCommandLineOption("--output <filename>", "output file (default reflect.jpg)");
	args.getApplicationUsage()->addCommandLineOption("-h or --help", "print usage message");

	std::string input = "reflect.dat";
	std::string output = "reflect.jpg";
	std::string preload;
	int continue_offset = 0;

	args.read("--input", input);
	args.read("--output", output);
	args.read("--preload", preload);
	args.read("--continue", continue_offset);

	if (args.read("-h") || args.read("--help")) {
		args.getApplicationUsage()->write(std::cout, osg::ApplicationUsage::COMMAND_LINE_OPTION);
		return 1;
	}

    osgProducer::Viewer viewer;
    viewer.setUpViewer(osgProducer::Viewer::STANDARD_SETTINGS);

    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFiles(args);
    if (!loadedModel) {
        std::cerr << args.getApplicationName() << ": model not loaded\n";
        return 1;
    }

    args.reportRemainingOptionsAsUnrecognized();
    if (args.errors()) {
        args.writeErrorMessages(std::cerr);
    }

    osgUtil::Optimizer optimizer;
    optimizer.optimize(loadedModel.get());

    viewer.setSceneData(loadedModel.get());
	viewer.getCamera(0)->getRenderSurface()->setWindowRectangle(0, 0, IMAGE_SIZE, IMAGE_SIZE);
	viewer.setClearColor(osg::Vec4(0, 0, 0, 1));

    viewer.realize();

	Reflection reflection(&viewer, input);
	reflection.generate(output, preload, continue_offset);

    viewer.cleanup_frame();
    viewer.sync();
    return 0;
}

