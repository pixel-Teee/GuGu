#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Renderer/Color.h>

namespace GuGu {
	class SimpleGradient : public CompoundWidget
	{
	public:
		struct BuilderArguments : public Arguments<SimpleGradient>
		{
			BuilderArguments()
			{}

			~BuilderArguments() = default;

			ARGUMENT_ATTRIBUTE(Color, startColor)

			ARGUMENT_ATTRIBUTE(Color, endColor)

			ARGUMENT_ATTRIBUTE(bool, hasAlphaBackground)

			ARGUMENT_ATTRIBUTE(Orientation, orientation)
		};

		void init(const BuilderArguments& arguments);

		virtual uint32_t onGenerateElement(PaintArgs& paintArgs, const math::box2& cullingRect, ElementList& elementList, const WidgetGeometry& allocatedGeometry, uint32_t layer) override;
	private:
		Attribute<Color> m_startColor;

		Attribute<Color> m_endColor;

		bool m_bHasAlphaBackground;

		Orientation m_orientation;
	};
}