#pragma once

#include "Widget.h"
#include "Brush.h"

#include "UIMacros.h"

#include "Attribute.h"

namespace GuGu {
	//class Brush;
	class ImageWidget : public Widget
	{
	public:
		ImageWidget();

		virtual ~ImageWidget();

		struct BuilderArguments
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

	private:
		Attribute<std::shared_ptr<Brush>> m_imageBursh;
	};
}