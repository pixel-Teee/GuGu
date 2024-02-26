#pragma once

#include "Widget.h"

namespace GuGu {
	class Brush;
	class ImageWidget : public Widget
	{
	public:
		ImageWidget();

		virtual ~ImageWidget();

		virtual void GenerateElement(ElementList& elementList, WidgetGeometry& allocatedGeometry, uint32_t layer);

		virtual math::double2 ComputeFixedSize(float inLayoutScaleMultiplier);

	private:
		std::shared_ptr<Brush> m_imageBursh;
	};
}