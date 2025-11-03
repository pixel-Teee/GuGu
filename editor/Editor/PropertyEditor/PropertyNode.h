#pragma once

#include <Core/Reflection/Field.h>
#include "PropertyEditorManager.h"

namespace GuGu {
	class PropertyNode;
	class CategoryPropertyNode;
	class ItemPropertyNode;
	class ObjectPropertyNode;
	class ComplexPropertyNode;
	struct PropertyNodeInitParams
	{
		//这个节点的父节点
		std::shared_ptr<PropertyNode> m_parentNode;
		//这个节点监视和修改的属性
		meta::Field* m_property;
	};
	class PropertyNode : public std::enable_shared_from_this<PropertyNode>
	{
	public:
		PropertyNode();
		virtual ~PropertyNode();

		void initNode(const PropertyNodeInitParams& initParams);

		void rebuildChildren();

		virtual void initChildNodes() = 0;

		meta::Field* getField() { return m_property; }
		const meta::Field* getField() const { return m_property; }

		int32_t getNumChildNodes() const { return m_childNodes.size(); }

		std::shared_ptr<PropertyNode>& getChildNode(const int32_t childIndex)
		{
			return m_childNodes[childIndex];
		}

		std::shared_ptr<PropertyNode> findChildPropertyNode(const GuGuUtf8Str& inPropertyName, bool bRecurse = false);

		virtual CategoryPropertyNode* asCategoryNode() { return nullptr; }
		virtual const CategoryPropertyNode* asCategoryNode() const { return nullptr; }
		virtual ItemPropertyNode* asItemPropertyNode() { return nullptr; }
		virtual const ItemPropertyNode* asItemPropertyNode() const { return nullptr; }
		virtual ObjectPropertyNode* asObjectNode() { return nullptr; }
		virtual const ObjectPropertyNode* asObjectNode() const { return nullptr; }
		virtual ComplexPropertyNode* asComplexNode() { return nullptr; }
		virtual const ComplexPropertyNode* asComplexNode() const { return nullptr; }

		void addChildNode(std::shared_ptr<PropertyNode> inNode);

		PropertyNode* getParentNode() { return m_parentNodeWeakPtr.lock().get(); }
		const PropertyNode* getParentNode() const { return m_parentNodeWeakPtr.lock().get(); }

		ComplexPropertyNode* findComplexParent();

		ObjectPropertyNode* findObjectItemParent();

		virtual GuGuUtf8Str getDisplayName() const { return ""; }

		virtual meta::Variant getOwnerFieldVariant(const meta::Variant& startVarint);

		PropertyAccess::Result getPropertyValueString(GuGuUtf8Str& outString);
	protected:
		void destoryTree();

		virtual void initBeforeNodeFlags() {}

		std::vector<std::shared_ptr<PropertyNode>> m_childNodes;

		std::weak_ptr<PropertyNode> m_parentNodeWeakPtr;

		PropertyNode* m_parentNode;

		//这个节点监视和修改的属性
		meta::Field* m_property;
	};
	class ObjectPropertyNode;
	class GameObject;
	class ComplexPropertyNode : public PropertyNode
	{
	public:
		ComplexPropertyNode() : PropertyNode() { }
		virtual ~ComplexPropertyNode() {}

		virtual ObjectPropertyNode* asObjectNode() { return nullptr; }
		virtual const ObjectPropertyNode* asObjectNode() const { return nullptr; }
		virtual ComplexPropertyNode* asComplexNode() { return this; }
		virtual const ComplexPropertyNode* asComplexNode() const { return this; }

		virtual std::vector<meta::Type> getAllStructures() = 0;

		virtual int32_t getInstancesNum() const = 0;

		virtual meta::Object* getInstanceAsObject(int32_t index) const = 0;
	};
}