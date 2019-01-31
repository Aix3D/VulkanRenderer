#pragma once
#include "MathBase.h"
#include <glm/mat4x4.hpp>
#include "Vector3.h"
#include "Vector4.h"

namespace Core
{
	typedef glm::mat4x4 Matrix4x4;

	Vector3 GetXAxis(const Matrix4x4 & mat);
	Vector3 GetYAxis(const Matrix4x4 & mat);
	Vector3 GetZAxis(const Matrix4x4 & mat);
}