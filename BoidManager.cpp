#include "BoidManager.h"



BoidManager::BoidManager()
{
}


BoidManager::~BoidManager()
{
}

void BoidManager::allocateBirds()
{
	for (int i = 0; i < maxBirds; ++i)
	{
		m_birds.push_back(new birds(Utils::RandomNumber<float>(-450.0f, 450.0f), Utils::RandomNumber<float>(50.0f, 450.0f), Utils::RandomNumber<float>(-450.0f, 450.0f), Utils::RandomNumber<float>(0.0f, 360.0f), i));
	}
	m_birds.at(0)->LoadResources();
	initialisePositions();
}


void BoidManager::drawBirds()
{
	for (auto ep : m_birds)
	{
		ep->Draw();
	}
}

void BoidManager::deleteBirds()
{
	m_birds.at(0)->ReleaseResources();

	for (int i = 0; i < maxBirds; i++)
	{
		delete m_birds.at(i);
	}
}

void BoidManager::initialisePositions()
{
	for	(int i = 0; i < maxBirds; ++i)
	{
		XMMATRIX mRotX, mRotY, mRotZ, mTrans, WorldMatrix;
		XMFLOAT4 iPos = m_birds.at(i)->getPosition();
		XMFLOAT4 iRot = m_birds.at(i)->getRotation();

		mRotX = XMMatrixRotationX(XMConvertToRadians(iRot.x));
		mRotY = XMMatrixRotationY(XMConvertToRadians(iRot.y));
		mRotZ = XMMatrixRotationZ(XMConvertToRadians(iRot.z));

		mTrans = XMMatrixTranslation(iPos.x, iPos.y, iPos.z);

		WorldMatrix = mRotZ * mRotX * mRotY * mTrans;

		m_birds.at(i)->setWorldMatrix(WorldMatrix);
		m_birds.at(i)->setVector(XMVector4Normalize(WorldMatrix.r[2]));
	}
}

void BoidManager::updateBirdsPosition()
{
	XMVECTOR acceleration, Seek, Avoid, Align, Wall, CurPos;
	XMFLOAT4 newPos;

	float maxSpeed = 0.0f;
	maxSpeed += 10.0f;
	if (maxSpeed > 1.0f)
		maxSpeed = 1.0f;

	float seekFactor = 1.0f;
	float avoidFactor = 2.0f;
	float alignFactor = 1.0f;



	for (int i = 0; i < maxBirds; i++)
	{
		Seek = findSeek(*m_birds.at(i)) * seekFactor;
		Avoid = findAvoid(*m_birds.at(i)) * avoidFactor;
		Align = findAlign(*m_birds.at(i)) * alignFactor;
		
		//Calculate the desired vector
		acceleration = m_birds.at(i)->getVector() + Seek + Align + Avoid;

		//turn around if about to hit wall
		Wall = avoidWall(*m_birds.at(i), acceleration);
		acceleration = vMult(XMVector4Normalize(acceleration), Wall);
		

		CurPos = XMLoadFloat4(&m_birds.at(i)->getPosition());
		CurPos += acceleration;
		XMStoreFloat4(&newPos, CurPos);


		m_birds.at(i)->setVector(acceleration);
		m_birds.at(i)->setPosition(newPos);
		updateWorldMatrix(*m_birds.at(i));
	}

}

void BoidManager::updateWorldMatrix(birds &bird)
{
	XMMATRIX mRotX, mRotY, mRotZ, mTrans, WorldMatrix;
	XMFLOAT4 iPos = bird.getPosition();
	XMFLOAT4 iRot = bird.getRotation();

	mRotX = XMMatrixRotationX(XMConvertToRadians(iRot.x));
	mRotY = XMMatrixRotationY(XMConvertToRadians(iRot.y));
	mRotZ = XMMatrixRotationZ(XMConvertToRadians(iRot.z));

	mTrans = XMMatrixTranslation(iPos.x, iPos.y, iPos.z);

	WorldMatrix = mRotZ * mRotX * mRotY * mTrans;

	bird.setWorldMatrix(WorldMatrix);
}

XMVECTOR BoidManager::findAvoid(birds &bird)
{
	birds tempBird = bird;
	XMVECTOR avoid = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR tempVectorI, tempVectorTB, vectorBetween;
	float distanceBetween;
	
	for (int i = 0; i < maxBirds; i++)
	{
		if (m_birds.at(i)->getID() != tempBird.getID())  //If bird is not itself 
		{
			distanceBetween = getDistanceBetween(getVectorBetween(*m_birds.at(i), tempBird));

			if (distanceBetween < 5.0f)
			{
				avoid = XMVectorSubtract(avoid, getVectorBetween(*m_birds.at(i), tempBird));
				avoid = XMVector4Normalize(avoid);
			}
		}
	}
	return avoid;
}

