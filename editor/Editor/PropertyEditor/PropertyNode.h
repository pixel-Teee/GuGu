#pragma once

namespace GuGu {
	class PropertyNode : public std::enable_shared_from_this<PropertyNode>
	{
	public:
		PropertyNode();
		virtual ~PropertyNode();
	};

	class ComplexPropertyNode : public PropertyNode
	{
	public:
		ComplexPropertyNode() : PropertyNode() { }
		virtual ~ComplexPropertyNode() {}


	};
}