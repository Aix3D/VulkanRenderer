#include "Camera.h"
#include <MathGlobal.h>

namespace Core
{
	Camera::Camera(int32 width, int32 height)
	{
		position = Vector3(0, 0, -8.0f);
		lookAt = Vector3(0, 0, -1.0);
		eularAngle = Vector3Dummy;

		zNear = 1.0f;
		zFar = 300.0f;
		ascept = width / (float)height;
		fovY = 90.0f * Deg2Rad;

		position = Vector3(0, 0, 10.0f);
		lookAt = Vector3(0, 0, -1.0f);
		eularAngle = Vector3Dummy;

		m_perspectiveMatrix = Perspective(fovY, ascept, zNear, zFar);
	}

	void Camera::Tick(float deltaTime, const InputState & inputState)
	{
		if (inputState.inlvaid)
			return;

		float moveSpeed = 30.0f * deltaTime * inputState.zPos;
		float rotateSpeed = 3.0f * deltaTime;

		if (inputState.leftShiftDown)
		{
			moveSpeed *= 2.0f;
			rotateSpeed *= 2.0f;
		}

		if (inputState.wDown)
			position += GetZAxis(m_viewMatrix) * -moveSpeed;

		if (inputState.sDown)
			position += GetZAxis(m_viewMatrix) * moveSpeed;

		if (inputState.aDown)
			position += GetXAxis(m_viewMatrix) * -moveSpeed;

		if (inputState.dDown)
			position += GetXAxis(m_viewMatrix) * moveSpeed;

		//	flip Y
		if (inputState.qDown)
			position += GetYAxis(m_viewMatrix) * moveSpeed;

		//	flip Y
		if (inputState.eDown)
			position += GetYAxis(m_viewMatrix) * -moveSpeed;

		if (inputState.rightBuffonDown)
		{
			if (!m_rightMouseClicked)
			{
				m_rightMousePosPrev.x = static_cast<float>(inputState.xPos);
				m_rightMousePosPrev.y = static_cast<float>(inputState.yPos);

				m_rightMouseClicked = True;
			}
			else
			{
				Vector2 rightMousePos = Vector2(
					inputState.xPos,
					inputState.yPos);

				float xDelta = rightMousePos.x - m_rightMousePosPrev.x;
				float yDelta = rightMousePos.y - m_rightMousePosPrev.y;

				//	flip Y
				yDelta = -yDelta;

				m_viewMatrix = Rotate(m_viewMatrix, 0.2f * xDelta * Deg2Rad, GetYAxis(m_viewMatrix));
				m_viewMatrix = Rotate(m_viewMatrix, 0.2f * yDelta * Deg2Rad, GetXAxis(m_viewMatrix));

				
				lookAt = -GetZAxis(m_viewMatrix);

				m_rightMousePosPrev.x = rightMousePos.x;
				m_rightMousePosPrev.y = rightMousePos.y;
			}
		}
		else
		{
			m_rightMouseClicked = False;
		}

		m_viewMatrix = LookAt(position, position + lookAt, Up);

		//m_viewMatrixInv = Inverse(m_viewMatrix);
		//m_viewProjectionMatrix = m_perspectiveMatrix * m_viewMatrix;
	}

	Camera::~Camera()
	{

	}

}