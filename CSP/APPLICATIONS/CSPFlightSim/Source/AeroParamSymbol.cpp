#include "stdinc.h"

#ifdef _WIN32
#include <typeinfo.h>
#endif
#include <typeinfo>

#include "AeroParamSymbol.h"
#include "global.h"

AeroParamSymbol::AeroParamSymbol()
{

	m_pAeroParam = new AeroParam;

	m_pFuncPtrMap = new FuncPtrMap;

	(*m_pFuncPtrMap)["WINGSPAN"] = &AeroParamSymbol::SetWingSpanOp;
	(*m_pFuncPtrMap)["WINGCHORD"] = &AeroParamSymbol::SetWingChordOp;
	(*m_pFuncPtrMap)["WINGAREA"] = &AeroParamSymbol::SetWingAreaOp;
	(*m_pFuncPtrMap)["DEMAX"] = &AeroParamSymbol::SetDeMaxOp;
	(*m_pFuncPtrMap)["DEMIN"] = &AeroParamSymbol::SetDeMinOp;
	(*m_pFuncPtrMap)["DAMAX"] = &AeroParamSymbol::SetDaMaxOp;
	(*m_pFuncPtrMap)["DAMIN"] = &AeroParamSymbol::SetDaMinOp;
	(*m_pFuncPtrMap)["DRMAX"] = &AeroParamSymbol::SetDrMaxOp;
	(*m_pFuncPtrMap)["DRMIN"] = &AeroParamSymbol::SetDrMinOp;
    (*m_pFuncPtrMap)["GMIN"] = &AeroParamSymbol::SetGMinOp;
	(*m_pFuncPtrMap)["GMAX"] = &AeroParamSymbol::SetGMaxOp;
	(*m_pFuncPtrMap)["MASS"] = &AeroParamSymbol::SetMassOp;
	(*m_pFuncPtrMap)["I_XX"] = &AeroParamSymbol::SetIXXOp;
	(*m_pFuncPtrMap)["I_YY"] = &AeroParamSymbol::SetIYYOp;
	(*m_pFuncPtrMap)["I_ZZ"] = &AeroParamSymbol::SetIZZOp;
	(*m_pFuncPtrMap)["I_XZ"] = &AeroParamSymbol::SetIXZOp;
	(*m_pFuncPtrMap)["THRUSTMAX"] = &AeroParamSymbol::SetThrustMaxOp;
	(*m_pFuncPtrMap)["CD0"] = &AeroParamSymbol::SetCD0Op;
	(*m_pFuncPtrMap)["CD_A"] = &AeroParamSymbol::SetCD_aOp;
	(*m_pFuncPtrMap)["CD_DE"] = &AeroParamSymbol::SetCD_deOp;
	(*m_pFuncPtrMap)["STALLAOA"] = &AeroParamSymbol::SetStallAOAOp;
	(*m_pFuncPtrMap)["CL0"] = &AeroParamSymbol::SetCL0Op;
	(*m_pFuncPtrMap)["CL_A"] = &AeroParamSymbol::SetCL_aOp;
	(*m_pFuncPtrMap)["CL_ADOT"] = &AeroParamSymbol::SetCL_adotOp;
	(*m_pFuncPtrMap)["CL_Q"] = &AeroParamSymbol::SetCL_qOp;
	(*m_pFuncPtrMap)["CL_DE"] = &AeroParamSymbol::SetCL_deOp;
	(*m_pFuncPtrMap)["CM0"] = &AeroParamSymbol::SetCM0Op;
	(*m_pFuncPtrMap)["CM_A"] = &AeroParamSymbol::SetCM_aOp;
	(*m_pFuncPtrMap)["CM_ADOT"] = &AeroParamSymbol::SetCM_adotOp;
	(*m_pFuncPtrMap)["CM_Q"] = &AeroParamSymbol::SetCM_qOp;
	(*m_pFuncPtrMap)["CM_DE"] = &AeroParamSymbol::SetCM_deOp;
	(*m_pFuncPtrMap)["CY_BETA"] = &AeroParamSymbol::SetCY_betaOp;
	(*m_pFuncPtrMap)["CY_P"] = &AeroParamSymbol::SetCY_pOp;
	(*m_pFuncPtrMap)["CY_R"] = &AeroParamSymbol::SetCY_rOp;
	(*m_pFuncPtrMap)["CY_DA"] = &AeroParamSymbol::SetCY_daOp;
	(*m_pFuncPtrMap)["CY_DR"] = &AeroParamSymbol::SetCY_drOp;
	(*m_pFuncPtrMap)["CI_BETA"] = &AeroParamSymbol::SetCI_betaOp;
	(*m_pFuncPtrMap)["CI_P"] = &AeroParamSymbol::SetCI_pOp;
	(*m_pFuncPtrMap)["CI_R"] = &AeroParamSymbol::SetCI_rOp;
	(*m_pFuncPtrMap)["CI_DA"] = &AeroParamSymbol::SetCI_daOp;
	(*m_pFuncPtrMap)["CI_DR"] = &AeroParamSymbol::SetCI_drOp;
	(*m_pFuncPtrMap)["CN_BETA"] = &AeroParamSymbol::SetCn_betaOp;
	(*m_pFuncPtrMap)["CN_P"] = &AeroParamSymbol::SetCn_pOp;
	(*m_pFuncPtrMap)["CN_R"] = &AeroParamSymbol::SetCn_rOp;
	(*m_pFuncPtrMap)["CN_DA"] = &AeroParamSymbol::SetCn_daOp;
	(*m_pFuncPtrMap)["CN_DR"] = &AeroParamSymbol::SetCn_drOp;
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
	
       	ConvertStringToUpper(propString);
       	ConvertStringToUpper(valueString);

    double value = atof( valueString.c_str() );

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

void AeroParamSymbol::SetWingSpanOp(double value)
{
	m_pAeroParam->m_fWingSpan = value;
}

void AeroParamSymbol::SetWingChordOp(double value)
{
	m_pAeroParam->m_fWingChord = value;
}

void AeroParamSymbol::SetWingAreaOp(double value)
{
	m_pAeroParam->m_fWingArea = value;
}

void AeroParamSymbol::SetDeMaxOp(double value)
{
	m_pAeroParam->m_fDeMax = value;
}

void AeroParamSymbol::SetDeMinOp(double value)
{
	m_pAeroParam->m_fDeMin = value;
}

void AeroParamSymbol::SetDaMaxOp(double value)
{
	m_pAeroParam->m_fDaMax = value;
}

void AeroParamSymbol::SetDaMinOp(double value)
{
	m_pAeroParam->m_fDaMin = value;
}

void AeroParamSymbol::SetDrMaxOp(double value)
{
	m_pAeroParam->m_fDrMax = value;
}

void AeroParamSymbol::SetDrMinOp(double value)
{
	m_pAeroParam->m_fDrMin = value;
}

void AeroParamSymbol::SetGMinOp(double value)
{
	m_pAeroParam->m_fGMin = value;
}

void AeroParamSymbol::SetGMaxOp(double value)
{
	m_pAeroParam->m_fGMax = value;
}

void AeroParamSymbol::SetMassOp(double value)
{
	m_pAeroParam->m_fMass = value;
}

void AeroParamSymbol::SetIXXOp(double value)
{
	m_pAeroParam->m_fI_XX = value;
}

void AeroParamSymbol::SetIYYOp(double value)
{
	m_pAeroParam->m_fI_YY = value;
}

void AeroParamSymbol::SetIZZOp(double value)
{
	m_pAeroParam->m_fI_ZZ = value;
}

void AeroParamSymbol::SetIXZOp(double value)
{
	m_pAeroParam->m_fI_XZ = value;
}

void AeroParamSymbol::SetThrustMaxOp(double value)
{
	m_pAeroParam->m_fThrustMax = value;
}

void AeroParamSymbol::SetCD0Op(double value)
{
	m_pAeroParam->m_fCD0 = value;
}

void AeroParamSymbol::SetCD_aOp(double value)
{
	m_pAeroParam->m_fCD_a = value;
}

void AeroParamSymbol::SetCD_deOp(double value)
{
	m_pAeroParam->m_fCD_de = value;
}

void AeroParamSymbol::SetCL0Op(double value)
{
	m_pAeroParam->m_fCL0 = value;
}

void AeroParamSymbol::SetCL_aOp(double value)
{
	m_pAeroParam->m_fCL_a = value;
}

void AeroParamSymbol::SetCL_adotOp(double value)
{
	m_pAeroParam->m_fCL_adot = value;
}

void AeroParamSymbol::SetCL_qOp(double value)
{
	m_pAeroParam->m_fCL_q = value;
}

void AeroParamSymbol::SetCL_deOp(double value)
{
	m_pAeroParam->m_fCL_de = value;
}

void AeroParamSymbol::SetStallAOAOp(double value)
{
	m_pAeroParam->m_fstallAOA = value;
}

void AeroParamSymbol::SetCM0Op(double value)
{
	m_pAeroParam->m_fCM0 = value;
}

void AeroParamSymbol::SetCM_aOp(double value)
{
	m_pAeroParam->m_fCM_a = value;
}

void AeroParamSymbol::SetCM_adotOp(double value)
{
	m_pAeroParam->m_fCM_adot = value;
}

void AeroParamSymbol::SetCM_qOp(double value)
{
	m_pAeroParam->m_fCM_q = value;
}

void AeroParamSymbol::SetCM_deOp(double value)
{
	m_pAeroParam->m_fCM_de = value;
}

void AeroParamSymbol::SetCY_betaOp(double value)
{
	m_pAeroParam->m_fCY_beta = value;
}

void AeroParamSymbol::SetCY_pOp(double value)
{
	m_pAeroParam->m_fCY_p = value;
}

void AeroParamSymbol::SetCY_rOp(double value)
{
	m_pAeroParam->m_fCY_r = value;
}

void AeroParamSymbol::SetCY_daOp(double value)
{
	m_pAeroParam->m_fCY_da = value;
}

void AeroParamSymbol::SetCY_drOp(double value)
{
	m_pAeroParam->m_fCY_dr = value;
}

void AeroParamSymbol::SetCI_betaOp(double value)
{
	m_pAeroParam->m_fCI_beta = value;
}

void AeroParamSymbol::SetCI_pOp(double value)
{
	m_pAeroParam->m_fCI_p = value;
}

void AeroParamSymbol::SetCI_rOp(double value)
{
	m_pAeroParam->m_fCI_r = value;
}

void AeroParamSymbol::SetCI_daOp(double value)
{
	m_pAeroParam->m_fCI_da = value;
}

void AeroParamSymbol::SetCI_drOp(double value)
{
	m_pAeroParam->m_fCI_dr = value;
}

void AeroParamSymbol::SetCn_betaOp(double value)
{
	m_pAeroParam->m_fCn_beta = value;
}

void AeroParamSymbol::SetCn_pOp(double value)
{
	m_pAeroParam->m_fCn_p = value;
}

void AeroParamSymbol::SetCn_rOp(double value)
{
	m_pAeroParam->m_fCn_r = value;
}

void AeroParamSymbol::SetCn_daOp(double value)
{
	m_pAeroParam->m_fCn_da = value;
}

void AeroParamSymbol::SetCn_drOp(double value)
{
	m_pAeroParam->m_fCn_dr = value;
}


std::ostream & operator << (std::ostream & os, const AeroParamSymbol& sym)
{
	os << "SymbolClass: " << typeid(sym).name();
	return os;

}
