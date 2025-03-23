#pragma once

#include "PropertyNode.h" //complex property node
#include <vector>

namespace GuGu {
	class GameObject;
	class ObjectPropertyNode : public ComplexPropertyNode
	{
	public:
		ObjectPropertyNode();

		virtual ~ObjectPropertyNode();

		void addObject(GameObject* inObject);

		void addObjects(const std::vector<GameObject*>& inObjects);

		virtual ObjectPropertyNode* asObjectNode() override { return this; }
		virtual const ObjectPropertyNode* asObjectNode() const override { return this; }

		int32_t getNumObjects() const { return m_objects.size(); }

		GameObject* getObject(int32_t inIndex);
		const GameObject* getObject(int32_t inIndex) const;
	protected:
		virtual void initChildNodes() override;
	private:
		void internalInitChildNodes(GuGuUtf8Str singleChildName = "");

		std::vector<GameObject*> m_objects;
	};
}