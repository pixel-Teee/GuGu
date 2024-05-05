#pragma once

#include <functional>

#include <Core/GuGuUtf8Str.h>

#include "BasicElement.h"
#include "Reply.h"

namespace GuGu {
	using OnIsTypedCharValid = std::function<bool(const GuGuUtf8Str)>;
	using OnFloatValueChanged = std::function<void(float)>;
	using OnTextCommitted = std::function<void(const GuGuUtf8Str&, TextCommit::Type)>;

	class Widget;
	class ITableRow;
	class TableViewBase;
	class WidgetGeometry;
	struct PointerEvent;
	template<typename ArgumentType>
	class WidgetDelegates
	{
	public:
		using OnGenerateRow = std::function<std::shared_ptr<ITableRow>(ArgumentType, const std::shared_ptr<class TableViewBase>&)>;

		using OnGetChildren = std::function<void(ArgumentType, std::vector<ArgumentType>&)>;

		using OnExpansionChanged = std::function<void(ArgumentType, bool)>;

		using OnSelectionChanged = std::function<void(ArgumentType, SelectInfo::Type)>;

		using OnGenerateWidget = std::function<std::shared_ptr<Widget>(ArgumentType)>;
	};

	//有时候一些 widgets 请求一些内容去显示，在这时候，它们依赖这个委托
	//例如，一个 pop up 的内容通常被代码所驱动，因此，它们通常是未知的，直到 pop up 被打开，在这时，OnGetContent 被调用
	using OnGetContent = std::function<std::shared_ptr<Widget>()>;

	using SimpleDelegate = std::function<void()>;

	//当颜色改变的时候，进行通知
	using OnLinearColorValueChanged = std::function<void(math::float4)>;

	using PointerEventHandler = std::function<Reply(const WidgetGeometry&, const PointerEvent&)>;
}