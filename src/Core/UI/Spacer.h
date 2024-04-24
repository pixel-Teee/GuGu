#pragma once

#include "Widget.h"
#include "UIMacros.h"

namespace GuGu {
	class Spacer : public Widget
	{
	public:
		Spacer();

		virtual ~Spacer();

		struct BuilderArguments : public Arguments<Spacer>
		{
			BuilderArguments()
				: msize(math::float2(0, 0))
			{
				//mVisibility = Visibility::SelfHitTestInvisible;
			}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(math::float2, size)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;

		virtual math::float2 ComputeFixedSize(float inLayoutScaleMultiplier);

	private:
		//Attribute<std::shared_ptr<Brush>> m_imageBursh;
		Attribute<math::float2> m_spacerSize;
	};
}