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

		void finalize();

		meta::Type getObjectBaseClass() { return m_baseClass.IsValid() ? m_baseClass : meta::Type::Invalid(); }

		virtual std::vector<meta::Type> getAllStructures() override;
	protected:
		virtual void initChildNodes() override;

		virtual void initBeforeNodeFlags() override;

		void setBestBaseClass();
	private:
		void internalInitChildNodes(GuGuUtf8Str singleChildName = "");

		void getCategoryProperties(const std::set<meta::Type>& classesToConsider, const meta::Field currentProperty, std::set<GuGuUtf8Str>& categoriesFromProperties, std::vector<GuGuUtf8Str>& sortedCategories);

		void getCategoryProperties(const std::set<meta::Type>& classesToConsider, const meta::Type currentProperty, std::set<GuGuUtf8Str>& categoriesFromProperties, std::vector<GuGuUtf8Str>& sortedCategories);

		std::vector<GameObject*> m_objects;

		meta::Type m_baseClass;
	};
}