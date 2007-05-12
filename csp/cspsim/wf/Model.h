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
 * @file Model.h
 *
 **/

#ifndef __CSPSIM_WF_MODEL_H__
#define __CSPSIM_WF_MODEL_H__

#include <csp/cspsim/wf/Control.h>

CSP_NAMESPACE

namespace wf {

/** A class that represents a 3d model.
 *
 */
class CSPSIM_EXPORT Model : public Control {
public:
	Model();
	virtual ~Model();

	virtual std::string getName() const;
	
	virtual std::string getModelFilePath() const;
	virtual void setModelFilePath(std::string& filePath);

	virtual double getScale() const;
	virtual void setScale(double scale);

	virtual void buildGeometry();
		
	template<class Archive>
	void serialize(Archive & ar) {
		Control::serialize(ar);
		ar & make_nvp("@ModelFilePath", m_FilePath);
		ar & make_nvp("@Scale", m_Scale);
	}
	
private:

protected:
	std::string m_FilePath;	
	double m_Scale;
};

} // namespace wf

CSP_NAMESPACE_END

#endif // __CSPSIM_WF_MODEL_H__

