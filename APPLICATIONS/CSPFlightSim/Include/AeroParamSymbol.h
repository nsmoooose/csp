#ifndef __AEROPARAMSYMBOL_H__
#define __AEROPARAMSYMBOL_H__

#include "BaseSymbol.h"
#include "AeroParam.h"

class FloatSetOp
{

public:

	FloatSetOp(float * _v) { m_pValue = _v; }
	void operator() (float value)  { *m_pValue = value; }
	float * m_pValue;

};

class AeroParamSymbol : public BaseSymbol
{
public:

	typedef void(AeroParamSymbol::*SetFuncPtr)(float);
	typedef std::map< std::string, SetFuncPtr > FuncPtrMap;

	AeroParamSymbol();
	~AeroParamSymbol();
	virtual std::string SetCommand(std::deque<std::string> & args);
	virtual std::string GetCommand(std::deque<std::string> & args);
	
	friend std::ostream & operator << (std::ostream & os, const AeroParamSymbol& sym);

	void SetWingSpanOp(float value);
	void SetWingChordOp(float value);
	void SetWingAreaOp(float value);
	void SetDeMaxOp(float value);
	void SetDeMinOp(float value);
	void SetDaMaxOp(float value);
	void SetDaMinOp(float value);
	void SetDrMaxOp(float value);
	void SetDrMinOp(float value);
	void SetMassOp(float value);
	void SetIXXOp(float value);
	void SetIYYOp(float value);
	void SetIZZOp(float value);
	void SetIXZOp(float value);
	void SetThrustMaxOp(float value);
	void SetCD0Op(float value);
	void SetCD_aOp(float value);
	void SetCD_deOp(float value);
	void SetCL0Op(float value);
	void SetCL_aOp(float value);
	void SetCL_adotOp(float value);
	void SetCL_qOp(float value);
	void SetCL_deOp(float value);
	void SetCM0Op(float value);
	void SetCM_aOp(float value);
	void SetCM_adotOp(float value);
	void SetCM_qOp(float value);
	void SetCM_deOp(float value);
	void SetCY_betaOp(float value);
	void SetCY_pOp(float value);
	void SetCY_rOp(float value);
	void SetCY_daOp(float value);
	void SetCY_drOp(float value);
	void SetCI_betaOp(float value);
	void SetCI_pOp(float value);
	void SetCI_rOp(float value);
	void SetCI_daOp(float value);
	void SetCI_drOp(float value);
	void SetCn_betaOp(float value);
	void SetCn_pOp(float value);
	void SetCn_rOp(float value);
	void SetCn_daOp(float value);
	void SetCn_drOp(float value);

	AeroParam * getAeroParam() { return m_pAeroParam; }

protected:


	std::map< std::string, FloatSetOp> propertyMap;
	FuncPtrMap * m_pFuncPtrMap;

	AeroParam * m_pAeroParam;

public:


};

#endif
