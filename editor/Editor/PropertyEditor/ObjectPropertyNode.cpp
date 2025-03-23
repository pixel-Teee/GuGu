#include <pch.h>
#include "ObjectPropertyNode.h"

#include <Core/Reflection/Type.h>
#include <Core/GamePlay/GameObject.h>

namespace GuGu {

	ObjectPropertyNode::ObjectPropertyNode()
	{

	}

	ObjectPropertyNode::~ObjectPropertyNode()
	{

	}

	void ObjectPropertyNode::addObject(GameObject* inObject)
	{
		m_objects.push_back(inObject);
	}

	void ObjectPropertyNode::addObjects(const std::vector<GameObject*>& inObjects)
	{
		m_objects.insert(m_objects.end(), inObjects.begin(), inObjects.end());//append
	}

	GuGu::GameObject* ObjectPropertyNode::getObject(int32_t inIndex)
	{
		return m_objects[inIndex];
	}

	const GuGu::GameObject* ObjectPropertyNode::getObject(int32_t inIndex) const
	{
		return m_objects[inIndex];
	}

	void ObjectPropertyNode::initChildNodes()
	{

	}

	void ObjectPropertyNode::internalInitChildNodes(GuGuUtf8Str singleChildName /*= ""*/)
	{
		
		//auto& fields = meta::ReflectionDatabase::Instance().types[type.GetID()].fields;

		std::set<meta::Type> classesToConsider;
		for (int32_t i = 0; i < getNumObjects(); ++i)
		{
			GameObject* teamObject = getObject(i);
			classesToConsider.insert(teamObject->GetType());
		}
	}

}