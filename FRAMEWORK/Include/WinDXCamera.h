#ifdef D3D
#ifndef WINDXCAMERATAG

  #define WINDXCAMERATAG

  #include "Framework.h"

  #include <d3d8.h>
  #include <d3dx8.h>

  class WinDXCamera : public StandardCamera
  {

  private:

      StandardGraphics *p_Graphics;
      IDirect3DDevice8 *p_Device;

      StandardMatrix4 p_ProjectionMatrix;
      StandardMatrix4 p_CameraMatrix;

      StandardMatrix4 p_CameraInverseMatrix;
      bool            p_Dirty;

  public:

      WinDXCamera(StandardGraphics *Graphics);
      ~WinDXCamera();

        // camera matrix methods.
	    void SetCameraIdentity();

	    // Absolute position methods.
	    void SetPosition(float loc_x, float loc_y, float loc_z,
		                         float target_x, float target_y, float target_z,
							     float up_x, float up_y, float up_z);

	    void SetPosition(StandardVector3 * pLoc,
							    StandardVector3 * pTarget,
							    StandardVector3 * pUp);

	    void SetPosition(float roll, float pitch, float heading,
		                         float loc_x, float loc_y, float loc_z);
	    
	    void SetPosition(float loc_x, float loc_y, float loc_z);
	    // relative position methods.

	    void Rotate(char axis, float amount);
      void Move(char axis, float amount);

	    // projection matrix methods.
	    void SetFOVPerspectiveView(float angle, unsigned short width, unsigned short height, float NearZ, float FarZ);
	    void SetPerspectiveView(float angle, unsigned short width, unsigned short height, float NearZ, float FarZ);
	    void SetOrthoView(unsigned short width, unsigned short height,float NearZ, float FarZ);

      void Apply();

      // query methods
      StandardMatrix4* GetProjectionMatrix();
      StandardMatrix4* GetCameraMatrix();
      StandardMatrix4* GetCameraInverseMatrix();

  };


#endif

#endif