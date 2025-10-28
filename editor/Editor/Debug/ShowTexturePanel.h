#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>
#include <Renderer/Color.h>
//theme editor widget
namespace GuGu {
	class Brush;
	class VerticalBox;
	class ShowTexturePanel : public CompoundWidget
	{
	public:
		ShowTexturePanel();

		virtual ~ShowTexturePanel();

		struct BuilderArguments : public Arguments<ShowTexturePanel>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments);

		void initShowTextureVerticalBox();

		Reply OnMouseWheel(const WidgetGeometry& myGeometry, const PointerEvent& inMouseEvent);
	private:
		std::shared_ptr<VerticalBox> m_showTextureVerticalBox;

		std::shared_ptr<Brush> m_debugFontBrush;

		float m_currentScale;
	};
}