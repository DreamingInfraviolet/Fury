#pragma once

/**The VMP model format header struct to be used when loading models.*/
struct VMFHeader
{
	unsigned vertCount;
	unsigned indexCount;
	float	 objectRadius;
	float centrePointX;
	float centrePointY;
	float centrePointZ;
};