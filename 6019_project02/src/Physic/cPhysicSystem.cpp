#include "cPhysicSystem.h"
#include "math.h"

cPhysicSystem::cPhysicSystem()
{
}

cPhysicSystem::~cPhysicSystem()
{
}

int CalculateHashValue(const glm::vec3& vec)
{
    int hashValue = 0;

    hashValue += ((int)(floor(vec.x + 250) / 10)) * 100000;
    hashValue += ((int)(floor(vec.y) / 10)) *1000;
    hashValue += ((int)(floor(vec.z + 250) / 10)) ;
    return hashValue;
}

// Test if AABB b intersects plane P
int TestAABBPlane(cBoundingBox b, sPlane p)
{
    // These two lines not necessary with a (Center, extents) AABB representation
    glm::vec3 c = (b.maxPoint + b.minPoint) * 0.5f; // Compute AABB center
    glm::vec3 e = b.maxPoint - c; // Compute positive extents
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    float r = e.x * std::abs(p.n.x) + e.y * std::abs(p.n.y) + e.z*(std::abs(p.n.z));
    // Compute distance of box center from plane
    float s = Dot(p.n, c) - p.d;
    // Intersection occurs when distance s falls within [-r, +r] interval
    return std::abs(s) <= r;
}

int TestTriangleAABB(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, cBoundingBox b)
{
    float p0, p1, p2, r;
    // compute box center and extents (if not already given in that format)
    glm::vec3 c = (b.minPoint + b.maxPoint) * 0.5f;
    float e0 = (b.maxPoint.x - b.minPoint.x) * 0.5f;
    float e1 = (b.maxPoint.y - b.minPoint.y) * 0.5f;
    float e2 = (b.maxPoint.z - b.minPoint.z) * 0.5f;
    // Translate triangle as conceptually moving AABB to origin
    v0 = v0 - c;
    v1 = v1 - c;
    v2 = v2 - c;
    // Compute edge vectors for triangle
    glm::vec3 f0 = v1 - v0, f1 = v2 - v1, f2 = v0 - v2;
    // Test axes a00..a22 (category 3)
    glm::vec3 a[3][3];

    a[0][0] = glm::vec3(0.f, -f0.z, f0.y);
    a[0][1] = glm::vec3(0.f, -f1.z, f1.y);
    a[0][2] = glm::vec3(0.f, -f2.z, f2.y);
     
    a[1][0] = glm::vec3(f0.z, 0.f, -f0.x);
    a[1][1] = glm::vec3(f1.z, 0.f, -f1.x);
    a[1][2] = glm::vec3(f2.z, 0.f, -f2.x);
     
    a[2][0] = glm::vec3(-f0.y, f0.x, 0.f);
    a[2][0] = glm::vec3(-f1.y, f1.x, 0.f);
    a[2][0] = glm::vec3(-f2.y, f2.x, 0.f);


    r = e1 * std::abs(f0.z) + e2 * std::abs(f0.y);
    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            p0 = Dot(v0, a[i][j]);
            p1 = Dot(v1, a[i][j]);
            p2 = Dot(v2, a[i][j]);
            
            if (std::fmaxf(-(std::fmaxf(p0, p2)), std::fminf(p0, p2)) > r)
            return 0; // axis is a separating axis

        }
    }

        // Test the three axes corresponding to the face normals of AABB b (category 1).
        // Exit if ...
        // ... [-e0, e0] and [min(v0.x, v1.x, v2.x), max(v0.x, v1.x, v2.x)] do not overlap
    if (std::fmaxf(v0.x, std::fmaxf(v1.x, v2.x)) < -e0 || std::fminf(v0.x, std::fminf(v1.x, v2.x)) > e0) return 0;
    // ... [-e1, e1] and [min(v0.y, v1.y, v2.y), max(v0.y, v1.y, v2.y)] do not overlap
    if (std::fmaxf(v0.y, std::fmaxf(v1.y, v2.y)) < -e1 || std::fminf(v0.y, std::fminf(v1.y, v2.y)) > e1) return 0;
    // ... [-e2, e2] and [min(v0.z, v1.z, v2.z), max(v0.z, v1.z, v2.z)] do not overlap
    if (std::fmaxf(v0.z, std::fmaxf(v1.z, v2.z)) < -e2 || std::fminf(v0.z, std::fminf(v1.z, v2.z)) > e2) return 0;
    // Test separating axis corresponding to triangle face normal (category 2)
    sPlane p;
    p.n = Cross(f0, f1);
    p.d = Dot(p.n, v0);
    return TestAABBPlane(b, p);
}

