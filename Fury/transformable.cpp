#include "transformable.h"

void Transformable::updateMatrix()
{
	if (m_pendingMatrixUpdate)
		forceMatrixUpdate();
}

void Transformable::forceMatrixUpdate()
{
	m_matrix.reset();
	m_matrix *= math::mat4().initTranslation(m_pos);
	m_matrix *= m_rot.normalize().toMatrix();
	m_matrix *= math::mat4().initScale(m_scale);

	m_pendingMatrixUpdate = false; //Signals that no update is needed
}
