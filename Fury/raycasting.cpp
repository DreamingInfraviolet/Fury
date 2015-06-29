#include "raycasting.h"


math::vec3 toBarycentric(const math::vec2 v1, const math::vec2 v2, const math::vec2 v3, const math::vec2 p)
{
	float detInv = 1 / ((v1.x - v3.x)*(v2.y - v3.y) - (v2.x - v3.x)*(v1.y - v3.y));
	math::vec2 pv3Delta = p - v3;
	float outCoord1 = (pv3Delta.x*(v2.y - v3.y) + (v3.x - v2.x)*pv3Delta.y)*detInv;
	float outCoord2 = (pv3Delta.x*(v3.y - v1.y) + (v1.x - v3.x)*pv3Delta.y)*detInv;
	float outCoord3 = 1 - outCoord1 - outCoord2;

	return math::vec3(outCoord1, outCoord2, outCoord3);
}

math::vec3 fromBarycentric(const math::vec3& v1, const math::vec3& v2, const math::vec3& v3, const math::vec3& p)
{
	return math::vec3(
		v1.x*p.x + v2.x*p.x + v3.x*p.x,
		v1.y*p.y + v2.y*p.y + v3.y*p.y,
		v1.z*p.z + v2.z*p.z + v3.z*p.z);
}



float findRayTrianglePlaneIntersectionParameter(const math::vec3& rayOrigin, const math::vec3& rayVector,
	const math::vec3& tv1,  const math::vec3& normal)
{
	//Now, the formula for line/plane intersection:
	float denominator = math::dot(rayVector, normal);
	if (denominator == 0) //Not single point intersection
		return -1;
	else
		return math::dot((tv1 - rayOrigin), normal) / denominator;
}


RayIntersectionData determineRayTriangleIntersection(const math::vec3& rayOrigin, const math::vec3& rayVector,
	const math::vec3& tv1, const math::vec3& tv2, const math::vec3& tv3)
{
	using namespace math;

	vec3 v1 = tv2 - tv1;
	vec3 v2 = tv3 - tv1;

	float rayParameter = findRayTrianglePlaneIntersectionParameter(rayOrigin, rayVector, tv1, cross(v1, v2));
	if (rayParameter < 0) return RayIntersectionData({}, {}, false); //If behind ray, not intersecting, or line on plane

	math::vec3 worldPos = rayOrigin + rayVector*rayParameter;

	float u, v;
	{
		float v1v2 = dot(v1, v2);
		float v2v2 = dot(v2, v2);
		float v1v1 = dot(v1, v1);
		math::vec3 w = worldPos - tv1;
		float wv2 = dot(w, v2);
		float wv1 = dot(w, v1);
		float den = (v1v2*v1v2 - v1v1*v2v2);
		if (den == 0)
			return RayIntersectionData({}, {}, false);
		float dInverse = 1.f / den;

		u = (v1v2*wv2 - v2v2*wv1) * dInverse;
		v = (v1v2*wv1 - v1v1*wv2) * dInverse;
	}

	if (u < 0 || v < 0 || u + v > 1) return RayIntersectionData({}, {}, false); //Not in triangle.

	return RayIntersectionData(worldPos, math::vec3(1 - u - v, u, v), true);
}