#pragma once
#include <glm/glm.hpp>
class cBoundingBox
{
public:
	glm::vec3 centerPoint;
	glm::vec3 minPoint;
	glm::vec3 maxPoint;
	glm::vec3 halfExtent;
};

