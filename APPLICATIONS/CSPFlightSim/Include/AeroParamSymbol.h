// Combat Simulator Project - FlightSim Demo
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
 * @file AeroParamSymbol.h
 *
 **/


#ifndef __AEROPARAMSYMBOL_H__
#define __AEROPARAMSYMBOL_H__

#include "BaseSymbol.h"
#include "AeroParam.h"


/**
 * class FloatSetOp - Describe me!
 *
 * @author unknown
 */
class FloatSetOp
{

public:

	FloatSetOp(double * _v) { m_pValue = _v; }
	void operator() (double value)  { *m_pValue = value; }
	double * m_pValue;

};


/**
 * class AeroParamSymbol - Describe me!
 *
 * @author unknown
 */
class AeroParamSymbol : public BaseSymbol
{
public:

	typedef void(AeroParamSymbol::*SetFuncPtr)(double);
	typedef std::map< std::string, SetFuncPtr > FuncPtrMap;

	AeroParamSymbol();
	~AeroParamSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args);
	virtual std::string GetCommand(std::deque<std::string> & args);
	
	friend std::ostream & operator << (std::ostream & os, const AeroParamSymbol& sym);

	void SetWingSpanOp(double value);
	void SetWingChordOp(double value);
	void SetWingAreaOp(double value);
	void SetDeMaxOp(double value);
	void SetDeMinOp(double value);
	void SetDaMaxOp(double value);
	void SetDaMinOp(double value);
	void SetDrMaxOp(double value);
	void SetDrMinOp(double value);
	void SetGMinOp(double value);
	void SetGMaxOp(double value);
	void SetMassOp(double value);
	void SetStallAOAOp(double value);
	void SetIXXOp(double value);
	void SetIYYOp(double value);
	void SetIZZOp(double value);
	void SetIXZOp(double value);
	void SetThrustMaxOp(double value);
	void SetCD0Op(double value);
	void SetCD_aOp(double value);
	void SetCD_deOp(double value);
	void SetCL0Op(double value);
	void SetCL_aOp(double value);
	void SetCL_adotOp(double value);
	void SetCL_qOp(double value);
	void SetCL_deOp(double value);
	void SetCM0Op(double value);
	void SetCM_aOp(double value);
	void SetCM_adotOp(double value);
	void SetCM_qOp(double value);
	void SetCM_deOp(double value);
	void SetCY_betaOp(double value);
	void SetCY_pOp(double value);
	void SetCY_rOp(double value);
	void SetCY_daOp(double value);
	void SetCY_drOp(double value);
	void SetCI_betaOp(double value);
	void SetCI_pOp(double value);
	void SetCI_rOp(double value);
	void SetCI_daOp(double value);
	void SetCI_drOp(double value);
	void SetCn_betaOp(double value);
	void SetCn_pOp(double value);
	void SetCn_rOp(double value);
	void SetCn_daOp(double value);
	void SetCn_drOp(double value);

	AeroParam * getAeroParam() { return m_pAeroParam; }

protected:


	//std::map< std::string, FloatSetOp> propertyMap;
	FuncPtrMap * m_pFuncPtrMap;

	AeroParam * m_pAeroParam;

public:


};

#endif // __AEROPARAMSYMBOL_H__

