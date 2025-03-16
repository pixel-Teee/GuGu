#pragma once

namespace GuGu {
	class PropertyNode : public std::enable_shared_from_this<PropertyNode>
	{
	public:
		PropertyNode();
		virtual ~PropertyNode();
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