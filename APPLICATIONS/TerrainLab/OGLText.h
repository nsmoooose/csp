///////////////////////////////////////////////////////////////////////////
//
//   TerrainEngine  - CSP - http://csp.homeip.net
//
//   coded by zzed        and Stormbringer
//   email:   zzed@gmx.li     storm_bringer@gmx.li 
//
///////////////////////////////////////////////////////////////////////////
//
//   <file-description>
//
///////////////////////////////////////////////////////////////////////////
//
//  file history:
//
//
//
//
//
///////////////////////////////////////////////////////////////////////////

#if !defined(OGLTEXT_H)
#define OGLTEXT_H


class COGLText
{
public:
	COGLText();
	~COGLText();
	void Print(const char *fmt, ...);
	void Print(float fX, float fY, const char *fmt, ...);
	void Init();
private:
	unsigned int	m_uiCharLists;
	unsigned int	*m_puiTextureList;
	void RenderText(char *cText);
	char* LoadCharacters(char *cFilename);
	void CreateDisplayLists(char *pCharArray);
};

extern COGLText OGLText;

#endif