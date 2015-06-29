#pragma once
#include "matrixd.h"

//information needed for object: Mesh, Model Matrix

/**
* Takes in 3 2D vectors to represent a 2d triangle and a point, and gives the barycentric coordinates of the point relative to the triangle.
* */
math::vec3 toBarycentric(const math::vec2 v1, const math::vec2 v2, const math::vec2 v3, const math::vec2 p);

/**
* Not really sure what this does...
* Takes in 3 vertices and a barycentric point, and finds the world space of the point.
* */
math::vec3 fromBarycentric(const math::vec3& v1, const math::vec3& v2, const math::vec3& v3, const math::vec3& p);

/**
* Contains information about a ray intersection event.
*/
struct RayIntersectionData
{
	math::vec3 worldPos;
	math::vec3 barycentricPos;
	bool intersects;
	RayIntersectionData(const math::vec3& worldPos_, const math::vec3& barycentricPos_, bool intersects_) :
		worldPos(worldPos_), barycentricPos(barycentricPos_), intersects(intersects_) {}
};




//General case:
//Finds the intersection parameter t where the line is rayOrigin+rayDirection*t with the plane going through the three points of
//the triangle. If no intersection, return negative value if NOT VISIBLE. May intersect, but no visible intersection.
float findRayTrianglePlaneIntersectionParameter(const math::vec3& rayOrigin, const math::vec3& rayVector,
	const math::vec3& tv1, const math::vec3& tv2, const math::vec3& tv3, const math::vec3& normal);
/**
 * Determines whether the ray hits the triangle, and if so, returns the appropriate information.
 */
RayIntersectionData determineRayTriangleIntersection(const math::vec3& rayOrigin, const math::vec3& rayVector,
	const math::vec3& tv1, const math::vec3& tv2, const math::vec3& tv3);