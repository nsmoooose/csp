// Combat Simulator Project
// Copyright (C) 2005 The Combat Simulator Project
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
 * @file Projectile.h
 *
 **/

#ifndef __CSPSIM_STORES_PROJECTILE_H__
#define __CSPSIM_STORES_PROJECTILE_H__

#include <csp/cspsim/DynamicObject.h>

CSP_NAMESPACE

class Store;

/** A dynamic object subclass representing launched or dropped objects, including
 *  for example dropped fuel tanks, smart and dumb bombs, and missiles.  Despite
 *  the name, the Projectile class is intended for use with both passively and actively
 *  guided munitions.
 */
class Projectile: public DynamicObject {
public:
	CSP_DECLARE_OBJECT(Projectile)

	Projectile();

	/** Called by StoresManagementSystem when releasing a store from a parent object.
	 *  The newly created Projectile instance is responsible for loading necessary
	 *  configuration data from the original Store object and hiding the original
	 *  (attached) 3D model of the store when the new SceneModel is shown.  The
	 *  projectile instance should only retain a reference to the parent if it is
	 *  absolutely necessary; if so, be careful to avoid circular references.
	 */
	virtual void prepareRelease(Ref<DynamicObject> const &parent, Ref<Store> const &store);

protected:
	virtual ~Projectile();

	virtual void onEnterScene();
	virtual void createSceneModel();

private:
	Ref<Store> m_Store;
	bool m_DetachedModel;
};

CSP_NAMESPACE_END

#endif // __CSPSIM_STORES_PROJECTILE_H__

