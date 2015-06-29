#pragma once
#include "movable.h"

/**Inherits from Movable, adding a matrix implementation of the movements.*/
class Transformable : public Movable
{
protected:
	math::mat4 m_matrix;
public:


	/**Updates the internal matrix if an update is needed.*/
	void updateMatrix();

	/**Forces an update of the internal matrix.*/
	void forceMatrixUpdate();

	/**Returns the internal matrix.*/
	math::mat4& matrix() {return m_matrix;}
};
