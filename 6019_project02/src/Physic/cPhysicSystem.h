#pragma once
#include <vector>
#include <map>
#include "cObject.h"
#include "cTriangle.h"
#include "../VAOManager/cModelDrawInfo.h"
#include "cBoundingBox.h"


class cPhysicSystem
{
public:
	cPhysicSystem();
	~cPhysicSystem();

	void createEnvironment(cModelDrawInfo drawInfo);
	void createObject(cMeshObj* meshObj, cModelDrawInfo* DrawInfo);
	void updateSystem(float dt);

	bool collisionCheck(cObject* pObj, cTriangle* t);

	cBoundingBox boundingBox;
	cModelDrawInfo* pDrawInfo;
	std::vector<cObject*> vec_Objects;
	std::map<int, std::vector<cTriangle*>> mapEnvironmentAABBStructure;
	std::map<int, std::vector<cTriangle*>> mapObjAABBStructure;
};

