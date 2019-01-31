#include "Matrix4x4.h"

namespace Core
{
	Vector3 GetXAxis(const Matrix4x4 & mat)
	{
		Vector3 xAxis = Vector3(
			mat[0][0],
			mat[1][0],
			mat[2][0]);

		return xAxis;
	}

	Vector3 GetYAxis(const Matrix4x4 & mat)
	{
		Vector3 yAxis = Vector3(
			mat[0][1],
			mat[1][1],
			mat[2][1]);

		return yAxis;
	}

	Vector3 GetZAxis(const Matrix4x4 & mat)
	{
		Vector3 zAxis = Vector3(
			mat[0][2],
			mat[1][2],
			mat[2][2]);

		return zAxis;
	}
}