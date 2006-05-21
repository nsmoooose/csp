// CSPLayout
// Copyright 2003, 2005 Mark Rose <mkrose@users.sourceforge.net>
//
// Based in part on osgpick sample code
// OpenSceneGraph - Copyright (C) 1998-2003 Robert Osfield
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, US

#ifndef __CSP_LAYOUT_VIEW_MANIPULATOR_H__
#define __CSP_LAYOUT_VIEW_MANIPULATOR_H__

#include "LayoutNodes.h"
#include "FeatureGraph.h"
#include "InsertDeleteCommand.h"
#include "View.h"
#include "Undo.h"

#include <osg/io_utils>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIActionAdapter>

#include <cmath>

/** Helper class for tracking changes to a layout node during manipulation.
 *  Stores the original matrix transform to allow the change to be cancelled.
 */
struct ManipulatedNode {
	LayoutNode * const node;
	osg::Matrix const old_matrix;
	typedef std::list<ManipulatedNode> List;

	ManipulatedNode(LayoutNode* feature_node): node(feature_node), old_matrix(node->getMatrix()) { }

	/** Set the layout node back to its original state.
	 */
	void cancel() { node->setMatrix(old_matrix); }

	/** Move the layout node to the specified (x,y) offset relative to the original
	 *  position.
	 */
	void moveBy(double x, double y) {
		osg::Matrix move = osg::Matrix::translate(osg::Vec3d(x, y, 0)) * old_matrix;
		node->setMatrix(move);
	}

	/** Rotate the layout node by the specified angle relative to the original orientation.
	 */
	void rotateBy(double angle) {
		osg::Matrix rotate = old_matrix * osg::Matrix::rotate(angle, 0, 0, 1);
		node->setMatrix(rotate);
	}
};


/** A command subclass for tracking movement and rotation of one or more layout nodes.
 *  Allows repositioning and rotation of layout nodes to be undone/redone.
 */
class ManipulateCommand: public Command {
	struct NodeChange {
		LayoutNode *node;
		osg::Matrix old_matrix;
		osg::Matrix new_matrix;
	};
	std::vector<NodeChange> _changes;
	std::string _label;
public:
	ManipulateCommand(ManipulatedNode::List const &nodes, std::string const &label): _label(label) {
		_changes.reserve(nodes.size());
		for (ManipulatedNode::List::const_iterator iter = nodes.begin(); iter != nodes.end(); ++iter) {
			NodeChange change;
			change.node = iter->node;
			change.old_matrix = iter->old_matrix;
			change.new_matrix = change.node->getMatrix();
			_changes.push_back(change);
		}
	}
	virtual void undo() {
		for (unsigned i = 0; i < _changes.size(); ++i) {
			_changes[i].node->setMatrix(_changes[i].old_matrix);
		}
	}
	virtual void redo() {
		for (unsigned i = 0; i < _changes.size(); ++i) {
			_changes[i].node->setMatrix(_changes[i].new_matrix);
		}
	}
	virtual std::string label() const { return _label; }
};


/** View and layout manipulator class that is loosely based on osgGA::TrackballManipulator.
 *  The view point is fixed to the z=0 plane, and all layout manipulation occurs within that
 *  plane.  Supports two manipulation modes: translate and rotate.
 */
class ViewManipulator : public osgGA::TrackballManipulator {

	// Graph callback for capturing group activation events.  The ViewManipulator
	// keeps track locally of the active group and world-to-active-group transform.
	class GraphEventAdapter: public GraphCallback {
		ViewManipulator *_view_manipulator;
	public:
		GraphEventAdapter(ViewManipulator *vm): _view_manipulator(vm) { assert(vm); }
		virtual void onActivate(LayoutNodePath *path, osg::Matrix const &to_world) {
			assert(path && !path->empty());
			LayoutNode *last = (*path)[path->size() - 1];
			assert(last->isGroup());
			_view_manipulator->setActiveGroup(last->asGroup(), to_world);
		}
	};

