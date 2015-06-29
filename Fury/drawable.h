#pragma once
#include "matrixd.h"
#include "camera.h"
class RenderWindow;

class Drawable
{
protected:
	enum Type {TYPE_STATICMESH};

	Type mType;
	Drawable(Type type) {mType=type;}

public:
	virtual void draw(RenderWindow* renderWindow, const math::mat4& initialTransformMatrix, const Camera& camera) = 0;
};