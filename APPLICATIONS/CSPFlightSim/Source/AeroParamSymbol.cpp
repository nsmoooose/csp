#include "stdinc.h"

#include "AeroParamSymbol.h"

AeroParamSymbol::AeroParamSymbol()
{

	m_pAeroParam = new AeroParam;

	m_pFuncPtrMap = new FuncPtrMap;

	(*m_pFuncPtrMap)["WINGSPAN"] = SetWingSpanOp;
	(*m_pFuncPtrMap)["WINGCHORD"] = SetWingChordOp;
	(*m_pFuncPtrMap)["WINGAREA"] = SetWingAreaOp;
	(*m_pFuncPtrMap)["DEMAX"] = SetDeMaxOp;
	(*m_pFuncPtrMap)["DEMIN"] = SetDeMinOp;
	(*m_pFuncPtrMap)["DAMAX"] = SetDaMaxOp;
	(*m_pFuncPtrMap)["DAMIN"] = SetDaMinOp;
	(*m_pFuncPtrMap)["DRMAX"] = SetDrMaxOp;
	(*m_pFuncPtrMap)["DRMIN"] = SetDrMinOp;
	(*m_pFuncPtrMap)["MASS"] = SetMassOp;
	(*m_pFuncPtrMap)["I_XX"] = SetIXXOp;
	(*m_pFuncPtrMap)["I_YY"] = SetIYYOp;
	(*m_pFuncPtrMap)["I_ZZ"] = SetIZZOp;
	(*m_pFuncPtrMap)["I_XZ"] = SetIXZOp;
	(*m_pFuncPtrMap)["THRUSTMAX"] = SetThrustMaxOp;
	(*m_pFuncPtrMap)["CD0"] = SetCD0Op;
	(*m_pFuncPtrMap)["CD_A"] = SetCD_aOp;
	(*m_pFuncPtrMap)["CD_DE"] = SetCD_deOp;
	(*m_pFuncPtrMap)["CL0"] = SetCL0Op;
	(*m_pFuncPtrMap)["CL_A"] = SetCL_aOp;
	(*m_pFuncPtrMap)["CL_ADOT"] = SetCL_adotOp;
	(*m_pFuncPtrMap)["CL_Q"] = SetCL_qOp;
	(*m_pFuncPtrMap)["CL_DE"] = SetCL_deOp;
	(*m_pFuncPtrMap)["CM0"] = SetCM0Op;
	(*m_pFuncPtrMap)["CM_A"] = SetCM_aOp;
	(*m_pFuncPtrMap)["CM_ADOT"] = SetCM_adotOp;
	(*m_pFuncPtrMap)["CM_Q"] = SetCM_qOp;
	(*m_pFuncPtrMap)["CM_DE"] = SetCM_deOp;
	(*m_pFuncPtrMap)["CY_BETA"] = SetCY_betaOp;
	(*m_pFuncPtrMap)["CY_P"] = SetCY_pOp;
	(*m_pFuncPtrMap)["CY_R"] = SetCY_rOp;
	(*m_pFuncPtrMap)["CY_DA"] = SetCY_daOp;
	(*m_pFuncPtrMap)["CY_DR"] = SetCY_drOp;
	(*m_pFuncPtrMap)["CI_BETA"] = SetCI_betaOp;
	(*m_pFuncPtrMap)["CI_P"] = SetCI_pOp;
	(*m_pFuncPtrMap)["CI_R"] = SetCI_rOp;
	(*m_pFuncPtrMap)["CI_DA"] = SetCI_daOp;
	(*m_pFuncPtrMap)["CI_DR"] = SetCI_drOp;
	(*m_pFuncPtrMap)["CN_BETA"] = SetCn_betaOp;
	(*m_pFuncPtrMap)["CN_P"] = SetCn_pOp;
	(*m_pFuncPtrMap)["CN_R"] = SetCn_rOp;
	(*m_pFuncPtrMap)["CN_DA"] = SetCn_daOp;
	(*m_pFuncPtrMap)["CN_DR"] = SetCn_drOp;
}

AeroParamSymbol::~AeroParamSymbol()
{
	delete m_pAeroParam;;
	delete m_pFuncPtrMap;
}

std::string AeroParamSymbol::SetCommand(std::deque<std::string> & args)
{

	std::string propString = args[0];
	std::string valueString = args[1];
    float value = atof( valueString.c_str() );

	FuncPtrMap::iterator pos;
	pos = m_pFuncPtrMap->find(propString);
	if (pos == m_pFuncPtrMap->end() )
		return "Unable to process " + propString;

	SetFuncPtr op = pos->second;
	(this->*op)(value);

	return "OK";
}

std::string AeroParamSymbol::GetCommand(std::deque<std::string> & args)
{
	return "Not Yet Implemented:";
}

void AeroParamSymbol::SetWingSpanOp(float value)
{
	m_pAeroParam->m_fWingSpan = value;
}

void AeroParamSymbol::SetWingChordOp(float value)
{
	m_pAeroParam->m_fWingChord = value;
}

void AeroParamSymbol::SetWingAreaOp(float value)
{
	m_pAeroParam->m_fWingArea = value;
}