	// Not everyone has a three-button mouse.
	enum {
		FAKE_MIDDLE_BUTTON = osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON|osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON
	};

	friend class View;

	/** Callback handler to track the active group and group transform.
	 */
	void setActiveGroup(LayoutGroup *group, osg::Matrix const &group_to_world) {
		if (_action) cancelAction();
		_active_group = group;
		_active_group_to_world = group_to_world;
		_active_group_from_world = osg::Matrix::inverse(group_to_world);
		std::cout << _active_group_from_world << "\n";
	}

	public:

	/** Layout manipulation modes.
	 */
	enum Mode {
		MOVE = 1,
		ROTATE = 2,
	};

	ViewManipulator(View *view): _view(view), _active_group(0), _mode(MOVE), _action(false), _graph_event_adapter_id(0) {
		_graph_event_adapter_id = _view->graph()->addCallback(new GraphEventAdapter(this));
		_active_group = _view->graph()->getActiveGroup();
	}

	~ViewManipulator() {
		disconnect();
	}

	void disconnect() {
		if (_view) {
			_view->graph()->removeCallback(_graph_event_adapter_id);
			_view = NULL;
		}
	}

	virtual const char * className() const { return "ViewManipulator"; }

	Mode getMode() const { return _mode; }

	void setMoveMode() {
		if (_action) cancelAction();
		_mode = MOVE;
		if (_view) _view->signalEditMode();
	}

	void setRotateMode() {
		if (_action) cancelAction();
		_mode = ROTATE;
		if (_view) _view->signalEditMode();
	}

	/** Get the current view point.
	 */
	osg::Vec3 center() const { return _center; }

	/** Get the distance from the camera to the view point.
	 */
	float distance() const { return _distance; }

	/** Move the view point and camera to focus on the specified node.
	 */
	void centerViewOnNode(LayoutNodePath *path, LayoutNode *node) {
		assert(path);
		osg::Matrix to_world = LayoutNode::getPathTransform(path);
		_distance = std::max<float>(50.0, 4.0 * node->getBound()._radius);
		_center = node->getBound()._center * to_world;
		_center.z() = 0.0;
	}

