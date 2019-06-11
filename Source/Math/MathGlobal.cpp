#include "MathGlobal.h"

namespace Core
{
	Matrix4x4 Perspective(float fovY, float aspect, float zNear, float zFar)
	{
		return glm::perspective(fovY, aspect, zNear, zFar);
	}

	Matrix4x4 Translate(const Matrix4x4 & m, const Vector3 & v)
	{
		return  glm::translate(m, v);
	}

	Matrix4x4 TranslateTo(const Vector3 & v)
	{
		return  glm::translate(Matrix4x4Identify, v);
	}

	Matrix4x4 Rotate(const Matrix4x4 & m, float angle, const Vector3 & v)
	{
		return glm::rotate(m, angle, v);
	}

	Matrix3x3 Rotate(const Matrix3x3 & m, float angle, const Vector3 & v)
	{
		Matrix4x4 tempMat = glm::rotate(Matrix4x4Identify, angle, v);

		Matrix3x3 result;

		result[0][0] = tempMat[0][0];
		result[0][1] = tempMat[0][1];
		result[0][2] = tempMat[0][2];

		result[1][0] = tempMat[1][0];
		result[1][1] = tempMat[1][1];
		result[1][2] = tempMat[1][2];

		result[2][0] = tempMat[2][0];
		result[2][1] = tempMat[2][1];
		result[2][2] = tempMat[2][2];

		return result;
	}

	Matrix4x4 LookAt(const Vector3 & position, const Vector3 & center, const Vector3 & up)
	{
		return glm::lookAt(position, center, up);
	}

	Matrix4x4 Scale(const Matrix4x4 & m, const Vector3 & v)
	{
		return glm::scale(m, v);
	}

	Matrix4x4 Inverse(const Matrix4x4 & m)
	{
		return glm::inverse(m);
	}

	Core::Matrix4x4 Transpose(const Matrix4x4 & m)
	{
		return glm::transpose(m);
	}

	Vector3 Normalize(const Vector3 & v)
	{
		return glm::normalize(v);
	}

	float Dot(const Vector3 & x, const Vector3 & y)
	{
		return glm::dot(x, y);
	}

	Vector3 Cross(const Vector3 & x, const Vector3 & y)
	{
		return glm::cross(x, y);
	}

	float Cross(const Vector2 & x, const Vector2 & y)
	{
		return x.x * y.y - y.x * x.y;
	}

	float Length(const Vector3 & v)
	{
		return glm::length(v);
	}

	int32 Clamp(int32 value, int32 left, int32 right)
	{
		if (value < left)
			return left;

		if (value > right)
			return right;

		return value;
	}

	int32 ClampToLeft(int32 value, int32 left)
	{
		if (value < left)
			return left;

		return value;
	}

	int32 ClampToRight(int32 value, int32 right)
	{
		if (value > right)
			return right;

		return value;
	}

	float Clamp(float value, float left, float right)
	{
		if (value < left)
			return left;

		if (value > right)
			return right;

		return value;
	}

	float ClampToLeft(float value, float left)
	{
		if (value < left)
			return left;

		return value;
	}

	float ClampToRight(float value, float right)
	{
		if (value > right)
			return right;

		return value;
	}

	Bool Equal(Vector2 & left, Vector2 & right)
	{
		return abs(left.x - right.x) < Hidden::ComparisonThreshold &&
			abs(left.y - right.y) < Hidden::ComparisonThreshold;
	}

	Bool Equal(Vector3 & left, Vector3 & right)
	{
		return abs(left.x - right.x) < Hidden::ComparisonThreshold &&
			abs(left.y - right.y) < Hidden::ComparisonThreshold &&
			abs(left.z - right.z) < Hidden::ComparisonThreshold;
	}

	Bool Equal(Vector4 & left, Vector4 & right)
	{
		return abs(left.x - right.x) < Hidden::ComparisonThreshold &&
			abs(left.y - right.y) < Hidden::ComparisonThreshold &&
			abs(left.z - right.z) < Hidden::ComparisonThreshold &&
			abs(left.w - right.w) < Hidden::ComparisonThreshold;
	}

	int32 ToUpper(float value)
	{
		int32 clampped = (int32)value;

		if (value - clampped > 0)
			return clampped + 1;
		else
			return clampped;
	}

	int32 ToLower(float value)
	{
		return int32(value);
	}

	float Max(float left, float right)
	{
		if (left > right)
			return left;
		else
			return right;
	}

	int32 Abs(int32 value)
	{
		if (value < 0)
			return -value;
		else
			return value;
	}

	float Abs(float value)
	{
		if (value < 0)
			return -value;
		else
			return value;
	}

	Matrix4x4 GetReflectionMatrix(Vector4 plane)
	{
		Matrix4x4 v_refMatrix = Matrix4x4Identify;

		v_refMatrix[0][0] = (1.0f - 2.0f * plane[0] * plane[0]);
		v_refMatrix[0][1] = (-2.0f * plane[0] * plane[1]);
		v_refMatrix[0][2] = (-2.0f * plane[0] * plane[2]);
		v_refMatrix[0][3] = 0;

		v_refMatrix[1][0] = (-2.0f * plane[1] * plane[0]);
		v_refMatrix[1][1] = (1.0f - 2.0f * plane[1] * plane[1]);
		v_refMatrix[1][2] = (-2.0f * plane[1] * plane[2]);
		v_refMatrix[1][3] = 0;

		v_refMatrix[2][0] = (-2.0f * plane[2] * plane[0]);
		v_refMatrix[2][1] = (-2.0f * plane[2] * plane[1]);
		v_refMatrix[2][2] = (1.f - 2.0f * plane[2] * plane[2]);
		v_refMatrix[2][3] = 0;

		v_refMatrix[3][0] = (-2.0f * plane[3] * plane[0]);
		v_refMatrix[3][1] = (-2.0f * plane[3] * plane[1]);
		v_refMatrix[3][2] = (-2.0f * plane[3] * plane[2]);
		v_refMatrix[3][3] = 1.0f;

		return v_refMatrix;
	}
}
