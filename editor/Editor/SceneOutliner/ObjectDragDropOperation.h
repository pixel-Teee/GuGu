#pragma once

#include <Core/UI/DragDropOperation.h>

namespace GuGu {
	namespace meta {
		class Object;
	}
	class ObjectDragDropOperation : public DragDropOperation
	{
	public:
		DRAG_DROP_OPERATOR_TYPE(ObjectDragDropOperation, DragDropOperation)

		ObjectDragDropOperation();
		
		virtual ~ObjectDragDropOperation();

		std::shared_ptr<meta::Object> getObject();

		const std::shared_ptr<meta::Object> getObject() const;

		//作为生成窗口的内容
		virtual std::shared_ptr<Widget> getDefaultDecorator() const override;

		static std::shared_ptr<ObjectDragDropOperation> New(std::shared_ptr<meta::Object> inObject);
	private:
		std::weak_ptr<meta::Object> m_object;
	};
}