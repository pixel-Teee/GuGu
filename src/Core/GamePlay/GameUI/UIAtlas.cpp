#include <pch.h>

#include "UIAtlas.h"
#include <Core/Reflection/TypeInfo.h>
#include <Core/Texture/GTexture.h>

namespace GuGu {
	static bool registerGuGuUIAtlas()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::UIAtlas");
		auto& type = db.types[id];
		meta::TypeInfo<UIAtlas>::Register(id, type, true, "A63E7E4E-62F0-49A9-BA72-E8BD5BB6F6C3");

		auto typeID = typeidof(UIAtlas);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<UIAtlas>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<UIAtlas>();

			type.AddConstructor<UIAtlas, false, false>({});

			type.AddConstructor<UIAtlas, true, true>({});

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });

			meta::TypeInfo<UIAtlas>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::UIAtlas>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<UIAtlas>>::Register(id, type, false, "53C7DA7E-672E-4534-97DF-CB6EA2DB6635");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::UIAtlas>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<UIAtlas>>::Register(id, type, false, "623DE0DD-53DB-419A-8805-4D0FF5C9F2C5");
		}
		return true;
	}
	static bool registerGuGuUIAtlasFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(UIAtlas).GetID()];

		type.AddField<UIAtlas, Array<uint8_t>>("m_fontAtlasData",
			(meta::FieldGetter<UIAtlas, Array<uint8_t>, false>::Signature) & UIAtlas::m_fontAtlasData,
			(meta::FieldSetter<UIAtlas, Array<uint8_t>, false>::Signature) & UIAtlas::m_fontAtlasData, {});
		return true;
	}
	IMPLEMENT_INITIAL_BEGIN(UIAtlas)
		ADD_PRIORITY(Object)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuUIAtlas)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(UIAtlas)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuUIAtlasFields)
	IMPLEMENT_INITIAL_FIELDS_END

	void UIAtlas::copyRowUtil(const UIAtlasCopyRowData& copyRowData, uint32_t stride)
	{
		const uint8_t* data = copyRowData.srcData;
		uint8_t* start = copyRowData.destData;
		const uint32_t sourceWidth = copyRowData.srcTextureWidth;
		const uint32_t destWidth = copyRowData.destTextureWidth;
		const uint32_t srcRow = copyRowData.srcRow;
		const uint32_t destRow = copyRowData.destRow;

		const uint32_t padding = m_slotPadding;
		const uint8_t* sourceDataAddr = &data[srcRow * sourceWidth * stride];
		uint8_t* destDataAddr = &start[(destRow * destWidth + padding) * stride];
		memcpy(destDataAddr, sourceDataAddr, sourceWidth * stride);

		if (padding > 0)
		{
			uint8_t* destPaddingPixelLeft = &start[destRow * destWidth * stride];
			uint8_t* destPaddingPixelRight = destPaddingPixelLeft + ((copyRowData.rowWidth - 1) * stride);

			const uint8_t* firstPixel = sourceDataAddr;
			const uint8_t* lastPixel = sourceDataAddr + ((sourceWidth - 1) * stride);
			memcpy(destPaddingPixelLeft, firstPixel, stride);
			memcpy(destPaddingPixelRight, lastPixel, stride);	
		}
	}

	UIAtlas::UIAtlas()
	{
		m_bNeedToUpdateAtlas = false;
		m_bHaveFontAtlas = false;
		m_slotPadding = 1;//slot padding
	}
	UIAtlas::~UIAtlas()
	{
	}

	void UIAtlas::PostLoad()
	{

	}

	void UIAtlas::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type UIAtlas::GetType() const
	{
		return typeof(UIAtlas);
	}

	meta::Object* UIAtlas::Clone(void) const
	{
		UIAtlas* atlas = new UIAtlas();
		return atlas;
	}

	void UIAtlas::OnSerialize(nlohmann::json& output) const
	{

	}

	void UIAtlas::OnDeserialize(const nlohmann::json& input)
	{

	}

	void UIAtlas::initializeFontAtlas(int32_t inWidth, int32_t inHeight, nvrhi::Format format)
	{
		std::shared_ptr<UIAtlasTextureSlot> rootSlot = std::make_shared<UIAtlasTextureSlot>(0, 0, m_fontAtlasWidth, m_fontAtlasHeight, m_slotPadding);
		m_textureAtlasEmptySlots.push_front(rootSlot);

		m_fontAtlasWidth = inWidth;
		m_fontAtlasHeight = inHeight;
		if (format == nvrhi::Format::RGBA8_UNORM) //[0, 1]
		{
			m_fontAtlasData.resize(inWidth * inHeight * 4 * 4);
			m_fontAtlasData.assign(inWidth * inHeight * 4 * 4, 0);
			m_stride = 4 * 4;
		}
		else if (format == nvrhi::Format::R8_UNORM) //[0, 1]
		{
			m_fontAtlasData.resize(inWidth * inHeight * 4);
			m_fontAtlasData.assign(inWidth * inHeight * 4, 0);
			m_stride = 4;
		}
		m_bNeedToUpdateAtlas = true;
		m_bHaveFontAtlas = true;

		m_texture = std::make_shared<GTexture>();

		m_texture->m_format = (uint32_t)nvrhi::Format::R8_UNORM;
		m_texture->m_width = 1024;
		m_texture->m_height = 1024;
		m_texture->m_bytesPerPixel = 4;
		m_texture->m_dimension = (uint32_t)nvrhi::TextureDimension::Texture2D;
		m_texture->m_mipLevels = 1;
		m_texture->m_data = m_fontAtlasData;

		//data layout
		m_texture->m_dataLayout.resize(1);
		m_texture->m_dataLayout[0].resize(1);
		m_texture->m_dataLayout[0][0].dataOffset = 0;
		m_texture->m_dataLayout[0][0].rowPitch = static_cast<size_t>(1024 * m_texture->m_bytesPerPixel);
		m_texture->m_dataLayout[0][0].dataSize = static_cast<size_t>(m_texture->m_width * m_texture->m_height * m_texture->m_bytesPerPixel);
	}

	bool UIAtlas::isNeedToUpdateAtlas()
	{
		return m_bNeedToUpdateAtlas;
	}

	std::shared_ptr<UIAtlasTextureSlot> UIAtlas::copyTextureToAtlas(int32_t inWidth, int32_t inHeight, const std::vector<uint8_t>& inData)
	{
		const uint32_t width = inWidth;
		const uint32_t height = inHeight;

		const uint32_t totalPadding = m_slotPadding * 2;
		const uint32_t paddedWidth = width + totalPadding;
		const uint32_t paddedHeight = height + totalPadding;

		std::shared_ptr<UIAtlasTextureSlot> findSlot = nullptr;
		for (auto it : m_textureAtlasEmptySlots)
		{
			if (paddedWidth <= it->width && paddedHeight <= it->height)
			{
				findSlot = it;
				break;
			}
		}

		if (findSlot != nullptr)
		{
			const uint32_t remainingWidth = std::max(0u, findSlot->width - paddedWidth);
			const uint32_t remainingHeight = std::max(0u, findSlot->height - paddedHeight);

			const uint32_t minSlotDim = 2;

			if (remainingWidth >= minSlotDim || remainingHeight >= minSlotDim)
			{
				std::shared_ptr<UIAtlasTextureSlot> leftSlot;
				std::shared_ptr<UIAtlasTextureSlot> rightSlot;

				if (remainingHeight <= remainingWidth)
				{
					leftSlot = std::make_shared<UIAtlasTextureSlot>(findSlot->x, findSlot->y + paddedHeight, paddedWidth, remainingHeight, m_slotPadding);
					rightSlot = std::make_shared<UIAtlasTextureSlot>(findSlot->x + paddedWidth, findSlot->y, remainingWidth, findSlot->height, m_slotPadding);
				}
				else
				{
					leftSlot = std::make_shared<UIAtlasTextureSlot>(findSlot->x + paddedWidth, findSlot->y, remainingWidth, paddedHeight, m_slotPadding);
					rightSlot = std::make_shared<UIAtlasTextureSlot>(findSlot->x, findSlot->y + paddedHeight, findSlot->width, remainingHeight, m_slotPadding);
				}

				auto slotIt = std::find(m_textureAtlasEmptySlots.begin(), m_textureAtlasEmptySlots.end(), findSlot);
				m_textureAtlasEmptySlots.erase(slotIt);
				auto slotIt2 = m_textureAtlasEmptySlots.insert(m_textureAtlasEmptySlots.begin(), leftSlot);
				++slotIt2;
				m_textureAtlasEmptySlots.insert(slotIt2, rightSlot);

				m_textureAtlasUsedSlots.push_back(findSlot);
			}
			else
			{
				auto slotIt = std::find(m_textureAtlasEmptySlots.begin(), m_textureAtlasEmptySlots.end(), findSlot);
				m_textureAtlasEmptySlots.erase(slotIt);
				m_textureAtlasUsedSlots.push_back(findSlot);
			}

			findSlot->width = paddedWidth;
			findSlot->height = paddedHeight;
		}
		else
		{
			GuGu_LOGE("could not find slot!");
			return false;
		}

		const std::shared_ptr<UIAtlasTextureSlot> newSlot = findSlot;

		if (newSlot && width > 0 && height > 0)
		{
			//copy data into slot
			copyDataIntoSlot(newSlot, inData);
		}

		m_texture->m_data = m_fontAtlasData;

		//find slot to copy
		m_bNeedToUpdateAtlas = true;

		return newSlot;
	}

	void UIAtlas::copyDataIntoSlot(std::shared_ptr<UIAtlasTextureSlot> slotToCopyTo, const std::vector<uint8_t>& data)
	{
		auto start = m_fontAtlasData.begin() + slotToCopyTo->y * m_fontAtlasWidth * m_stride + slotToCopyTo->x * m_stride;

		const uint32_t padding = m_slotPadding;
		const uint32_t allPadding = padding * 2;

		const uint32_t sourceWidth = slotToCopyTo->width - allPadding;
		const uint32_t sourceHeight = slotToCopyTo->height - allPadding;

		UIAtlasCopyRowData copyRowData;
		copyRowData.destData = &(*start);
		copyRowData.srcData = data.data();
		copyRowData.destTextureWidth = m_fontAtlasWidth;
		copyRowData.srcTextureWidth = sourceWidth;
		copyRowData.rowWidth = slotToCopyTo->width;//在源图像上额外增加了padding

		if (padding > 0)
		{
			//copy first color row into padding
			copyRowData.srcRow = 0;
			copyRowData.destRow = 0;

			copyRowUtil(copyRowData, m_stride);
		}

		//copy each row of the texture
		for (uint32_t row = padding; row < slotToCopyTo->height - padding; ++row)
		{
			copyRowData.srcRow = row - padding;
			copyRowData.destRow = slotToCopyTo->height - padding;

			copyRowUtil(copyRowData, m_stride);
		}
	}

}