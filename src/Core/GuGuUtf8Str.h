#pragma once

#include <string>
#include <vector>

namespace GuGu {
	class GuGuUtf8Str {
	public:
		GuGuUtf8Str(const char* str = "");

		GuGuUtf8Str(const GuGuUtf8Str& rhs);

		GuGuUtf8Str(const std::string& str);

		GuGuUtf8Str& operator=(const GuGuUtf8Str& rhs); 

		~GuGuUtf8Str();

		int32_t len() const;

		int32_t capacity() const;

		int32_t calculateCharacterByteCount();

		//void swap(GuGuUtf8Str& rhs);

		void reserve(size_t capacity);

		//------append and pushBack------
		GuGuUtf8Str& pushBack(const char* ch); //push back one char

		GuGuUtf8Str& append(const char* str); //push str

		GuGuUtf8Str& append(const GuGuUtf8Str& s);

		GuGuUtf8Str& append(int32_t n, const char* ch);
		//------append and pushBack------

		GuGuUtf8Str& insert(size_t pos, const char* str);

		GuGuUtf8Str& erase(size_t pos, size_t len);

		int32_t find(const char* str, size_t pos = 0) const;

		int32_t findFirstOf(const char* str, int32_t pos = -1) const;

		int32_t findLastOf(const char* str, int32_t pos = -1) const;

		GuGuUtf8Str substr(int32_t pos = 0, int32_t len = -1) const;

		GuGuUtf8Str& operator+=(const GuGuUtf8Str& rhs);

		GuGuUtf8Str& operator+=(const char* str);//multiple character

		friend GuGuUtf8Str operator+(const GuGuUtf8Str& lhs, const GuGuUtf8Str& rhs);

		bool operator==(const GuGuUtf8Str& rhs) const;

		bool operator!=(const GuGuUtf8Str& rhs) const;

		bool operator<(const GuGuUtf8Str& rhs) const;

		GuGuUtf8Str operator[](size_t pos) const;

		std::wstring getUtf16String() const;//useful for win32 api

		static GuGuUtf8Str fromUtf16ToUtf8(const wchar_t* str);

		std::vector<uint32_t> getUnicode() const;

		int32_t getTotalByteCount() const;

		const char* getStr() const;

		void clear();

		int32_t bytePosToCharacterPos(int32_t bytePos) const;

		friend std::ostream& operator<<(std::ostream& out, const GuGuUtf8Str& str);
    
		void stringSplit(const char* split, std::vector<GuGuUtf8Str>& res) const;

		//去除字符串前后的空格
		GuGuUtf8Str trim() const;

		std::vector<uint8_t> getBinary() const;
	private:
		char* m_str;
		int32_t m_capacity;
		int32_t m_len;
		int32_t m_totalByteCount;
		std::vector<uint32_t> m_characterByteCount;//every character byte count
	};


	template<typename... Args>
	static GuGuUtf8Str strFormat(const GuGuUtf8Str& format, Args... args)
	{
		auto sizeBuf = std::snprintf(nullptr, 0, format.getStr(), args...) + 1;
		std::unique_ptr<char[]> buf(new(std::nothrow) char[sizeBuf]);

		if (!buf)
			return GuGuUtf8Str("");

		std::snprintf(buf.get(), sizeBuf, format.getStr(), args...) + 1;
		return GuGuUtf8Str(buf.get());
	}
}

namespace std
{
    template<>
    struct hash<GuGu::GuGuUtf8Str>
    {
        size_t operator()(const GuGu::GuGuUtf8Str& s) const
        {
            size_t val = 0;
            const char* str = s.getStr();
            for(size_t i = 0; str[i]; ++i)
            {
                val ^= static_cast<size_t>(str[i]);
            }
            return val;
        }
    };
}
