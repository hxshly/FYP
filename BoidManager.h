#pragma once

#include <algorithm>
#include <memory>
#include "UtilityFunctions.h"
#include "birds.h"

class birds;

class BoidManager
{
public:
	BoidManager();
	~BoidManager();

	void allocateBirds();
	void updateBirdsPosition();
	void drawBirds();
	void deleteBirds();

	void initialisePositions();
	void updateWorldMatrix(birds &bird);

	XMVECTOR findSeek(birds &bird);
	XMVECTOR findAvoid(birds &bird);
	XMVECTOR findAlign(birds &bird);
	XMVECTOR avoidWall(birds &bird, XMVECTOR a);

	XMVECTOR getVectorBetween(birds &a, birds &b);
	float getDistanceBetween(XMVECTOR vectorBetween);

	XMVECTOR negate(XMVECTOR a, int N);
	XMVECTOR vMult(XMVECTOR a, XMVECTOR b);


	float groupRadius = 100.0f;

	int maxBirds = 1000;
	std::vector<birds*> m_birds;

};

