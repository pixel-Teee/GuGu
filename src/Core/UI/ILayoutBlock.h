#pragma once

namespace GuGu {
	class ILayoutBlock
	{
	public:
		virtual ~ILayoutBlock() {}

		virtual TextRange getTextRange() const = 0;

		virtual std::shared_ptr<IRun> getRun() const = 0;

		virtual void setLocationOffset(const math::float2& inLocationOffset) = 0;

		virtual math::float2 getLocationOffset() const = 0;

		virtual math::float2 getSize() const = 0;

		virtual LayoutBlockTextContext getTextContext() const = 0;
	};
}