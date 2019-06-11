#pragma once
#include "MathBase.h"
#include <glm/mat3x3.hpp>
#include "Vector3.h"

namespace Core
{
	typedef glm::mat3x3 Matrix3x3;

	Vector3 GetXAxis(const Matrix3x3 & mat);
	Vector3 GetYAxis(const Matrix3x3 & mat);
	Vector3 GetZAxis(const Matrix3x3 & mat);
}