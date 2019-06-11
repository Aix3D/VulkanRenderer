#pragma once
#include "IComponent.h"
#include "../Math/Vector4.h"
#include "../Math/Vector3.h"

namespace Core
{
	struct TransformData : public IComponent
	{
		Vector3 position;
		Vector3 rotation;
		Vector3 scale;
	};
}