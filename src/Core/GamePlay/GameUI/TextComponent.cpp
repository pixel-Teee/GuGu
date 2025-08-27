#include <pch.h>

#include "TextComponent.h"
#include <Core/AssetManager/AssetManager.h>
#include <Core/Texture/GTexture.h>
#include <Core/Reflection/TypeInfo.h>
#include <Core/GamePlay/GameObject.h>
#include <Core/GamePlay/GameUI/GFont.h>
#include "UITransformComponent.h"
#include "UIDrawInfo.h"

namespace GuGu {
	static bool registerGuGuTextComponent()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto id = db.AllocateType("GuGu::TextComponent");
		auto& type = db.types[id];
		meta::TypeInfo<TextComponent>::Register(id, type, true, "1E4E898F-886E-4623-B6DF-BF5911548428");

		auto typeID = typeidof(TextComponent);

		//array constructor
		type.SetArrayConstructor<TextComponent>();

		type.AddConstructor<TextComponent, false, false>({});

		type.AddConstructor<TextComponent, true, true>({});

		type.LoadBaseClasses(db, typeID, { typeof(UIComponent) });

		{
			auto id = db.AllocateType("std::shared_ptr<GuGu::TextComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::shared_ptr<TextComponent>>::Register(id, type, false, "C058CD30-105C-40E4-912B-6C31DC129644");
		}

		{
			auto id = db.AllocateType("std::weak_ptr<GuGu::TextComponent>");
			auto& type = db.types[id];
			meta::TypeInfo<std::weak_ptr<TextComponent>>::Register(id, type, false, "09089768-C7A5-43F7-9600-6473A149D5D0");
		}

		//method
		type.AddMethod("setText", &TextComponent::setText, {});

		return true;
	}

	static bool registerGuGuTextComponentFields()
	{
		auto& db = meta::ReflectionDatabase::Instance();
		auto& type = db.types[typeof(TextComponent).GetID()];

		type.AddField<TextComponent, Color>("m_color",
			(meta::FieldGetter<TextComponent, Color&, true>::Signature) & TextComponent::getColor,
			(meta::FieldSetter<TextComponent, Color,  true>::Signature) & TextComponent::setColor, {});


		type.AddField<TextComponent, std::shared_ptr<AssetData>>("m_fontAsset",
			(meta::FieldGetter<TextComponent, std::shared_ptr<AssetData>, true>::Signature) & TextComponent::getFontAsset,
			(meta::FieldSetter<TextComponent, std::shared_ptr<AssetData>, true>::Signature) & TextComponent::setFontAsset, {});

		type.AddField<TextComponent, float>("m_fontPoint",
			(meta::FieldGetter<TextComponent, float&, true>::Signature) & TextComponent::getFontPoint,
			(meta::FieldSetter<TextComponent, float, true>::Signature) & TextComponent::setFontPoint, {});

		type.AddField<TextComponent, GuGuUtf8Str>("m_text",
			(meta::FieldGetter<TextComponent, GuGuUtf8Str&, true>::Signature) & TextComponent::getText,
			(meta::FieldSetter<TextComponent, GuGuUtf8Str, true>::Signature) & TextComponent::setText, {});

		type.AddField<TextComponent, std::weak_ptr<GameObject>>("m_owner",
			(meta::FieldGetter<TextComponent, std::weak_ptr<GameObject>&, true>::Signature) & TextComponent::getParentGameObject,
			(meta::FieldSetter<TextComponent, std::weak_ptr<GameObject>&, true>::Signature) & TextComponent::setParentGameObject, {});

		type.AddField<TextComponent, math::float2>("m_alignment",
			(meta::FieldGetter<TextComponent, math::float2&, true>::Signature) & TextComponent::getAlignment,
			(meta::FieldSetter<TextComponent, math::float2, true>::Signature) & TextComponent::setAlignment, {});

		return true;
	}

	IMPLEMENT_INITIAL_BEGIN(TextComponent)
		ADD_PRIORITY(UIComponent)
		ADD_INITIAL_FUNCTION_WITH_PRIORITY(registerGuGuTextComponent)
	IMPLEMENT_INITIAL_END

	IMPLEMENT_INITIAL_FIELDS_BEGIN(TextComponent)
		ADD_PRIORITY_FIELDS(AssetData)
		ADD_PRIORITY_FIELDS(Color)
		ADD_PRIORITY_FIELDS(GameObject)
		ADD_INITIAL_FIELDS_FUNCTION_WITH_PRIORITY(registerGuGuTextComponentFields)
	IMPLEMENT_INITIAL_FIELDS_END