XMVECTOR BoidManager::findSeek(birds &bird)
{
	birds tempBird = bird;
	XMVECTOR seek = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR tempVectorI;
	XMVECTOR tempVectorTB = XMLoadFloat4(&tempBird.getPosition());
	float distanceBetween;
	bool seeking = false;
	int count = 0;

	for (int i = 0; i < maxBirds; i++)
	{
		if (m_birds.at(i)->getID() != tempBird.getID())
		{
			distanceBetween = getDistanceBetween(getVectorBetween(*m_birds.at(i), tempBird));

			if (distanceBetween < 15.0f)
			{
				tempVectorI = m_birds.at(i)->getVector();
				seek = XMVectorAdd(seek, tempVectorI);
				++count;
			}
		}
	}
	
	if (count > 1)
	{
	seek = XMVectorSubtract(seek, tempVectorTB);
	seek = XMVector4Normalize(seek);
	seek = seek / (count - 1);
	seek = XMVectorSubtract(seek, tempBird.getVector()) / 100;
	}

	return seek;
}

XMVECTOR BoidManager::findAlign(birds &bird)
{
	birds tempBird = bird;
	XMVECTOR Align = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR tempVectorI, tempVectorTB, vectorBetween;
	float distanceBetween;
	int count = 0;
	
	for (int i = 0; i < maxBirds; i++)
	{
		if (m_birds.at(i)->getID() != tempBird.getID())
		{
			distanceBetween = getDistanceBetween(getVectorBetween(*m_birds.at(i), tempBird));

			if (distanceBetween < 10.0f)
			{
				tempVectorI = m_birds.at(i)->getVector();
				Align = XMVectorAdd(Align, tempVectorI);
				++count;
			}
		}
	}

	if (count > 1)
	{
		Align = Align / (count - 1);
		Align = XMVector4Normalize(Align);
		Align = XMVectorSubtract(Align, tempBird.getVector());
	}

	return Align;
}

XMVECTOR BoidManager::avoidWall(birds &bird, XMVECTOR a)
{
	XMVECTOR avoidWall = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	birds tempBird = bird;
	int count = 0;


	//Turn around if near an X edge
	if ((tempBird.getPosX() > 490) || (tempBird.getPosX() < -490))
	{
		avoidWall = negate(a, 0);
	}

	//Turn around if near an Y edge
	if ((tempBird.getPosY() > 490) || (tempBird.getPosY() < 5))
	{
		avoidWall = negate(a, 1);
	}

	//Turn around if near an Z edge
	if ((tempBird.getPosZ() > 490) || (tempBird.getPosZ() < -490))
	{
		avoidWall = negate(a, 2);
	}

	return avoidWall;
}

XMVECTOR BoidManager::getVectorBetween(birds &a, birds &b)
{
	XMVECTOR tempVectorA, tempVectorB, vectorBetween;

	tempVectorA = XMLoadFloat4(&a.getPosition());
	tempVectorB = XMLoadFloat4(&b.getPosition());
	vectorBetween = XMVectorSubtract(tempVectorA, tempVectorB);

	return vectorBetween;
}

float BoidManager::getDistanceBetween(XMVECTOR vectorBetween)
{
	float distanceBetween;

	distanceBetween = XMVectorGetX(XMVector4Length((vectorBetween)));

	return distanceBetween;
}

XMVECTOR BoidManager::negate(XMVECTOR a, int N)
{
	XMVECTOR negated = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	if (N == 0)
	{
		//negated = XMVectorSet(-XMVectorGetX(a), XMVectorGetY(a), XMVectorGetZ(a), XMVectorGetW(a));
		negated = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	}
	else if (N == 1)
	{
		//negated = XMVectorSet(XMVectorGetX(a), -XMVectorGetY(a), XMVectorGetZ(a), XMVectorGetW(a));
		negated = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	}
	else if (N == 2)
	{
		//negated = XMVectorSet(XMVectorGetX(a), XMVectorGetY(a), -XMVectorGetZ(a), XMVectorGetW(a));
		negated = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	}


	return negated;
}

XMVECTOR BoidManager::vMult(XMVECTOR a, XMVECTOR b)
{
	
	float x, y, z;
	
	if (XMVectorGetX(b) == -1.0f)
		{
			x = (XMVectorGetX(a) * XMVectorGetX(b));
		}
		else {
				x = XMVectorGetX(a);
			 }


	if (XMVectorGetY(b) == -1.0f)
		{
			y = (XMVectorGetY(a) * XMVectorGetY(b));
		}
		else {
				y = XMVectorGetY(a);
			 }


	if (XMVectorGetZ(b) == -1.0f)
		{
			z = (XMVectorGetZ(a) * XMVectorGetZ(b));
		}
		else {
				z = XMVectorGetZ(a);
			 }

	XMVECTOR mult = XMVectorSet(x, y, z, 0.0f);

	return mult;
}



