#include <pch.h>

#include "LayoutUtils.h"

namespace GuGu {
    math::float2 computePopupFitInRect(const math::box2& inAnchor, 
        const math::box2& popupRect, 
        const Orientation& orientation, 
        const math::box2& rectToFit)
    {
        //pop up rect 和 rect to fit 的相交区域
        math::box2 intersectionBox = popupRect & rectToFit;
        const bool bAdjustmentNeeded = intersectionBox != popupRect;

        if (bAdjustmentNeeded)
        {
            const math::float2 popupPosition = popupRect.m_mins;
            const math::float2& popupSize = popupRect.diagonal();

            //在我们打开的方向，查看是否有充足的空间，如果没有，翻转打开的方向沿着相同的洲
            math::float2 newPosition = math::float2(0, 0);

            if (orientation == Orientation::Horizontal)
            {
                //inanchor.right + popupsize.x < recttofit.right
                const bool bFitsRight = inAnchor.m_maxs.x + popupSize.x < rectToFit.m_maxs.x;
                const bool bFitsLeft = inAnchor.m_mins.x - popupSize.x >= rectToFit.m_mins.x;

                if (bFitsRight || !bFitsLeft)
                {
                    //inachor.right 
                    //inachor.top
                    newPosition = math::float2(inAnchor.m_maxs.x, inAnchor.m_mins.y);
                }
                else
                {
                    newPosition = math::float2(inAnchor.m_mins.x - popupSize.x, inAnchor.m_mins.y);
                }
            }
            else
            {
                const bool bFitsDown = inAnchor.m_maxs.y + popupSize.y < rectToFit.m_maxs.y;
                const bool bFitsUp = inAnchor.m_mins.y - popupSize.y >= rectToFit.m_mins.y;

                if (bFitsDown || !bFitsUp)
                {
                    newPosition = math::float2(inAnchor.m_mins.x, inAnchor.m_maxs.y);
                }
                else
                {
                    newPosition = math::float2(inAnchor.m_mins.x, inAnchor.m_mins.y - popupSize.y);
                }

                if (!bFitsDown && !bFitsUp)
                {
                    return computePopupFitInRect(inAnchor, popupRect, Orientation::Horizontal, rectToFit);
                }
            }

            math::float2 startPos = newPosition;
            math::float2 endPos = newPosition + popupSize;
            math::float2 adjust = math::float2(0, 0);
            if (startPos.x < rectToFit.m_mins.x)
            {
                adjust.x = rectToFit.m_mins.x - startPos.x;
            }

            if (startPos.y < rectToFit.m_mins.y)
            {
                adjust.y = rectToFit.m_mins.y - startPos.y;
            }

            if (endPos.x > rectToFit.m_maxs.x)
            {
                adjust.x = rectToFit.m_maxs.x - endPos.x;
            }

            if (endPos.x > rectToFit.m_maxs.y)
            {
                adjust.y = rectToFit.m_maxs.y - endPos.y;
            }

            newPosition += adjust;

            return newPosition;
        }
        else
        {
            return popupRect.m_mins;
        }
    }
}