#pragma once

#include "movable.h"


///A class to encapsulate a camera in the game world.
class Camera : public Movable
{
private:
	math::vec3 m_forward;
	math::vec3 m_up;

	math::mat4 m_projectionMatrix;
	math::mat4 m_transformMatrix;
public:

	///Initialises the camera, setting a default projection matrix.
	Camera(float fov, float width, float height, float znear, float zfar);

	void initPerspectiveProjectionMatrix(float screenw, float screenh, float fov, float znear, float zfar);

	void initOrthoProjectionMatrix(float left, float right, float bottom, float top, float near_, float far_);

	const math::mat4& matrix() const;

	const math::mat4& projection() const;

	///Returns the normalised forward vector.
	math::vec3 forwardVector();

	///Returns the normalised up vector.
	math::vec3 upVector();

	math::vec3 leftVector();

	void rotateX(float angle);

	void rotateY(float angle);

	//Flies in the given direction
	void flyAt(const math::vec3& direction, float amount);

	///Moves the camera forward given its orientation.
	void flyForwards(float distance);

	///Moves the camera back given its orientation.
	void flyBackwards(float distance);

	///Moves the camera left given its orientation.
	void flyLeft(float distance);

	///Moves the camera right given its orientation.
	void flyRight(float distance);

	///Moves the camera up.
	void flyUp(float distance);

	///Moves the camera down.
	void flyDown(float distance);


	///Rotates the camera right given its orientation.
	void lookRight(float angle);

	///Rotates the camera left given its orientation.
	void lookLeft(float angle);

	///Rotates the camera up given its orientation.
	void lookUp(float angle);

	///Rotates the camera down given its orientation.
	void lookDown(float angle);

	///Updates the camera matrix.
	void generateMatrix();

	///Resets the camera to its initial state.
	void reset();
};