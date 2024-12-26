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

		struct BuilderArguments : public Arguments<ImageWidget>
		{
			BuilderArguments() {
				mVisibility = Visibility::SelfHitTestInvisible;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(std::shared_ptr<Brush>, brush)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

	private:
		Attribute<std::shared_ptr<Brush>> m_imageBursh;
	};
}