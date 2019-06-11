#include "Entity.h"

namespace Core
{
	int32 Entity::idSeed = 1;

	Entity::Entity()
	{
		m_id = idSeed;
		++idSeed;
	}

	Core::int32 Entity::GetID() const
	{
		return m_id;
	}

	void Entity::OnPause()
	{

	}

	void Entity::OnResume()
	{

	}

	Entity::~Entity()
	{

	}
}