	TextComponent::TextComponent()
	{
		m_color = Color(0.5f, 1.0f, 1.0f, 1.0f);
		m_text = GuGuUtf8Str(u8"Love");
		m_fontPoint = 14;

		m_alignment = math::float2(0.5, 0.5);//居中对齐

		GuGuUtf8Str noFileExtensionsFileName = "STKAITI"; //ttf
		GuGuUtf8Str outputFilePath = "content/" + noFileExtensionsFileName + ".json";
		m_fontAsset = AssetManager::getAssetManager().loadAsset(AssetManager::getAssetManager().getGuid(outputFilePath, typeof(GFont)));
	}

	TextComponent::~TextComponent()
	{

	}

	meta::Object* TextComponent::Clone(void) const
	{
		TextComponent* textComponent = new TextComponent();
		textComponent->m_color = m_color;
		return textComponent;
	}

	void TextComponent::Update(float fElapsedTimeSeconds)
	{

	}

	meta::Type TextComponent::GetType() const
	{
		return typeof(TextComponent);
	}

	std::shared_ptr<UIDrawInfo> TextComponent::generateUIDrawInformation(bool bFlip)
	{
		std::shared_ptr<UIDrawInfo> drawInfo = std::make_shared<UIDrawInfo>();
		drawInfo->m_texture = getFont()->m_atlas->m_texture; //font atlas
		
		std::shared_ptr<GFont> font = getFont();

		std::shared_ptr<GameObject> owner = m_owner.lock();
		if (owner)
		{
			//------
			// 0---1
			// |   |
			// 2---3
			//------
			std::shared_ptr<UITransformComponent> uiTransformComponent = owner->getComponent<UITransformComponent>();

			//get global transform
			math::affine3 worldTransform = uiTransformComponent->GetLocalToWorldTransformFloat();

			math::float3 absolutePos;
			math::quat absoluteQuat;
			math::float3 absoluteScale;

			math::decomposeAffine(worldTransform, &absolutePos, &absoluteQuat, &absoluteScale);

			math::float2 localSize = uiTransformComponent->getLocalSize();

			float scale = uiTransformComponent->getScaling().x;

			float maxHeight = font->getFontMaxHeightMetrices(m_fontPoint, scale);

			math::float2 startPos = absolutePos.xy() + math::float2(0, localSize.y);
			uint32_t currenIndex = 0;

			//------为文本对齐提前做宽度的测量------
			std::vector<float> measuredWidth;
			math::float2 startPos2 = absolutePos.xy() + math::float2(0, localSize.y);
			//计算每一行的实际宽度，为后续文本对齐做准备
			for (size_t i = 0; i < m_text.len(); ++i)
			{
				GuGuUtf8Str currentChar = m_text[i];

				//获取度量信息
				GFontCharacterMetrics metrics = font->getFontMetrices(currentChar, m_fontPoint, scale);

				//计算字形位置
				float bearingY = metrics.m_bearing.y;
				float bearingX = metrics.m_bearing.x;
				float advanceX = metrics.m_advance;
				float glyphWidth = metrics.m_size.x;
				float glyphHeight = metrics.m_size.y;

				if ((startPos2.x + bearingX + glyphWidth) < (absolutePos.x + localSize.x))
				{
					startPos2.x = startPos2.x + advanceX;
				}
				else
				{
					measuredWidth.push_back(startPos2.x - absolutePos.x);
					startPos2.x = absolutePos.x;
					startPos2.y = startPos2.y - maxHeight;//todo:add line gap
				}
			}
			measuredWidth.push_back(startPos2.x - absolutePos.x);
			//------为文本对齐提前做宽度的测量------
			
			//generate text
			uint32_t currentMeasureWidthIndex = 0;
			for (size_t i = 0; i < m_text.len(); ++i)
			{
				GuGuUtf8Str currentChar = m_text[i];

				//获取度量信息
				GFontCharacterMetrics metrics = font->getFontMetrices(currentChar, m_fontPoint, scale);
				
				//计算字形位置
				float bearingY = metrics.m_bearing.y;
				float bearingX = metrics.m_bearing.x;
				float advanceX = metrics.m_advance;
				float glyphWidth = metrics.m_size.x;
				float glyphHeight = metrics.m_size.y;

				//uv信息
				math::float2 uvPosition = metrics.m_uvPosition;
				math::float2 uvSize = metrics.m_uvSize;
				float startPosY = startPos.y - (maxHeight - bearingY);

				//文本对齐的x偏移
				float offsetX = (localSize.x - measuredWidth[currentMeasureWidthIndex]) * m_alignment.x;

				if ((startPos.x + bearingX + glyphWidth + offsetX) < (absolutePos.x + localSize.x))
				{
					//vertex generate
					drawInfo->m_uiVertex.push_back(GameUIVertex(uvPosition, math::float3(startPos.x + bearingX + offsetX, startPosY, 0), m_color)); //0
					drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(uvPosition.x + uvSize.x, uvPosition.y), math::float3(startPos.x + bearingX + glyphWidth + offsetX, startPosY, 0), m_color)); //1
					drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(uvPosition.x, uvPosition.y + uvSize.y), math::float3(startPos.x + bearingX + offsetX, startPosY - glyphHeight, 0), m_color)); //2
					drawInfo->m_uiVertex.push_back(GameUIVertex(uvPosition + uvSize, math::float3(startPos.x + bearingX + glyphWidth + offsetX, startPosY - glyphHeight, 0), m_color)); //3

