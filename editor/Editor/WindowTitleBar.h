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

		WindowZone::Type getWindowZoneOverride() const override;
	private:

		std::weak_ptr<WindowWidget> m_parentWindow;
	};
}