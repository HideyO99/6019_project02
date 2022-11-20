#pragma once
#include "cBoundingBox.h"
#include "../MeshObj/cMeshObj.h"

class cObject
{
public:
	cObject();
	~cObject();

	void setMass(float mass);
	void integrate(float dT);
	void applyForce(const glm::vec3& f_dir);
	void killAllForces();
	void update();

	cMeshObj* pMeshObj;

	cBoundingBox* pBBox;

	glm::vec3 position;
	glm::vec3 prevPosition;
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;
	float damping;
	float invMass;
};

