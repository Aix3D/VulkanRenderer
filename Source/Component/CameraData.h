#pragma once
#include "IComponent.h"
#include "../Math/Matrix4x4.h"
#include "../Math/Vector3.h"

namespace Core
{
	//	XXX:
	//		��Ҫ��ϸ�˽�һ��cache friendly�Ľṹ�����֯
	struct CameraData : public IComponent
	{
		Matrix4x4 m_perspectiveMatrix;
		Matrix4x4 m_viewMatrix;
		Matrix4x4 m_viewMatrixInv;
		Matrix4x4 m_viewProjectionMatrix;
		Vector3 lookAtCenter;
		Vector3 lookAtDir;
		Vector3 eularAngle;
		Vector3 cameraUp;
		float zNear;
		float zFar;
		float ascept;
		float fovY;


	};
}