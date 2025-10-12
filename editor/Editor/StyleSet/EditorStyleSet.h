#pragma once

#include <Core/UI/StyleSet.h>
#include "Theme.h"

namespace GuGu {
	class RootFileSystem;
	class EditorStyleSet : public StyleSet
	{
	public:
		EditorStyleSet();

		virtual ~EditorStyleSet();

		static std::shared_ptr<StyleSet> getStyleSet();

		void loadTheme(const GuGuUtf8Str& themeName);

		void writeTheme(const GuGuUtf8Str& themeName, Theme theme);
	private:
		std::shared_ptr<RootFileSystem> m_rootFileSystem;

		std::map<GuGuUtf8Str, Theme> m_themes;

		GuGuUtf8Str m_currentTheme;
	};
}