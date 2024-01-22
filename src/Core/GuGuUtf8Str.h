#pragma once

#include <string>
#include <vector>

namespace GuGu {
	class GuGuUtf8Str {
	public:
		//GuGuUtf8Str();

		GuGuUtf8Str(const char* str = "");

		GuGuUtf8Str(const GuGuUtf8Str& rhs);

		GuGuUtf8Str& operator=(const GuGuUtf8Str& rhs);

        GuGuUtf8Str(const std::string& str);

		~GuGuUtf8Str();

		int32_t len() const;

		int32_t capacity() const;

		int32_t calculateCharacterByteCount();

		//void swap(GuGuUtf8Str& rhs);

		void reserve(size_t capacity);

		void pushBack(const char* ch); //push back one char

		void append(const char* str);

		void append(const GuGuUtf8Str& s);

		void append(int32_t n, const char* ch);

		//void resize(int32_t n, char* ch = '\0');

		GuGuUtf8Str& operator+=(const char* str);//one character

		GuGuUtf8Str& insert(size_t pos, const char* str);

		GuGuUtf8Str& erase(size_t pos, size_t len);

		size_t find(const char* str, size_t pos = 0) const;

		GuGuUtf8Str substr(size_t pos = 0, size_t len = -1) const;

		GuGuUtf8Str operator[](size_t pos);

		std::wstring getUtf16String() const;//useful for win32 api

		char* getStr();

		const char* getStr() const;

		void clear();

		friend std::ostream& operator<<(std::ostream& out, const GuGuUtf8Str& str);

		//typedef GuGuUtf8Str iterator;
		//typedef const GuGuUtf8Str const_iterator;

		//iterator begin();
		//iterator end();

        bool operator==(const GuGuUtf8Str& rhs) const;
	private:
		char* m_str;
		int32_t m_capacity;
		int32_t m_len;
		int32_t m_totalByteCount;
		std::vector<uint32_t> m_characterByteCount;//every character byte count
	};
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