					startPos.x = startPos.x + advanceX;
				}
				else
				{
					currentMeasureWidthIndex = currentMeasureWidthIndex + 1;
					float offsetX = (localSize.x - measuredWidth[currentMeasureWidthIndex]) * m_alignment.x;
					startPos.x = absolutePos.x;
					startPos.y = startPos.y - maxHeight;//todo:add line gap
					startPosY = startPos.y - (maxHeight - bearingY);

					//vertex generate
					drawInfo->m_uiVertex.push_back(GameUIVertex(uvPosition, math::float3(startPos.x + bearingX + offsetX, startPosY, 0), m_color)); //0
					drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(uvPosition.x + uvSize.x, uvPosition.y), math::float3(startPos.x + bearingX + glyphWidth + offsetX, startPosY, 0), m_color)); //1
					drawInfo->m_uiVertex.push_back(GameUIVertex(math::float2(uvPosition.x, uvPosition.y + uvSize.y), math::float3(startPos.x + bearingX + offsetX, startPosY - glyphHeight, 0), m_color)); //2
					drawInfo->m_uiVertex.push_back(GameUIVertex(uvPosition + uvSize, math::float3(startPos.x + bearingX + glyphWidth + offsetX, startPosY - glyphHeight, 0), m_color)); //3

					startPos.x = startPos.x + advanceX;
				}

				//index generate
				drawInfo->m_uiIndices.push_back(currenIndex + 0);
				drawInfo->m_uiIndices.push_back(currenIndex + 2);
				drawInfo->m_uiIndices.push_back(currenIndex + 1);

				drawInfo->m_uiIndices.push_back(currenIndex + 1);
				drawInfo->m_uiIndices.push_back(currenIndex + 2);
				drawInfo->m_uiIndices.push_back(currenIndex + 3);

				currenIndex = currenIndex + 4;
			}

			drawInfo->m_zorder = uiTransformComponent->getZOrder();
			drawInfo->m_drawType = UIDrawType::Font;
		}


		return drawInfo;
	}

	math::float2 TextComponent::getDesiredSize() const
	{
		//nothing to do
		return math::float2(0, 0);
	}

	Color TextComponent::getColor() const
	{
		return m_color;
	}

	Color& TextComponent::getColor()
	{
		return m_color;
	}

	void TextComponent::setColor(Color& inColor)
	{
		m_color = inColor;
	}

	float TextComponent::getFontPoint() const
	{
		return m_fontPoint;
	}

	float& TextComponent::getFontPoint()
	{
		return m_fontPoint;
	}

	void TextComponent::setFontPoint(float inSize)
	{
		m_fontPoint = inSize;
	}

	std::shared_ptr<GuGu::AssetData> TextComponent::getFontAsset() const
	{
		return m_fontAsset;
	}

	void TextComponent::setFontAsset(const std::shared_ptr<AssetData> inAssetData)
	{
		m_fontAsset = inAssetData;
	}

	std::shared_ptr<GFont> TextComponent::getFont() const
	{
		return std::static_pointer_cast<GFont>(m_fontAsset->m_loadedResource);
	}

	GuGuUtf8Str TextComponent::getText() const
	{
		return m_text;
	}

	GuGuUtf8Str& TextComponent::getText()
	{
		return m_text;
	}

	void TextComponent::setText(GuGuUtf8Str inText)
	{
		m_text = inText;
	}

	math::float2 TextComponent::getAlignment() const
	{
		return m_alignment;
	}

	math::float2& TextComponent::getAlignment()
	{
		return m_alignment;
	}

	void TextComponent::setAlignment(math::float2 inAlignment)
	{
		m_alignment = inAlignment;
	}

}