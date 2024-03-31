#include <pch.h>

#include "IEditableTextWidget.h"

namespace GuGu {
    MoveCursor MoveCursor::cardinal(CursorMoveGranularity granularity, math::int2 direction, CursorAction action)
    {
        return MoveCursor(granularity, CursorMoveMethod::Cardinal, math::float2(direction.x, direction.y), 1.0f, action);
    }
    MoveCursor MoveCursor::viaScreenPointer(math::float2 localPosition, float geometryScale, CursorAction action)
    {
        return MoveCursor(CursorMoveGranularity::Character, CursorMoveMethod::ScreenPosition, localPosition, geometryScale, action);
    }

    MoveCursor::MoveCursor(CursorMoveGranularity inGranularity, CursorMoveMethod inMethod, math::float2 inDirectionPosition, float inGeometryScale, CursorAction inAction)
        : m_granularity(inGranularity)
        , m_method(inMethod)
        , m_directionOrPosition(inDirectionPosition)
        , m_geometryScale(inGeometryScale)
        , m_action(inAction)
    {
    }

    CursorMoveMethod MoveCursor::getMoveMethod() const
    {
        return m_method;
    }

    math::float2 MoveCursor::getLocalPosition() const
    {
        return m_directionOrPosition;
    }

    math::int2 MoveCursor::getMoveDirection() const
    {
        return math::int2(m_directionOrPosition.x, m_directionOrPosition.y);
    }

    CursorAction MoveCursor::getAction() const
    {
        return m_action;
    }

    float MoveCursor::getGeometryScale() const
    {
        return m_geometryScale;
    }
        
}