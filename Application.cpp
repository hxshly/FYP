#include "Application.h"
#include "Heightmap.h"
#include "Aeroplane.h"
#include "birds.h"

Application* Application::s_pApp = NULL;


const int CAMERA_MAP = 0;
const int CAMERA_BOID = 1;
const int CAMERA_MAX = 2;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool Application::HandleStart()
{
	s_pApp = this;
	this->SetWindowTitle("Murmurations");

	m_bWireframe = false;
	m_pHeightMap = new HeightMap("Resources/black.bmp", 2.0f);

	m_bManager = new BoidManager();
	m_bManager->allocateBirds();

	m_cameraZ = -0.01f;
	m_rotationAngle = 0.f;

	if (!this->CommonApp::HandleStart())
		return false;

	this->SetRasterizerState(false, m_bWireframe);

	m_cameraState = CAMERA_MAP;
	
	return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleStop()
{
	m_bManager->deleteBirds();
	delete m_pHeightMap;
	delete m_bManager;
	this->CommonApp::HandleStop();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleUpdate()
{
	if (m_cameraState == CAMERA_MAP)
	{
		static bool dbQ = false;
		static bool dbA = false;

		//lerp to top down
		if (this->IsKeyPressed('Q'))
		{
			if (!dbQ && !dbA)
			{
				dbQ = true;
			}
		}
		else
		{
			if (m_cameraZ > -0.02f && dbQ)
			{
				dbQ = false;
			}
		}

		if (dbQ)
		{
			m_cameraZ = Utils::lerp<float>(m_cameraZ, -0.01f, 0.15f);
			m_rotationAngle = Utils::lerp<float>(m_rotationAngle, 0.0f, 0.15f);
		}


		//lerp to side view
		if (this->IsKeyPressed('A'))
		{
			if (!dbA && !dbQ)
			{
				dbA = true;
			}
		}
		else
		{
			if (m_cameraZ < -1599.0f)
			{
				dbA = false;
			}
		}

		if (dbA)
		{
			m_cameraZ = Utils::lerp<float>(m_cameraZ, -1600.0f, 0.15f);
		}

		if (m_cameraZ < -1599.0f)
		{
			m_rotationAngle += 0.005f;
		}
	}

	static bool dbC = false;
	if (this->IsKeyPressed('C'))
	{
		if (!dbC)
		{
			if (++m_cameraState == CAMERA_MAX)
				m_cameraState = CAMERA_MAP;

			dbC = true;
		}
	}
	else
	{
		dbC = false;
	}


	static bool dbW = false;
	if (this->IsKeyPressed('W'))
	{
		if (!dbW)
		{
			m_bWireframe = !m_bWireframe;
			this->SetRasterizerState(false, m_bWireframe);
			dbW = true;
		}
	}
	else
	{
		dbW = false;
	}

	m_bManager->updateBirdsPosition();

}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void Application::HandleRender()
{
	XMFLOAT3 vUpVector(0.0f, 1.0f, 0.0f);
	XMFLOAT3 vCamera, vLookat;

	switch (m_cameraState)
	{
	case CAMERA_MAP:
		vCamera = XMFLOAT3(sin(m_rotationAngle)*m_cameraZ, 2000.0f, cos(m_rotationAngle)*m_cameraZ);
		vLookat = XMFLOAT3(0.0f, 0.0f, 0.0f);
		break;
	case CAMERA_BOID:
		//vCamera = XMFLOAT3(m_birds.at(0)->GetCameraPosition().x, m_birds.at(0)->GetCameraPosition().y, m_birds.at(0)->GetCameraPosition().z);
		//vLookat = XMFLOAT3(m_birds.at(0)->GetFocusPosition().x, m_birds.at(0)->GetFocusPosition().y, m_birds.at(0)->GetFocusPosition().z);
		break;
	}

	XMMATRIX  matView;
	matView = XMMatrixLookAtLH(XMLoadFloat3(&vCamera), XMLoadFloat3(&vLookat), XMLoadFloat3(&vUpVector));

	XMMATRIX matProj;
	matProj = XMMatrixPerspectiveFovLH(float(XM_PI / 4), 2, 1.5f, 5000.0f);

	this->SetViewMatrix(matView);
	this->SetProjectionMatrix(matProj);

	this->EnablePointLight(0, XMFLOAT3(100.0f, 100.f, -100.f), XMFLOAT3(1.f, 1.f, 1.f));
	this->SetLightAttenuation(0, 200.f, 2.f, 2.f, 2.f);
	this->EnableDirectionalLight(1, D3DXVECTOR3(-1.f, -1.f, -1.f), D3DXVECTOR3(0.65f, 0.55f, 0.65f));


	//changes the background colour
	this->Clear(XMFLOAT4(.6f, .6f, .6f, 1.f));

	XMMATRIX  matWorld;
	matWorld = XMMatrixIdentity();
	this->SetWorldMatrix(matWorld);

	m_pHeightMap->Draw();
	
	m_bManager->drawBirds();


}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	Application application;

	Run(&application);

	return 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
