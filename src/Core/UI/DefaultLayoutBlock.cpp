#include <pch.h>

#include "DefaultLayoutBlock.h"

namespace GuGu {

	DefaultLayoutBlock::DefaultLayoutBlock(const std::shared_ptr<IRun>& inRun, const TextRange& inRange, math::float2 inSize, const LayoutBlockTextContext& inTextContext)
		: m_run(inRun)
		, m_range(inRange)
		, m_size(inSize)
		, m_textContext(inTextContext)
	{
	}
	TextRange DefaultLayoutBlock::getTextRange() const
	{
		return m_range;
	}
	std::shared_ptr<IRun> DefaultLayoutBlock::getRun() const
	{
		return m_run;
	}
	void DefaultLayoutBlock::setLocationOffset(const math::float2& inLocationOffset)
	{
		m_locationOffset = inLocationOffset;
	}
	math::float2 DefaultLayoutBlock::getSize() const
	{
		return m_size;
	}
	LayoutBlockTextContext DefaultLayoutBlock::getTextContext() const
	{
		return m_textContext;
	}
}