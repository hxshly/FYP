#include "birds.h"

// Initialise static class variable
CommonMesh* birds::s_BoidMesh = NULL;

birds::birds(float fX, float fY, float fZ, float randRot, int id)
{
	m_mWorldMatrix = XMMatrixIdentity();
	
	mPos = XMFLOAT4(fX, fY, fZ, 0.0f);
	mRot = XMFLOAT4(randRot, randRot, randRot, 0.0f);
	mVector = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	mCamOff = XMFLOAT4(0.0f, 100.0f, -100.0f, 0.0f);
	mCamRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	birdID = id;
}

birds::~birds()
{
}

void birds::LoadResources(void)
{
	s_BoidMesh = CommonMesh::LoadFromXFile(Application::s_pApp, "Resources/Robot/body.x");
}

void birds::ReleaseResources(void)
{
	delete s_BoidMesh;
}

void birds::Draw(void)
{
	Application::s_pApp->SetWorldMatrix(m_mWorldMatrix);
	s_BoidMesh->Draw();
}

