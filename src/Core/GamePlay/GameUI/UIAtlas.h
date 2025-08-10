#pragma once

#include <Renderer/nvrhi.h>

namespace GuGu {
	struct UIAtlasCopyRowData
	{
		//source data to copy
		const uint8_t* srcData;
		//place to copy data to
		uint8_t* destData;
		//the row number to copy
		uint32_t srcRow;
		//the row number to copy to
		uint32_t destRow;
		//the width of a source row
		uint32_t rowWidth;
		//the width of the source texture
		uint32_t srcTextureWidth;
		//the width of the dest texture
		uint32_t destTextureWidth;
	};
	struct UIAtlasTextureSlot
	{
		uint32_t x;
		uint32_t y;
		uint32_t width;//width - 2 * padding才是实际的图像数据
		uint32_t height;
		uint32_t padding;
		UIAtlasTextureSlot(uint32_t inX, uint32_t inY, uint32_t inWidth, uint32_t inHeight, uint32_t inPadding)
			: x(inX), y(inY), width(inWidth), height(inHeight), padding(inPadding)
		{}
	};

	class GTexture;
	class UIAtlas :	public meta::Object
	{
	public:
		DECLARE_INITIAL
		DECLARE_INITIAL_FIELDS
		UIAtlas();

		virtual ~UIAtlas();

		virtual void PostLoad() override;

		virtual void Update(float fElapsedTimeSeconds) override;
		virtual meta::Type GetType() const override;
		//拷贝对象
		virtual Object* Clone(void) const override;
		//序列化
		virtual void OnSerialize(nlohmann::json& output) const override;
		//反序列化
		virtual void OnDeserialize(const nlohmann::json& input) override;

		void initializeFontAtlas(int32_t inWidth, int32_t inHeight, nvrhi::Format format);

		bool isNeedToUpdateAtlas();

		std::shared_ptr<UIAtlasTextureSlot> copyTextureToAtlas(int32_t width, int32_t height, const std::vector<uint8_t>& inData);

		void copyDataIntoSlot(std::shared_ptr<UIAtlasTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data);

		void copyRowUtil(const UIAtlasCopyRowData& copyRowData, uint32_t stride);

		//font atlas
		bool m_bHaveFontAtlas;
		Array<uint8_t> m_fontAtlasData;
		uint32_t m_fontAtlasWidth;
		uint32_t m_fontAtlasHeight;
		bool m_bNeedToUpdateAtlas;
		uint32_t m_slotPadding;
		std::list<std::shared_ptr<UIAtlasTextureSlot>> m_textureAtlasEmptySlots;
		std::list<std::shared_ptr<UIAtlasTextureSlot>> m_textureAtlasUsedSlots;
		uint32_t m_stride;

		nvrhi::Format m_fontAtlasFormat;

		//gpu texture
		std::shared_ptr<GTexture> m_texture;

	};
}