#ifndef _STANDARDMATH_INL_
#define _STANDARDMATH_INL_

inline StandardRay MouseToRay(StandardWindow3D *p_Window3D, StandardCamera *p_Camera)
{

  StandardRay Ray;

  _Point pt;

  pt = p_Window3D->GetMouse();

  StandardMatrix4 tempProj;

  tempProj = *p_Camera->GetProjectionMatrix();

  StandardVector3 v;

  v.x =  ((pt.x * 2) - 1) / tempProj.rowcol[0][0];
  v.y = -((pt.y * 2) - 1) / tempProj.rowcol[1][1];
  v.z =  1.0f;

  StandardVector3 vPickRayDir, vPickRayOrig;

  StandardMatrix4 tempCamera;

  tempCamera = *p_Camera->GetCameraMatrix();
  tempCamera.Inverse();

  Ray.Direction.x  = v.x*tempCamera.rowcol[0][0] + v.y*tempCamera.rowcol[1][0] + v.z*tempCamera.rowcol[2][0];
  Ray.Direction.y  = v.x*tempCamera.rowcol[0][1] + v.y*tempCamera.rowcol[1][1] + v.z*tempCamera.rowcol[2][1];
  Ray.Direction.z  = v.x*tempCamera.rowcol[0][2] + v.y*tempCamera.rowcol[1][2] + v.z*tempCamera.rowcol[2][2];
  Ray.Position.x = tempCamera.rowcol[3][0];
  Ray.Position.y = tempCamera.rowcol[3][1];
  Ray.Position.z = tempCamera.rowcol[3][2];

  Ray.Direction.Normalize();

  return Ray;

}


inline bool RayIntersectTriangle(StandardVector3 orig,
                                 StandardVector3 dir,
                                 StandardVector3 v0,
                                 StandardVector3 v1,
                                 StandardVector3 v2,
                                 double *length, double *u, double *v)
{

    // Find vectors for two edges sharing vert0
    StandardVector3 edge1 = v1 - v0;
    StandardVector3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    StandardVector3 pvec;
    pvec = Cross(dir, edge2);

    // If determinant is near zero, ray lies in plane of triangle
    double det = Dot( edge1, pvec);
    if( det < 0.0001f )
        return false;

    // Calculate distance from vert0 to ray origin
    StandardVector3 tvec = orig - v0;

    // Calculate U parameter and test bounds
    *u = Dot( tvec, pvec);
    if( *u < 0.0f || *u > det )   
      return false;

    // Prepare to test V parameter
    StandardVector3 qvec;
    qvec = Cross(tvec, edge1);

    // Calculate V parameter and test bounds
    *v = Dot( dir, qvec);
    if( *v < 0.0f || *u + *v > det )
        return false;


    // Calculate t, scale parameters, ray intersects triangle
    *length = Dot( edge2, qvec );
    double fInvDet = 1.0f / det;
    *length *= fInvDet;
    if(*length < 0)
    {
      return false;
    }
    *u *= fInvDet;
    *v *= fInvDet;


  return true;
}

#endif
