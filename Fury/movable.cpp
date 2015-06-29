#include "movable.h"

Movable::Movable() : m_pendingMatrixUpdate(false), m_scale(1.f) {}

Movable::Movable(float x, float y, float z) : m_pendingMatrixUpdate(true)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
	m_scale = 1.f;
}

void Movable::setPos(float x, float y, float z)
{
	m_pos.x = x;
	m_pos.y = y;
	m_pos.z = z;
	m_pendingMatrixUpdate = true;
}

void Movable::setPos(const math::vec3 & pos)
{
	m_pos = pos;
	m_pendingMatrixUpdate = true;
}

void Movable::setRot(const math::Quaternion & quat)
{
	m_rot = quat;
	m_pendingMatrixUpdate = true;
}

void Movable::setScale(float scale)
{
	m_scale = scale;
	m_pendingMatrixUpdate = true;
}

void Movable::move(float x, float y, float z)
{
	m_pos.x += x;
	m_pos.y += y;
	m_pos.z += z;
	m_pendingMatrixUpdate = true;
}

void Movable::move(const math::vec3& delta)
{
	m_pos.x+=delta.x;
	m_pos.y+=delta.y;
	m_pos.z+=delta.z;
	m_pendingMatrixUpdate = true;
}

void Movable::rotate(float x, float y, float z, float w)
{
	// ///
	setRot(math::Quaternion(x, y, z, w)); //To be extended
	m_pendingMatrixUpdate = true;
}

void Movable::rotate(const math::Quaternion & quat)
{
	m_rot = quat*m_rot;
	m_pendingMatrixUpdate = true;
}

void Movable::scale(float scale)
{
	m_scale *= scale;
	m_pendingMatrixUpdate = true;
}

math::vec3 Movable::getPos()
{
	return m_pos;
}

math::Quaternion Movable::getRot()
{
	return m_rot;
}

float Movable::getScale()
{
	return m_scale;
}

float Movable::getPosX()
{
	return m_pos.x;
}

float Movable::getPosY()
{
	return m_pos.y;
}

float Movable::getPosZ()
{
	return m_pos.z;
}

float Movable::getRotX()
{
	return m_rot.x;
}

float Movable::getRotY()
{
	return m_rot.y;
}

float Movable::getRotZ()
{
	return m_rot.z;
}

float Movable::getRotW()
{
	return m_rot.w;
}

void Movable::resetPos()
{
	m_pos.reset();
	m_rot.reset();
	m_scale = 0;
	m_pendingMatrixUpdate = true;
}

bool Movable::pendingUpdate()
{
	return m_pendingMatrixUpdate;
}