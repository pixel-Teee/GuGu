#pragma once

namespace GuGu {
	class UITextManager
	{
	public:
		UITextManager();

		~UITextManager();

		static std::shared_ptr<UITextManager> getUITextManager();

		nlohmann::json loadFontFile(const GuGuUtf8Str& fontFilePhysicalFilePath);
	private:
	};
}