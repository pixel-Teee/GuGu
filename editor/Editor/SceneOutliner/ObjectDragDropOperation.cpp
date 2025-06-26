#include <pch.h>

#include "ObjectDragDropOperation.h"
#include <Core/UI/TextBlockWidget.h>
#include <Core/UI/Border.h>

namespace GuGu {

	ObjectDragDropOperation::ObjectDragDropOperation()
	{

	}

	ObjectDragDropOperation::~ObjectDragDropOperation()
	{

	}

	std::shared_ptr<meta::Object> ObjectDragDropOperation::getObject()
	{
		return m_object.lock();
	}

	const std::shared_ptr<meta::Object> ObjectDragDropOperation::getObject() const
	{
		return m_object.lock();
	}

	std::shared_ptr<GuGu::Widget> ObjectDragDropOperation::getDefaultDecorator() const
	{
		std::shared_ptr<Widget> thumbnailWidget;
		if (m_object.lock())
		{
			thumbnailWidget =
				WIDGET_NEW(TextBlockWidget)
				.text(u8"object");
		}
		else
		{
			thumbnailWidget = WIDGET_NEW(TextBlockWidget)
				.text(u8"null");
		}

		return WIDGET_NEW(Border)
			.BorderBackgroundColor(math::float4(0.65f, 0.56f, 0.45f, 1.0f)) //beige7
			.padding(Padding(2.0f))
			.Content
			(
				WIDGET_NEW(Border)
				.BorderBackgroundColor(math::float4(0.79f, 0.74f, 0.68f, 1.0f)) //beige4
				//.padding(Padding(11.0f))
				.Content
				(
					thumbnailWidget
				)
			);
	}

	std::shared_ptr<ObjectDragDropOperation> ObjectDragDropOperation::New(std::shared_ptr<meta::Object> inObject)
	{
		std::shared_ptr<ObjectDragDropOperation> operation = std::make_shared<ObjectDragDropOperation>();

		operation->m_object = inObject;
		operation->construct();
		return operation;
	}

}