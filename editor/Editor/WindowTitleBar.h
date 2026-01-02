#pragma once

#include <Core/UI/CompoundWidget.h>
#include <Core/UI/UIMacros.h>

namespace GuGu {
	class WindowWidget;
	class WindowTitleBar : public CompoundWidget
	{
	public:
		WindowTitleBar();

		virtual ~WindowTitleBar();

		struct BuilderArguments : public Arguments<WindowTitleBar>
		{
			BuilderArguments() {}

			~BuilderArguments() = default;
		};

		void init(const BuilderArguments& arguments, const std::shared_ptr<WindowWidget> inParentWindow);

		Reply exitApplication();

		Reply miniMizeWindow();

		Reply maximizeWindow();

		Reply restoreWindow();

		WindowZone::Type getWindowZoneOverride() const override;

		Visibility getRestoreButtonVisibility() const;
	private:

		std::weak_ptr<WindowWidget> m_parentWindow;

		bool m_bIsMaximize;//maximize
	};
}