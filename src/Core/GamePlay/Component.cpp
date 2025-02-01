#include <pch.h>

#include "Component.h"

namespace GuGu {
    meta::Type Component::GetType() const
    {
        return typeof(Component);
    }


	const std::weak_ptr<GameObject>& Component::getParentGameObject() const
	{
		return m_owner;
	}

	std::weak_ptr<GameObject>& Component::getParentGameObject()
	{
		return m_owner;
	}

	void Component::setParentGameObject(const std::weak_ptr<GameObject>& inGameObject)
	{
		m_owner = inGameObject;
	}
}