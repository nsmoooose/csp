#ifdef D3D

#include "D3D.h"
#include "D3DCamera.h"


D3DCamera::D3DCamera(StandardGraphics * pDisplay) : Camera(pDisplay)
{
	// do the cast here one time to the specific D3DDisplay;
	m_pD3Display = (D3DGraphics *)pDisplay;

}

D3DCamera::~D3DCamera()
{

}

void D3DCamera::SetPosition(float loc_x, float loc_y, float loc_z,
		                     float target_x, float target_y, float target_z,
							 float up_x, float up_y, float up_z)
{

}

void D3DCamera::SetPosition(vector3 & loc, vector3 & target, vector3 & up)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;

	D3DXMatrixLookAtRH(&matWorld, (D3DXVECTOR3*)&loc, 
		(D3DXVECTOR3*)&target, (D3DXVECTOR3*)&up);

	D3DXMatrixIdentity(&matView);

	m_pD3Display->Device->SetTransform(D3DTS_WORLD, &matWorld);
	m_pD3Display->Device->SetTransform(D3DTS_VIEW, &matView);
}

void D3DCamera::SetPosition(_Vector * loc, _Vector * target, _Vector * up)
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;

	
	D3DXMatrixIdentity(&matView);


	D3DXMatrixLookAtRH(&matWorld, (D3DXVECTOR3*)loc, 
		(D3DXVECTOR3*)target, (D3DXVECTOR3*)up);
	D3DXMatrixIdentity(&matView);

	m_pD3Display->Device->SetTransform(D3DTS_WORLD, &matWorld);
	m_pD3Display->Device->SetTransform(D3DTS_VIEW, &matView);

}

void D3DCamera::SetPosition(float roll, float pitch, float heading,
		                     float loc_x, float loc_y, float loc_z)
{
	D3DXMATRIX mat;
    D3DXMatrixRotationYawPitchRoll(&mat, heading, pitch, roll);
	m_pD3Display->Device->SetTransform(D3DTS_VIEW, &mat);
}

void D3DCamera::Translate(float X, float Y, float Z)
{
	D3DXMATRIX mat;
	m_pD3Display->Device->GetTransform(D3DTS_WORLD, &mat);
	D3DXMatrixTranslation(&mat, -X, -Y, -Z);
	// need some conversion for right-left coord sys.
	mat._11 *= -1;
	mat._33 *= -1;
	m_pD3Display->Device->SetTransform(D3DTS_WORLD, &mat);
}

void D3DCamera::Rotate(float angle, float x, float y, float z)
{
	D3DXMATRIX CurrentMat;
	D3DXMATRIX RotMat;
	D3DXVECTOR3 axis(x,y,z);
	m_pD3Display->Device->GetTransform(D3DTS_WORLD, &CurrentMat);
	D3DXMatrixRotationAxis(&RotMat, &axis, angle);
	D3DXMatrixMultiply(&CurrentMat, &RotMat, &CurrentMat);
	m_pD3Display->Device->SetTransform(D3DTS_WORLD, &CurrentMat);
}

	
void D3DCamera::SetMatrix(_Matrix *  mat)
{
	
}

void D3DCamera::MultiMatrix(_Matrix *  mat)
{

}

void D3DCamera::SetPerspectiveView(float angle, float nearplane, float farplane)
{
	D3DXMATRIX mat;
	_Rect rect = m_pD3Display->GetCoordsRect();
	float aspect = (rect.x2-rect.x1)/(rect.y2-rect.y1);

	D3DXMatrixPerspectiveFovRH(&mat, angle, aspect,
		nearplane,farplane);
	m_pD3Display->Device->SetTransform(D3DTS_PROJECTION, &mat);
}

void D3DCamera::SetOrthoView(float nearplane, float farplane)
{
	D3DXMATRIX mat;
	_Rect rect = m_pD3Display->GetCoordsRect();
	D3DXMatrixOrthoRH(&mat, rect.x2-rect.x1, rect.y1-rect.y2, nearplane, farplane);
	m_pD3Display->Device->SetTransform(D3DTS_PROJECTION, &mat);

}


void D3DCamera::Reset()
{
	D3DXMATRIX matWorld;
	D3DXMATRIX matView;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixIdentity(&matView);
	m_pD3Display->Device->SetTransform(D3DTS_WORLD, &matWorld);
	m_pD3Display->Device->SetTransform(D3DTS_VIEW, &matView);

}

#endif



