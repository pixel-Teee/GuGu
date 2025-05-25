#include <pch.h>

#include "GTexture.h"
#include <Core/Reflection/TypeInfo.h>

namespace GuGu {
	static bool registerGuGuGTexture()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::GTexture");
		auto& type = db.types[id];
		meta::TypeInfo<GTexture>::Register(id, type, true, "026CB8ED-6633-4577-B32C-F3F4D9C2FC22");

		auto typeID = typeidof(GTexture);
		if (typeID != meta::InvalidTypeID && !meta::TypeInfo<GTexture>::Defined)
		{
			auto& type = db.types[typeID];

			//array constructor
			type.SetArrayConstructor<GTexture>();

			type.AddConstructor<GTexture, false, false>({});

			type.AddConstructor<GTexture, true, true>({});

			type.LoadBaseClasses(db, id, { typeof(meta::Object) });

			type.AddField<GTexture, Array<uint8_t>>("m_data",
				(meta::FieldGetter<GTexture, Array<uint8_t>, false>::Signature) & GTexture::m_data,
				(meta::FieldSetter<GTexture, Array<uint8_t>, false>::Signature) & GTexture::m_data, {});
			type.AddField<GTexture, uint32_t>("m_format",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_format,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_format, {});
			type.AddField<GTexture, uint32_t>("m_width",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_width,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_width, {});
			type.AddField<GTexture, uint32_t>("m_height",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_height,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_height, {});
			type.AddField<GTexture, uint32_t>("m_depth",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_depth,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_depth, {});
			type.AddField<GTexture, uint32_t>("m_arraySize",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_arraySize,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_arraySize, {});
			type.AddField<GTexture, uint32_t>("m_dimension",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_dimension,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_dimension, {});
			type.AddField<GTexture, uint32_t>("m_arraySize",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_mipLevels,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_mipLevels, {});
			type.AddField<GTexture, uint32_t>("m_bytesPerPixel",
				(meta::FieldGetter<GTexture, uint32_t, false>::Signature) & GTexture::m_bytesPerPixel,
				(meta::FieldSetter<GTexture, uint32_t, false>::Signature) & GTexture::m_bytesPerPixel, {});

			meta::TypeInfo<GTexture>::Defined = true;
		}

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::GTexture>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<GTexture>>::Register(id, type, false, "7D876BB5-7FE5-4AAE-82B8-0BB529272FD0");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::GTexture>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<GTexture>>::Register(id, type, false, "B73F8372-6560-4AE9-806E-B475B8CD4B09");
		}
		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(GTexture)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuGTexture)
	IMPLEMENT_INITIAL_END
	GTexture::GTexture()
	{
		m_format = static_cast<uint32_t>(nvrhi::Format::UNKNOWN);
		m_width = 1;
		m_height = 1;
		m_depth = 1;
		m_arraySize = 1;
		m_mipLevels = 1;
		m_dimension = static_cast<uint32_t>(nvrhi::TextureDimension::Unknown);
		m_bytesPerPixel = 0;
	}
	GTexture::~GTexture()
	{
	}

	void GTexture::PostLoad()
	{
		this->m_dataLayout.resize(1);
		this->m_dataLayout[0].resize(1);
		this->m_dataLayout[0][0].dataOffset = 0;
		this->m_dataLayout[0][0].rowPitch = static_cast<size_t>(m_width * m_bytesPerPixel);
		this->m_dataLayout[0][0].dataSize = static_cast<size_t>(m_width * m_height * m_bytesPerPixel);
	}

	void GTexture::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type GTexture::GetType() const
	{
		return typeof(GTexture);
	}

	meta::Object* GTexture::Clone(void) const
	{
		GTexture* texture = new GTexture();
		texture->m_format = m_format;
		texture->m_width = m_width;
		texture->m_height = m_height;
		texture->m_depth = m_depth;
		texture->m_arraySize = m_arraySize;
		texture->m_dimension = m_dimension;
		texture->m_mipLevels = m_mipLevels;
		texture->m_data = m_data;
		texture->m_bytesPerPixel = m_bytesPerPixel;
		texture->m_dataLayout = m_dataLayout;
		return texture;
	}

	void GTexture::OnSerialize(nlohmann::json& output) const
	{
		output["type"] = typeidof(GTexture);
		output["format"] = m_format;
		output["width"] = m_width;
		output["height"] = m_height;
		output["depth"] = m_depth;
		output["arraySize"] = m_arraySize;
		output["dimension"] = m_dimension;
		output["mipLevels"] = m_mipLevels;
		output["bytesPerPixel"] = m_bytesPerPixel;
		output["data"] = nlohmann::json::array();
		for (size_t i = 0; i < m_data.size(); ++i)
		{
			output["data"].emplace_back(m_data[i]);
		}
	}

	void GTexture::OnDeserialize(const nlohmann::json& input)
	{
		m_format = input["format"];
		m_width = input["width"];
		m_height = input["height"];
		m_depth = input["depth"];
		m_arraySize = input["arraySize"];
		m_dimension = input["dimension"];
		m_mipLevels = input["mipLevels"];
		m_bytesPerPixel = input["bytesPerPixel"];
		nlohmann::json textureData = input["data"];
		m_data.resize(textureData.size());
		for (int32_t i = 0; i < textureData.size(); ++i)
		{
			m_data.push_back(textureData[i]);
		}
	}

	math::float3 GTexture::getColor(int32_t x, int32_t y) const
	{
		int32_t index = y * m_width + x;
		if (m_bytesPerPixel == 3)
		{
			return math::float3(m_data[index * 3], m_data[index * 3 + 1], m_data[index * 3 + 2]);
		}
		else if (m_bytesPerPixel == 4)
		{
			return math::float3(m_data[index * 4], m_data[index * 4 + 1], m_data[index * 4 + 2]);
		}
		return math::float3(0, 0, 0);
	}

}