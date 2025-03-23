#pragma once

#include <Core/Reflection/Field.h>

namespace GuGu {
	class PropertyNode;
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
	protected:
		void destoryTree();

		std::vector<std::shared_ptr<PropertyNode>> m_childNodes;

		std::weak_ptr<PropertyNode> m_parentNodeWeakPtr;

		PropertyNode* m_parentNode;

		//这个节点监视和修改的属性
		meta::Field* m_property;
	};
	class ObjectPropertyNode;
	class ComplexPropertyNode : public PropertyNode
	{
	public:
		ComplexPropertyNode() : PropertyNode() { }
		virtual ~ComplexPropertyNode() {}

		virtual ObjectPropertyNode* asObjectNode() { return nullptr; }
		virtual const ObjectPropertyNode* asObjectNode() const { return nullptr; }
	};
}