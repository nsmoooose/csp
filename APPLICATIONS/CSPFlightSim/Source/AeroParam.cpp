#include "stdinc.h"
#include "AeroParam.h"

AeroParam::AeroParam()
{
	m_fWingSpan = 0.0f;
	m_fWingChord = 0.0f;
	m_fWingArea = 0.0f;

	m_fDeMax = 0.0f;
	m_fDeMin = 0.0f;
	m_fDaMax = 0.0f;
	m_fDaMin = 0.0f;
	m_fDrMax = 0.0f;
	m_fDrMin = 0.0f;

	m_fGMin = -9.0;
	m_fGMax = 9.0;

	m_fMass = 0.0f;
	m_fI_XX = 0.0f;
	m_fI_YY = 0.0f;
	m_fI_ZZ = 0.0f;
	m_fI_XZ = 0.0f;

	m_fThrustMax = 0.0f;

	m_fCD0 = 0.0f;
	m_fCD_a = 0.0f;
	m_fCD_de = 0.0f;

	m_fstallAOA = 90.0;
	m_fCL0 = 0.0f;
	m_fCL_a = 0.0f;
	m_fCL_adot = 0.0f;
	m_fCL_q = 0.0f;
	m_fCL_de = 0.0f;

	m_fCM0 = 0.0f;         
	m_fCM_a = 0.0f;        
	m_fCM_adot = 0.0f;
	m_fCM_q = 0.0f;         
	m_fCM_de = 0.0f;        

	m_fCY_beta = 0.0f;      
	m_fCY_p = 0.0f;         
	m_fCY_r = 0.0f;         
	m_fCY_da = 0.0f;        
	m_fCY_dr = 0.0f;        

	m_fCI_beta = 0.0f;       
	m_fCI_p = 0.0f;          
	m_fCI_r = 0.0f;          
	m_fCI_da = 0.0f;         
	m_fCI_dr = 0.0f;         

	m_fCn_beta = 0.0f;       
	m_fCn_p = 0.0f;          
	m_fCn_r = 0.0f;          
	m_fCn_da = 0.0f;         
	m_fCn_dr = 0.0f;
}