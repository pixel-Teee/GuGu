#pragma once

#include <Core/UI/StyleSet.h>

namespace GuGu {
	class EditorStyleSet : public StyleSet
	{
	public:
		EditorStyleSet();

		virtual ~EditorStyleSet();

		static std::shared_ptr<StyleSet> getStyleSet();
	private:
	};
}