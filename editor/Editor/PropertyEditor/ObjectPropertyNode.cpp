#include <pch.h>
#include "ObjectPropertyNode.h"

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

}