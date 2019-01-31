#pragma once
#include "MathBase.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective 
#include <glm/gtc/constants.hpp>
#include "Matrix4x4.h"
#include "Vector3.h"
#include "Vector2.h"

//	不想暴露出来的全局变量,写在另一个namespace里.
namespace Hidden
{
	const float ComparisonThreshold = 0.0000001f;
}

namespace Core
{
	Matrix4x4 Perspective(float fovY, float aspect, float zNear, float zFar);

	Matrix4x4 Translate(const Matrix4x4 & m, const Vector3 & v);

	Matrix4x4 TranslateTo(const Vector3 & v);

	Matrix4x4 Rotate(const Matrix4x4 & m, float angle, const Vector3 & v);

	Matrix4x4 LookAt(const Vector3 & position, const Vector3 & lookDir, const Vector3 & up);

	Matrix4x4 Scale(const Matrix4x4 & m, const Vector3 & v);

	Matrix4x4 Inverse(const Matrix4x4 & m);

	Matrix4x4 Transpose(const Matrix4x4 & m);

	Vector3 Normalize(const Vector3 & v);

	float Dot(const Vector3 & x, const Vector3 & y);

	Vector3 Cross(const Vector3 & x, const Vector3 & y);

	float Cross(const Vector2 & x, const Vector2 & y);

	float Length(const Vector3 & v);

	//	radians,弧度
	//	degree,角度

	//	弧度转角度
	const float Rad2Deg = 57.2957795f;

	//	角度转弧度	
	const float Deg2Rad = 0.0174532925f;

	const Vector3 Up = Vector3(0, 1.0f, 0);
	const Vector4 Up4 = Vector4(0, 1.0f, 0, 0);

	const Vector3 Right = Vector3(1.0f, 0, 0);
	const Vector4 Right4 = Vector4(1.0f, 0, 0, 0);

	const Vector3 Forward = Vector3(0, 0, 1.0f);
	const Vector4 Forward4 = Vector4(0, 0, 1.0f, 0);

	const Vector3 Zero = Vector3(0, 0, 0);

	const float PI = 3.141592654f;

	const Matrix4x4 Matrix4x4Identify = Matrix4x4(
		1.0f, 0, 0, 0,
		0, 1.0f, 0, 0,
		0, 0, 1.0f, 0,
		0, 0, 0, 1.0f);

	const Vector3 Vector3Dummy = Vector3(0, 0, 0);

	const Vector4 Vector4Dummy = Vector4(0, 0, 0, 0);

	int32 Clamp(int32 value, int32 left, int32 right);
	int32 ClampToLeft(int32 value, int32 left);
	int32 ClampToRight(int32 value, int32 right);

	float Clamp(float value, float left, float right);
	float ClampToLeft(float value, float left);
	float ClampToRight(float value, float right);

	Bool Equal(Vector2 & left, Vector2 & right);
	Bool Equal(Vector3 & left, Vector3 & right);
	Bool Equal(Vector4 & left, Vector4 & right);

	int32 ToUpper(float value);
	int32 ToLower(float value);
}