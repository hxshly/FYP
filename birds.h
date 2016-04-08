#pragma once

#include "Application.h"

__declspec(align(16)) class birds
{
public:
	birds(float fX, float fY, float fZ, float randRot, int id);
	~birds();

	static void LoadResources(void);		 
	static void ReleaseResources(void);		
	void Draw(void);

private:

	//Boid attributes
	int birdID;
	XMFLOAT4 mPos;
	XMVECTOR mVector;
	XMFLOAT4 mRot;

	XMFLOAT4 mCamRot;
	XMFLOAT4 mCamOff;
	XMVECTOR mCamWorldPos;
	XMMATRIX mCamWorldMatrix;

	XMMATRIX m_mWorldMatrix;	
	XMMATRIX mBoidCameraRot;				

	static CommonMesh*	s_BoidMesh;   //One Mesh for all boids.
	
public:
	//Getters
	XMFLOAT4 GetCameraPosition(void) { XMFLOAT4 v4Pos; XMStoreFloat4(&v4Pos, mCamWorldPos); return v4Pos; }
	XMFLOAT4 GetFocusPosition(void) { return getPosition(); }

	XMFLOAT4 getRotation(void) { return mRot; }
	float getRotX(void) { return mRot.x; }
	float getRotY(void) { return mRot.y; }
	float getRotZ(void) { return mRot.z; }

	XMFLOAT4 getPosition(void) { return mPos; }
	float getPosX(void) { return mPos.x; }
	float getPosY(void) { return mPos.y; }
	float getPosZ(void) { return mPos.z; }

	XMVECTOR getVector(void) { return mVector; }
	int getID(void) { return birdID; }

	//Setters
	void setWorldMatrix(XMMATRIX &a) { m_mWorldMatrix = a; }
	void setVector(XMVECTOR v) { mVector = v; }
	void setPosition(XMFLOAT4 p) {mPos = p;}
	
	
	//DeclSpec for simd
	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

};