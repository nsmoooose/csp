// TerrainManager.cpp: implementation of the TerrainManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TerrainManager.h"
#include <RenderKernel.h>
#include <ParticleStructure.h>
#include <Material.h>
#include <Modifier.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TerrainManager::TerrainManager()
{

}

bool TerrainManager::Init(RenderKernel *renderKernel)
{
	this->thisRenderKernel = renderKernel;
	SetHost(thisRenderKernel);
	
	material = NewMem(Material);
	material->CreateTextures();
	material->diffuseMap->filename = PString("data/3d/textures/test_texture.tga");
	Material::Register(thisRenderKernel, material);

	//analog zu OpenGL benutzen wir anstatt glVertex3f() Vector3(), uebergeben aber genauso die 3d koordinaten
	vertexArray[0] = Vector3(0.0f, -5.0f, 2.0f);
	//Texturkoordinaten sind auch nicht schwer.
	vertexArray[0].u = 0.5f;
	vertexArray[0].v = 0.0f;


	vertexArray[1] = Vector3(-2.5f, -5.0f, 0.0f);
	vertexArray[1].u = 1.0f;
	vertexArray[1].v = 0.0f;

	vertexArray[2] = Vector3(2.5f, -5.0f, 0.0f);
	vertexArray[2].u = 1.0f;
	vertexArray[2].v = 1.0f;

	//hier noch schnell fuer die 3 vertices eine farbe vergeben und feddich.
	vertexArray[0].color = vertexArray[1].color = vertexArray[2].color = renderKernel->GetDisplay()->ConvertColor(color);

	return true;
}


//Diese Funktion brauchen wir beim Terrain hoechstwahrscheinlich nicht. Ist eigentlich nur die CollisionDetection
//fuer 'normale' Objekte
virtual void JETAPI TerrainManager::ResetBoundingVolume()
{
	ParticleStructure::ResetBoundingVolume();
	if(boundingVolume) {
		boundingVolume->SetRadius(10.0f);
		boundingVolume->SetRadiusXY(10.0f);
	}
	else {
		BoundingVolume *sphere = NewMem(BoundingVolume(GetPosition(), 10.0f));
		sphere->SetRadiusXY(10.0f);
		SetBoundingVolume(sphere);
	}
}

virtual Modifier::Result JETAPI Update(float dt)
{
	//hier wuerde alles passieren, das irgendwas mit der Geometrie/Texturierung/Faerbung macht.
	//der Sample code wuerde hier jetzt die Farben der vertices aendern, aber das is uns wurscht.
}


virtual void JETAPI TerrainManager::Cull(ViewPort &view, const Matrix4 &trans)
{
	ParticleStructure::Cull(view, trans);

	if(visible) {
		material->Activate();
		view.AddToVisibleList(VisibleElements(trans, this));
	}
	else {
		culled = true;
	}
}

virtual void JETAPI TerrainManager::Render(Display *display, ViewPort &viewport, Matrix4 &transformation)
{
	display->LoadMatrix(transformation);
	Material::GetMaterialManager(renderKernel)->GetTexture(material, 0)->Activate();
	display->DrawArray(D_TRIANGLES, vertexArray, 3);
}

TerrainManager::~TerrainManager()
{

}
