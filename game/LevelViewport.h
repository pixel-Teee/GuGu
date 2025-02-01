#pragma once

#include <Core/UI/Viewport.h>

namespace GuGu {
	class LevelViewport : public ViewportWidget
	{
	public:
		struct BuilderArguments : public Arguments<LevelViewport>
		{
			BuilderArguments() = default;

			~BuilderArguments() = default;

			ARGUMENT_NAMED_SLOT(SingleChildSlot, Content)
		};

		void init(const BuilderArguments& arguments);

		virtual void Tick(const WidgetGeometry& allocatedGeometry, const double inCurrentTime, const float inDeltaTime) override;

		virtual bool supportsKeyboardFocus() const override;
	private:
		//EditorCamera m_editorCamera;
	};
}