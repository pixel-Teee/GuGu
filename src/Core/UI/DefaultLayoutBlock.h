#pragma once

#include <Core/Math/MyMath.h>

#include "IRun.h"
#include "ILayoutBlock.h"
#include "TextRange.h"

namespace GuGu {
	class IRun;
	struct TextRange;
	struct LayoutBlockTextContext;
	class DefaultLayoutBlock : public ILayoutBlock
	{
	public:
		static std::shared_ptr<DefaultLayoutBlock> Create(const std::shared_ptr<IRun>& inRun, const TextRange& inRange, math::float2 inSize, const LayoutBlockTextContext& inTextContext)
		{
			return std::make_shared<DefaultLayoutBlock>(inRun, inRange, inSize, inTextContext);
		}

		DefaultLayoutBlock(const std::shared_ptr<IRun>& inRun, const TextRange& inRange, math::float2 inSize, const LayoutBlockTextContext& inTextContext);

		virtual ~DefaultLayoutBlock() {}

		virtual TextRange getTextRange() const override;

		virtual std::shared_ptr<IRun> getRun() const override;

		virtual void setLocationOffset(const math::float2& inLocationOffset) override;

		virtual math::float2 getLocationOffset() const override;

		virtual math::float2 getSize() const override;

		virtual LayoutBlockTextContext getTextContext() const override;
	private:
		std::shared_ptr<IRun> m_run;

		TextRange m_range;
		math::float2 m_size;
		math::float2 m_locationOffset;//块的左上角起始点，每个块都有这个，绘制的时候使用
		LayoutBlockTextContext m_textContext;
	};
}