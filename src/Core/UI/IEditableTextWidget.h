#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	class Widget;

	enum class CursorMoveMethod
	{
		//移动在基本的方向，箭头左，右，上，下
		Cardinal,
		//移动光标在正确的字符，基于给定的屏幕位置
		ScreenPosition
	};

	enum class CursorMoveGranularity
	{
		//一次移动一个字母
		Character,
		//一次移动一个词
		Word
	};

	enum class CursorAction
	{
		//只是重定位光标
		MoveCursor,
		//移动光标并且选择它通过的任何文本
		SelectText
	};

	class MoveCursor
	{
	public:
		static MoveCursor cardinal(CursorMoveGranularity granularity, math::int2 direction, CursorAction action);

		static MoveCursor viaScreenPointer(math::float2 localPosition, float geometryScale, CursorAction action);

		MoveCursor(CursorMoveGranularity inGranularity, CursorMoveMethod inMethod, math::float2 inDirectionPosition, float inGeometryScale, CursorAction inAction);

		CursorMoveMethod getMoveMethod() const;

		math::float2 getLocalPosition() const;

		float getGeometryScale() const;
	private:
		CursorMoveGranularity m_granularity;
		CursorMoveMethod m_method;
		math::float2 m_directionOrPosition;
		CursorAction m_action;
		float m_geometryScale;
	};

	class IEditableTextWidget
	{
	public:
		virtual std::shared_ptr<Widget> getWidget() = 0;
	};
}