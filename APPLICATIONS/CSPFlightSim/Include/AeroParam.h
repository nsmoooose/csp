#ifndef __AEROPARAM_H__
#define __AEROPARAM_H__

struct AeroParam
{

	AeroParam();


	float m_fWingSpan;
	float m_fWingChord;
	float m_fWingArea;

	float m_fDeMax;
	float m_fDeMin;
	float m_fDaMax;
	float m_fDaMin;
	float m_fDrMax;
	float m_fDrMin;

	float m_fMass;
	float m_fI_XX;
	float m_fI_YY;
	float m_fI_ZZ;
	float m_fI_XZ;

	float m_fThrustMax;

	float m_fCD0;
	float m_fCD_a;
	float m_fCD_de;

	float m_fCL0;
	float m_fCL_a;
	float m_fCL_adot;
	float m_fCL_q;
	float m_fCL_de;

	float m_fCM0;         
	float m_fCM_a;        
	float m_fCM_adot;
	float m_fCM_q;         
	float m_fCM_de;        

	float m_fCY_beta;      
	float m_fCY_p;         
	float m_fCY_r;         
	float m_fCY_da;        
	float m_fCY_dr;        

	float m_fCI_beta;       
	float m_fCI_p;          
	float m_fCI_r;          
	float m_fCI_da;         
	float m_fCI_dr;         

	float m_fCn_beta;       
	float m_fCn_p;          
	float m_fCn_r;          
	float m_fCn_da;         
	float m_fCn_dr;

};

#endif