void cPhysicSystem::createEnvironment(cModelDrawInfo drawInfo)
{
    std::vector<glm::vec3> vertices;
    std::vector<int> triangles;

    for (size_t i = 0; i < drawInfo.numberOfIndices; i+=3)
    {
        glm::vec3 pA = glm::vec3(drawInfo.pVertices[drawInfo.pIndices[i]].x, drawInfo.pVertices[drawInfo.pIndices[i]].y, drawInfo.pVertices[drawInfo.pIndices[i]].z);
        glm::vec3 pB = glm::vec3(drawInfo.pVertices[drawInfo.pIndices[i+1]].x, drawInfo.pVertices[drawInfo.pIndices[i+1]].y, drawInfo.pVertices[drawInfo.pIndices[i+1]].z);
        glm::vec3 pC = glm::vec3(drawInfo.pVertices[drawInfo.pIndices[i+2]].x, drawInfo.pVertices[drawInfo.pIndices[i+2]].y, drawInfo.pVertices[drawInfo.pIndices[i+2]].z);


        int hashA = CalculateHashValue(pA);
        int hashB = CalculateHashValue(pB);
        int hashC = CalculateHashValue(pC);

        cTriangle* tri = new cTriangle(pA, pB, pC);
        mapEnvironmentAABBStructure[hashA].push_back(tri);
        if (hashA != hashB)
        {
            mapEnvironmentAABBStructure[hashB].push_back(tri);
        }
        if (hashA != hashC && hashB != hashC)
        {
            mapEnvironmentAABBStructure[hashC].push_back(tri);

        }
    }
}

void cPhysicSystem::createObject(cMeshObj* meshObj,cModelDrawInfo* DrawInfo)
{
    cObject* plane = new cObject();
    plane->pMeshObj = meshObj;
    pDrawInfo = DrawInfo;
    plane->setMass(1.f);
    plane->pBBox = &boundingBox;
    plane->position = glm::vec3(-50, 50, 70);
    boundingBox.halfExtentOffset = glm::vec3((pDrawInfo->extentX / 2.f), (pDrawInfo->extentY / 2.f), (pDrawInfo->extentZ / 2.f));
    boundingBox.centerPointOffset = glm::vec3(boundingBox.halfExtentOffset.x + pDrawInfo->minX, boundingBox.halfExtentOffset.y + pDrawInfo->minY, boundingBox.halfExtentOffset.z + pDrawInfo->minZ);
    boundingBox.maxPointOffset = glm::vec3(pDrawInfo->maxX, pDrawInfo->maxY, pDrawInfo->maxZ);
    boundingBox.minPointOffset = glm::vec3(pDrawInfo->minX, pDrawInfo->minY, pDrawInfo->minZ);
    vec_Objects.push_back(plane);
    boundingBox.pMeshObj->scale = boundingBox.halfExtentOffset;

    std::vector<glm::vec3> vertices;
    std::vector<int> triangles;
    for (size_t i = 0; i < pDrawInfo->numberOfTriangles; i += 3)
    {
        glm::vec3 pA = glm::vec3(pDrawInfo->pVertices[pDrawInfo->pIndices[i]].x, pDrawInfo->pVertices[pDrawInfo->pIndices[i]].y, pDrawInfo->pVertices[pDrawInfo->pIndices[i]].z) + plane->position;
        glm::vec3 pB = glm::vec3(pDrawInfo->pVertices[pDrawInfo->pIndices[i + 1]].x, pDrawInfo->pVertices[pDrawInfo->pIndices[i + 1]].y, pDrawInfo->pVertices[pDrawInfo->pIndices[i + 1]].z) + plane->position;
        glm::vec3 pC = glm::vec3(pDrawInfo->pVertices[pDrawInfo->pIndices[i + 2]].x, pDrawInfo->pVertices[pDrawInfo->pIndices[i + 2]].y, pDrawInfo->pVertices[pDrawInfo->pIndices[i + 2]].z) + plane->position;

        int hashA = CalculateHashValue(pA);
        int hashB = CalculateHashValue(pB);
        int hashC = CalculateHashValue(pC);

        cTriangle* tri = new cTriangle(pA, pB, pC);
        mapObjAABBStructure[hashA].push_back(tri);
        if (hashA != hashB)
        {
            mapObjAABBStructure[hashB].push_back(tri);
        }
        if (hashA != hashC && hashB != hashC)
        {
            mapObjAABBStructure[hashC].push_back(tri);
        }
    }

}

void cPhysicSystem::updateSystem(float dt)
{
    size_t numObj = vec_Objects.size();
    if (numObj == 0) 
    {
        return;
    }

    for (size_t i = 0; i < numObj; i++)
    {
        //vec_Objects[i]->applyForce(glm::vec3(0, -0.0981f, 0));
        vec_Objects[i]->integrate(dt);
    }

    for (size_t i = 0; i < numObj; i++)
    {
        vec_Objects[i]->killAllForces();
        vec_Objects[i]->update();
    }
    for (size_t i = 0; i < numObj; i++)
    {
        int hash = CalculateHashValue(vec_Objects[i]->position);
        auto hash_it = mapEnvironmentAABBStructure.find(hash);
        if (hash_it !=mapEnvironmentAABBStructure.end())
        {
            for (size_t j = 0; j < mapEnvironmentAABBStructure[hash].size(); j++)
            {
                collisionCheck(vec_Objects[i]->pBBox , mapEnvironmentAABBStructure[hash].at(j));
            }
        }
    }
}

bool cPhysicSystem::collisionCheck(cBoundingBox* aabb, cTriangle* t)
{
    int result = TestTriangleAABB(t->pointA, t->pointB, t->pointC, *aabb);
    if (result == 0)
    {
        return false;
    }
    return true;
}
