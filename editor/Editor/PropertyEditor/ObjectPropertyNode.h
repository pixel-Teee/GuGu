#pragma once

#include "PropertyNode.h" //complex property node
#include <vector>

namespace GuGu {
	namespace meta
	{
		class Object;
	}
	
	class ObjectPropertyNode : public ComplexPropertyNode
	{
	public:
		ObjectPropertyNode();

		virtual ~ObjectPropertyNode();

		void addObject(meta::Object* inObject);

		void addObjects(const std::vector<meta::Object*>& inObjects);

		virtual ObjectPropertyNode* asObjectNode() override { return this; }
		virtual const ObjectPropertyNode* asObjectNode() const override { return this; }

		int32_t getNumObjects() const { return m_objects.size(); }

		meta::Object* getObject(int32_t inIndex);
		const meta::Object* getObject(int32_t inIndex) const;

		void finalize();

		meta::Type getObjectBaseClass() { return m_baseClass.IsValid() ? m_baseClass : meta::Type::Invalid(); }

		virtual std::vector<meta::Type> getAllStructures() override;

		virtual int32_t getInstancesNum() const override { return getNumObjects(); }

		virtual meta::Object* getInstanceAsObject(int32_t index) const override;

		virtual meta::Variant getOwnerFieldVarint(const meta::Variant& startVarint) override;
	protected:
		virtual void initChildNodes() override;

		virtual void initBeforeNodeFlags() override;

		void setBestBaseClass();
	private:
		void internalInitChildNodes(GuGuUtf8Str singleChildName = "");

		void getCategoryProperties(const std::set<meta::Type>& classesToConsider, const meta::Field currentProperty, std::set<GuGuUtf8Str>& categoriesFromProperties, std::vector<GuGuUtf8Str>& sortedCategories);

		void getCategoryProperties(const std::set<meta::Type>& classesToConsider, const meta::Type currentProperty, std::set<GuGuUtf8Str>& categoriesFromProperties, std::vector<GuGuUtf8Str>& sortedCategories);

		std::vector<meta::Object*> m_objects;

		meta::Type m_baseClass;
	};
}