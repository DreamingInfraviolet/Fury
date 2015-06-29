#pragma once
#include "matrixd.h"
/**A class that handles basic conceptual movement/transformation and world placement in 3D, not implementing any matrices, etc.*/
class Movable
{
protected:
	math::vec3 m_pos;
	math::Quaternion m_rot;
	float m_scale;

protected:
	bool m_pendingMatrixUpdate;

public:

	/**Initialises the pendingUpdate variable to true.*/
	Movable();

	/**Constructor that acts as the default one, but also sets the position to the given value.*/
	Movable(float x, float y, float z);

	/**Sets the position of the object.*/
	void setPos(float x, float y, float z);
	/**Sets the position of the object.*/
	void setPos(const math::vec3 & pos);
	/**Sets the rotation of the object.*/
	void setRot(const math::Quaternion & quat);
	/**Sets the overall of the object of all 3 axes.*/
	void setScale(float scale);

	/**Increments the position by the given value.*/
	void move(float x, float y, float z);
	/**Increments the position by the given value.*/
	void move(const math::vec3 & delta);
	/**Increments the rotation towards the given quenaternion.*/
	void rotate(float x, float y, float z, float w);
	/**Increments the rotation towards the given quenaternion.*/
	void rotate(const math::Quaternion & quat);
	/**Multiplies the overall scale by the given value.*/
	void scale(float scale);

	/**Returns the position of the object.*/
	math::vec3 getPos();
	/**Returns the quaternion corresponding to the rotation of the object.*/
	math::Quaternion getRot();
	/**Returns the scale of the object.*/
	float getScale();

	/**Returns the X position.*/
	float getPosX();
	/**Returns the Y position.*/
	float getPosY();
	/**Returns the Z position.*/
	float getPosZ();
	/**Returns the X rotation quaternion component.*/
	float getRotX();
	/**Returns the Y rotation quaternion component.*/
	float getRotY();
	/**Returns the Z rotation quaternion component.*/
	float getRotZ();
	/**Returns the W rotation quaternion component.*/
	float getRotW();

	/**Resets the transformations to zero.*/
	void resetPos();

	/**Returns whether any transformation had taken place after the last update (if applicable).*/
	bool pendingUpdate();
};