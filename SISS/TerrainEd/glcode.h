// glcode.h
// Declarations for external OpenGL module.  These functions are
// defined in glcode.c and are called appropriately by the CView
// derived classes.
#if !defined(GLCODE_H)
#define GLCODE_H

int InitGL(GLvoid);
int DrawGLScene(GLvoid);
GLvoid ReSizeGLScene(GLsizei width, GLsizei height);
int LoadScene();
void SetCamera();
void KillGL();


#endif // Include/Define