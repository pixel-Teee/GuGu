#include <pch.h>

#include "UITextManager.h"
#include <fstream>
#include <Core/GamePlay/GameUI/GFont.h>
#include <Core/AssetManager/AssetManager.h>

namespace GuGu {

	UITextManager::UITextManager()
	{

	}

	UITextManager::~UITextManager()
	{

	}

	std::shared_ptr<UITextManager> UITextManager::getUITextManager()
	{
		static std::shared_ptr<UITextManager> uiTextManager = std::make_shared<UITextManager>();
		return uiTextManager;
	}

	nlohmann::json UITextManager::loadFontFile(const GuGuUtf8Str& fontFilePhysicalFilePath)
	{
		std::ifstream in(fontFilePhysicalFilePath.getStr(), std::ios::binary); //todo:fix this, use file system

		std::vector<uint8_t> fileData;
		in.seekg(0, std::ios::end);
		fileData.reserve(in.tellg());
		in.seekg(0, std::ios::beg);
		char ch;
		while (in.get(ch))
		{
			fileData.push_back(ch);
		}

		std::shared_ptr<GFont> gFont = std::make_shared<GFont>();
		gFont->m_data = fileData;

		nlohmann::json fontJson = AssetManager::getAssetManager().serializeJson(gFont);
		return fontJson;
	}

}