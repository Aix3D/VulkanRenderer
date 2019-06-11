#pragma once
#include "IComponent.h"
#include "../DesignPattern/Singleton.h"

namespace Core
{
	struct InputStateData : public Singleton<InputStateData>, public IComponent
	{
		Bool inlvaid = False;

		//	KeyboardState
		Bool wDown = False;
		Bool sDown = False;
		Bool aDown = False;
		Bool dDown = False;
		Bool qDown = False;
		Bool eDown = False;
		Bool leftShiftDown = False;

		//	MouseState
		Bool leftBuffonDown = False;
		Bool middleBuffonDown = False;
		Bool rightBuffonDown = False;
		Bool mouseIsMoveing = False;
		uint32 xPos = 0;
		uint32 yPos = 0;
		float zPos = 1;
		uint32 xPosPrev = 0;
		uint32 yPosPrev = 0;
	};
}