	/** Handle mouse and keyboard events.
	 */
	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
		int key = ea.getKey();
		switch (ea.getEventType()) {

			case (osgGA::GUIEventAdapter::PUSH):
				flushMouseEventStack();
				addMouseEvent(ea);
				// any button push cancels the current layout manipulation
				if (_action) cancelAction();
				if (ea.getButtonMask() == osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON || ea.getButtonMask() == FAKE_MIDDLE_BUTTON) {
					// the middle button controls the view
					calcMovement();
				} else if (ea.getButtonMask() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
					// the left button manipulates the layout
					beginAction();
				}
				return true;

			case (osgGA::GUIEventAdapter::MOVE):
				updateCoordinates(ea);
				break;

			case (osgGA::GUIEventAdapter::DRAG):
				addMouseEvent(ea);
				if (_action) {
					manipulateNodes();
				} else {
					manipulateViews();
				}
				return true;

			case (osgGA::GUIEventAdapter::RELEASE):
				if (_action) {
					if (ea.getButtonMask() == 0) {
						// only the left mouse button was used during the manipulation, so apply the changes.
						applyAction();
					} else {
						// sanity: shouldn't be able to get here due to constraints on starting an action.
						cancelAction();
					}
				} else {
					// finish view change
					flushMouseEventStack();
					calcMovement();
				}
				return true;

			case (osgGA::GUIEventAdapter::SCROLLUP):
				// use the mouse wheel to zoom in/out
				_distance *= 0.9;
				if (_distance < 2.0) _distance = 2.0;
				return true;

			case (osgGA::GUIEventAdapter::SCROLLDOWN):
				// use the mouse wheel to zoom in/out
				_distance *= 1.1;
				if (_distance > 1e+6) _distance = 1e+6;
				return true;

			case (osgGA::GUIEventAdapter::KEYDOWN):
				switch (key) {
					case 'z':
						if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) {
							// ctrl-z is undo last command
							if (_view) _view->graph()->undo();
						} else {
							// 'z' move the camera to 200 m directly over the view point.
							_rotation = osg::Quat();
							_distance = 200.0;
						}
						return true;

					case osgGA::GUIEventAdapter::KEY_Home:
						if (!_action) {
							flushMouseEventStack();
							_distance = 200.0;
							_rotation = osg::Quat();
							if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
								_center = osg::Vec3(0,0,0);
								// shift-home: focus on the active group
								if (_active_group) {
									_center = _active_group_to_world * osg::Vec3(0,0,0);
									_distance = std::max<float>(200.0, _active_group->getBound()._radius);
								}
							} else
							if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) {
								// ctrl-home: focus on the world origin
								_center = osg::Vec3(0,0,0);
							} else {
								// focus on the selected layout node, if any.
								// TODO really should find the center and bound of the full selection if
								// more than one object is selected.
								_center = osg::Vec3(0,0,0);
								if (_active_group) {
									LayoutNode *node = _active_group->getOnlySelectedChild();
									if (node) {
										_center = _active_group_to_world * node->getBound()._center;
										_distance = std::max<float>(200.0, node->getBound()._radius);
									} else {
										_center = _active_group_to_world * osg::Vec3(0,0,0);
										_distance = std::max<float>(200.0, _active_group->getBound()._radius);
									}
									_center.z() = 0.0;
								}
							}
							return true;
						}
						break;

					case 'b':
						//_box_select = true;
						break;

					case 'g':
						// 'g' switches to "move mode" (layout node translation)
						setMoveMode();
						return true;

					case 'r':
						if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) {
							// ctrl-r is redo last undone command
							if (_view) _view->graph()->redo();
						} else {
							// 'r' switches to "rotate mode" (layout node rotation)
							setRotateMode();
						}
						break;

					case 'x':
						// delete the current selection
						deleteSelectedNodes();
						break;

					case osgGA::GUIEventAdapter::KEY_Tab:
						// tab activates the current selection (assuming it is a layout group),
						// while shift-tab activates the parent of the current active group.
						if (!_action && _active_group) {
							if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
								if (_view) _view->graph()->activateParent();
							} else {
								LayoutNode *node = _active_group->getOnlySelectedChild();
								if (node && node->isGroup()) {
									if (_view) _view->graph()->activateChild(node->asGroup());
								}
							}
						}
						break;

					case osgGA::GUIEventAdapter::KEY_End:
						// make the top feature group the active group.
						if (!_action && _active_group) {
							if (_view) _view->graph()->activateRoot();
						}
						break;

					default:
						std::cout << "key = " << key << "\n";
				}

			default:
				break;
		}
		return false;
	}

