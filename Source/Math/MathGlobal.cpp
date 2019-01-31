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

	Matrix4x4 LookAt(const Vector3 & position, const Vector3 & lookDir, const Vector3 & up)
	{
		return glm::lookAt(position, lookDir, up);
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
}