void AeroParamSymbol::SetDeMaxOp(float value)
{
	m_pAeroParam->m_fDeMax = value;
}

void AeroParamSymbol::SetDeMinOp(float value)
{
	m_pAeroParam->m_fDeMin = value;
}

void AeroParamSymbol::SetDaMaxOp(float value)
{
	m_pAeroParam->m_fDaMax = value;
}

void AeroParamSymbol::SetDaMinOp(float value)
{
	m_pAeroParam->m_fDaMin = value;
}

void AeroParamSymbol::SetDrMaxOp(float value)
{
	m_pAeroParam->m_fDrMax = value;
}

void AeroParamSymbol::SetDrMinOp(float value)
{
	m_pAeroParam->m_fDrMin = value;
}

void AeroParamSymbol::SetMassOp(float value)
{
	m_pAeroParam->m_fMass = value;
}

void AeroParamSymbol::SetIXXOp(float value)
{
	m_pAeroParam->m_fI_XX = value;
}

void AeroParamSymbol::SetIYYOp(float value)
{
	m_pAeroParam->m_fI_YY = value;
}

void AeroParamSymbol::SetIZZOp(float value)
{
	m_pAeroParam->m_fI_ZZ = value;
}

void AeroParamSymbol::SetIXZOp(float value)
{
	m_pAeroParam->m_fI_XZ = value;
}

void AeroParamSymbol::SetThrustMaxOp(float value)
{
	m_pAeroParam->m_fThrustMax = value;
}

void AeroParamSymbol::SetCD0Op(float value)
{
	m_pAeroParam->m_fCD0 = value;
}

void AeroParamSymbol::SetCD_aOp(float value)
{
	m_pAeroParam->m_fCD_a = value;
}

void AeroParamSymbol::SetCD_deOp(float value)
{
	m_pAeroParam->m_fCD_de = value;
}

void AeroParamSymbol::SetCL0Op(float value)
{
	m_pAeroParam->m_fCL0 = value;
}

void AeroParamSymbol::SetCL_aOp(float value)
{
	m_pAeroParam->m_fCL_a = value;
}

void AeroParamSymbol::SetCL_adotOp(float value)
{
	m_pAeroParam->m_fCL_adot = value;
}

void AeroParamSymbol::SetCL_qOp(float value)
{
	m_pAeroParam->m_fCL_q = value;
}

void AeroParamSymbol::SetCL_deOp(float value)
{
	m_pAeroParam->m_fCL_de = value;
}

void AeroParamSymbol::SetCM0Op(float value)
{
	m_pAeroParam->m_fCM0 = value;
}

void AeroParamSymbol::SetCM_aOp(float value)
{
	m_pAeroParam->m_fCM_a = value;
}

void AeroParamSymbol::SetCM_adotOp(float value)
{
	m_pAeroParam->m_fCM_adot = value;
}

void AeroParamSymbol::SetCM_qOp(float value)
{
	m_pAeroParam->m_fCM_q = value;
}

void AeroParamSymbol::SetCM_deOp(float value)
{
	m_pAeroParam->m_fCM_de = value;
}

void AeroParamSymbol::SetCY_betaOp(float value)
{
	m_pAeroParam->m_fCY_beta = value;
}

void AeroParamSymbol::SetCY_pOp(float value)
{
	m_pAeroParam->m_fCY_p = value;
}

void AeroParamSymbol::SetCY_rOp(float value)
{
	m_pAeroParam->m_fCY_r = value;
}

void AeroParamSymbol::SetCY_daOp(float value)
{
	m_pAeroParam->m_fCY_da = value;
}

void AeroParamSymbol::SetCY_drOp(float value)
{
	m_pAeroParam->m_fCY_dr = value;
}

void AeroParamSymbol::SetCI_betaOp(float value)
{
	m_pAeroParam->m_fCI_beta = value;
}

void AeroParamSymbol::SetCI_pOp(float value)
{
	m_pAeroParam->m_fCI_p = value;
}

void AeroParamSymbol::SetCI_rOp(float value)
{
	m_pAeroParam->m_fCI_r = value;
}

void AeroParamSymbol::SetCI_daOp(float value)
{
	m_pAeroParam->m_fCI_da = value;
}

void AeroParamSymbol::SetCI_drOp(float value)
{
	m_pAeroParam->m_fCI_dr = value;
}

void AeroParamSymbol::SetCn_betaOp(float value)
{
	m_pAeroParam->m_fCn_beta = value;
}

void AeroParamSymbol::SetCn_pOp(float value)
{
	m_pAeroParam->m_fCn_p = value;
}

void AeroParamSymbol::SetCn_rOp(float value)
{
	m_pAeroParam->m_fCn_r = value;
}

void AeroParamSymbol::SetCn_daOp(float value)
{
	m_pAeroParam->m_fCn_da = value;
}

void AeroParamSymbol::SetCn_drOp(float value)
{
	m_pAeroParam->m_fCn_dr = value;
}


std::ostream & operator << (std::ostream & os, const AeroParamSymbol& sym)
{
	os << "SymbolClass: " << typeid(sym).name();
	return os;

}