protected:

	/** Move the camera in response to mouse input.
	 */
	virtual bool manipulateViews() {
		if (_view) _view->updatePosition(0, 0, 0, 0, 0, 0, 0);

		// return if less then two events have been added.
		if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

		float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
		float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

		// return if there is no movement.
		if (dx==0 && dy==0) return false;

		unsigned int buttonMask = _ga_t1->getButtonMask();
		if (buttonMask==osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON || buttonMask==FAKE_MIDDLE_BUTTON) {
			if (_ga_t1->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
				osg::Vec3d view_axis = _rotation * osg::Vec3d(0, 0, 1);
				osg::Vec3d view_right = _rotation * osg::Vec3d(1, 0, 0);
				double elevation = acos(view_axis.z());
				double azimuth = atan2(view_right.y(), view_right.x());
				elevation = std::max<float>(0.0, std::min<float>(1.5707, elevation - dy));
				azimuth += dx;
				_rotation = osg::Quat(elevation, osg::Vec3d(1,0,0)) * osg::Quat(azimuth, osg::Vec3d(0,0,1));
			} else {
				float scale = -0.5f * _distance;
				osg::Vec3 x_axis = _rotation * osg::Vec3(1, 0, 0);
				osg::Vec3 y_axis = osg::Vec3(0, 0, 1) ^ x_axis;
				osg::Vec3 motion = x_axis * dx + y_axis * dy;
				_center += motion* scale;
			}
			return true;
		}

		if (buttonMask==osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
			float scale = 1.0f+dy;
			_distance *= scale;
			if (_distance < 2.0) _distance = 2.0;
			if (_distance > 1e+6) _distance = 1e+6;
			return true;
		}

		return false;
	}

	/** Move or rotate the selected layout nodes in response to mouse input.
	 */
	bool manipulateNodes() {
		// return if less then two events have been added.
		if (_ga_t0.get()==NULL || _ga_t1.get()==NULL) return false;

		float dx = _ga_t0->getXnormalized()-_ga_t1->getXnormalized();
		float dy = _ga_t0->getYnormalized()-_ga_t1->getYnormalized();

		// return if there is no movement.
		if (dx==0 && dy==0) return false;

		float total_dx = _ga_t0->getXnormalized() - _action_start_x_normalized;
		float total_dy = _ga_t0->getYnormalized() - _action_start_y_normalized;

		const bool constrain = ((_ga_t0->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL) != 0);
		const bool grid = ((_ga_t0->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) != 0);
		const bool precision = ((_ga_t0->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT) != 0);

		// require a minimum amount of mouse movement before actually doing anything, which helps
		// to prevent (and notice) accidents while picking objects.
		if (!_action_trigger) {
			float pixel_x;
			float pixel_y;
			if (_view) _view->computePixelCoords(_ga_t0->getXnormalized(), _ga_t0->getYnormalized(), pixel_x, pixel_y);
			pixel_x -= _action_start_pixel_x;
			pixel_y -= _action_start_pixel_y;
			const float min_motion = 5.0f;  // pixels
			if ((pixel_x*pixel_x + pixel_y*pixel_y) < min_motion*min_motion) return false;
			_action_trigger = true;
		}

		osg::Matrix rotation_matrix;
		rotation_matrix.set(_rotation);

		if (_mode == MOVE) {
			float scale = 0.35f * _distance;
			if (precision) scale *= 0.2;
			osg::Vec3 x_axis = _rotation * osg::Vec3(1, 0, 0);
			osg::Vec3 y_axis = osg::Vec3(0, 0, 1) ^ x_axis;
			osg::Vec3 motion = (x_axis * total_dx + y_axis * total_dy) * scale;
			if (constrain) {
				if (!_action_constrain) {
					_action_constrain_x = (std::abs(motion.x()) > std::abs(motion.y()));
				}
				if (_action_constrain_x) {
					motion.y() = 0.0;
				} else {
					motion.x() = 0.0;
				}
			}
			if (grid) {
				float scale = (_view ? _view->getGridScale() : 1.0f);
				if (precision) scale *= 0.1;
				motion.x() = floor(motion.x()/scale + 0.5) * scale;
				motion.y() = floor(motion.y()/scale + 0.5) * scale;
			}

			// motion is in world coordinates, so we need to rotate back to active_group coordinates
			osg::Matrix to_local_coordinates(_active_group_from_world);
			to_local_coordinates.setTrans(0, 0, 0);
			motion = motion * to_local_coordinates;

			for (ManipulatedNode::List::iterator iter = _targets.begin(); iter != _targets.end(); ++iter) {
				iter->moveBy(motion.x(), motion.y());
			}

			if (_view) _view->updatePosition(0, 0, 0, 0, motion.x(), motion.y(), 0);
		} else if (_mode == ROTATE) {
			float scale = 2.0f;
			if (precision) scale *= 0.2;
			float x = _ga_t0->getXnormalized();
			float y = _ga_t0->getYnormalized();
			osg::Vec3 dv(scale * dx, scale * dy, 0.0);
			dv = dv * rotation_matrix;
			osg::Vec3 v(x, y, 0.0);
			v = v * rotation_matrix;
			v = v ^ dv;
			float dangle = v.z();
			_action_angle += dangle;
			float angle = _action_angle;
			if (constrain) {
				float angle_increment = osg::inDegrees(5.0);
				angle = floor(angle / angle_increment + 0.5) * angle_increment;
			}
			for (ManipulatedNode::List::iterator iter = _targets.begin(); iter != _targets.end(); ++iter) {
				iter->rotateBy(angle);
			}
			if (_view) _view->updatePosition(0, 0, 0, 0, 0, 0, angle);
		}

		_action_constrain = constrain;
		return true;
	}

	/** Pass the current mouse coordinates to the view.
	 */
	void updateCoordinates(const osgGA::GUIEventAdapter& ea) {
		if (_view) _view->updateMouseCoordinates(ea.getXnormalized(), ea.getYnormalized());
		if (_view) _view->updatePosition(ea.getXnormalized(), ea.getYnormalized(), 0, 0, 0, 0, 0);
	}

	/** Start to manipulate the selected layout nodes.  Each call to beginAction() must
	 *  be followed by either cancelAction() or applyAction().  It is an error to call
	 *  beginAction() while another action is progress.
	 */
	bool beginAction() {
		assert(!_action);
		assert(_active_group);

		_action_start_x_normalized = _ga_t0->getXnormalized();
		_action_start_y_normalized = _ga_t0->getYnormalized();
		if (_view) _view->computePixelCoords(_ga_t0->getXnormalized(), _ga_t0->getYnormalized(), _action_start_pixel_x, _action_start_pixel_y);
		_action_trigger = false;
		_action_constrain = false;
		_action_angle = 0.0;

		LayoutGroup::ChildList selected  = _active_group->getSelectedChildren();
		if (!selected.empty()) {
			_targets.clear();
			for (LayoutGroup::ChildList::iterator iter = selected.begin(); iter != selected.end(); ++iter) {
				_targets.push_back(ManipulatedNode(*iter));
			}
			_action = true;
		}

		return _action;
	}

	/** Cancel an action, restoring the selected layout nodes to their original positions
	 *  and orientations.
	 */
	void cancelAction() {
		if (!_action) return;
		for (ManipulatedNode::List::iterator iter = _targets.begin(); iter != _targets.end(); ++iter) {
			iter->cancel();
		}
		_targets.clear();
		_action = false;
	}

	/** Apply changes to the selected layout nodes.  Adds a ManipulateCommand to the graph
	 *  to allow these changes to later be undone.
	 */
	void applyAction() {
		assert(_action);
		if (_action_trigger) {
			std::string label = (_mode == MOVE ? "move node" : "rotate node");
			if (_targets.size() > 1) label += 's';
			if (_view) _view->graph()->runCommand(new ManipulateCommand(_targets, label));
		}
		_targets.clear();
		_action = false;
	}

	/** Delete all layout nodes in the current selection.
	 */
	void deleteSelectedNodes() {
		if (_view) _view->deleteSelectedNodes();
	}

	View *_view;
	LayoutGroup *_active_group;
	Mode _mode;

	bool _action;
	float _action_start_pixel_x;
	float _action_start_pixel_y;
	float _action_start_x_normalized;
	float _action_start_y_normalized;
	bool _action_trigger;
	float _action_angle;
	bool _action_constrain;
	bool _action_constrain_x;

	osg::Matrix _active_group_from_world;
	osg::Matrix _active_group_to_world;
	ManipulatedNode::List _targets;
	CallbackId _graph_event_adapter_id;

};

#endif  // __CSP_LAYOUT_VIEW_MANIPULATOR_H__
