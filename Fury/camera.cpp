#include "camera.h"


Camera::Camera(float fov, float width, float height, float znear, float zfar)
{
	m_projectionMatrix = math::mat4().initProjection(fov, width, height, znear, zfar);
	reset();
}


void Camera::initPerspectiveProjectionMatrix(float screenw, float screenh, float fov, float znear, float zfar)
{
	m_projectionMatrix = math::mat4().initProjection(fov, screenw, screenh, znear, zfar);
}

void Camera::initOrthoProjectionMatrix(float left, float right, float bottom, float top, float near_, float far_)
{
	m_projectionMatrix = math::mat4().initOrthoProjection(left, right, bottom, top, near_, far_);
}

const math::mat4& Camera::matrix() const
{
	return m_transformMatrix;
}

const math::mat4& Camera::projection() const
{
	return m_projectionMatrix;
}

math::vec3 Camera::forwardVector()
{
	return m_forward;
}

math::vec3 Camera::leftVector()
{
	return math::cross(m_up, m_forward).normalize();
}

math::vec3 Camera::upVector()
{
	return m_up;
}

void Camera::flyAt(const math::vec3& direction, float amount)
{
	m_pos -= direction*amount;
}

void Camera::flyForwards(float distance)
{
	m_pos -= m_forward*distance;
}

void Camera::flyBackwards(float distance)
{
	m_pos += m_forward*distance;
}

void Camera::flyLeft(float distance)
{
	m_pos -= leftVector()*distance;
}

void Camera::flyRight(float distance)
{
	m_pos += leftVector()*distance;
}

void Camera::flyUp(float distance)
{
	m_pos.y -= distance;
}

void Camera::flyDown(float distance)
{
	m_pos.y += distance;
}

void Camera::lookLeft(float angle)
{
	math::Quaternion quat;
	math::vec3 hAxis = math::cross(math::vec3(0, 1, 0), m_forward).normalize();
	quat = math::Quaternion().fromAxisRotation(math::vec3(0, 1, 0), angle);
	quat.rotateVector(m_forward);
	m_forward.normalize();
	m_up = math::cross(m_forward, hAxis);
	m_up.normalize();
}

void Camera::lookRight(float angle)
{
	lookLeft(-angle);
}

void Camera::lookDown(float angle)
{
	math::Quaternion quat;
	math::vec3 hAxis = math::cross(math::vec3(0, 1, 0), m_forward).normalize();
	quat = math::Quaternion().fromAxisRotation(hAxis, angle);
	quat.rotateVector(m_forward);
	m_forward.normalize();
	m_up = math::cross(m_forward, hAxis);
	m_up.normalize();
}

void Camera::lookUp(float angle)
{
	lookDown(-angle);
}

void Camera::generateMatrix()
{

	math::vec3 f = m_forward.normalize();
	math::vec3 r = math::cross(m_up.normalize(), f);
	math::vec3 u = math::cross(f, r);

	math::mat4 transform = math::mat4(r.x, r.y, r.z, 0,
		u.x, u.y, u.z, 0,
		f.x, f.y, f.z, 0,
		0, 0, 0, 1);

	m_transformMatrix = transform * math::mat4().initTranslation(m_pos);
}

void Camera::reset()
{
	m_forward = math::vec3(0, 0, -1);
	m_up = math::vec3(0, 1, 0);

	m_transformMatrix.reset();
}