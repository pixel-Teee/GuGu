#include "pch.h"
#include "GuGuUtf8Str.h"

namespace GuGu {
	GuGuUtf8Str::GuGuUtf8Str(const char* str) //this contains \0
	{
		m_capacity = strlen(str);
		m_str = new char[m_capacity + 1];
		strcpy(m_str, str);
		m_len = calculateCharacterByteCount();
		m_totalByteCount = m_capacity;
	}
	GuGuUtf8Str::GuGuUtf8Str(const GuGuUtf8Str& rhs)
		: m_str(new char[rhs.m_capacity + 1])
		, m_capacity(rhs.m_capacity)
		, m_len(rhs.m_len)
	{
		strcpy(m_str, rhs.m_str);
		m_characterByteCount = rhs.m_characterByteCount;
		m_totalByteCount = rhs.m_totalByteCount;
	}
	GuGuUtf8Str::GuGuUtf8Str(const std::string& str) {
		m_capacity = str.size();
		m_str = new char[m_capacity + 1];
		strcpy(m_str, str.c_str());
		m_len = calculateCharacterByteCount();
		m_totalByteCount = m_capacity;
	}
	GuGuUtf8Str& GuGuUtf8Str::operator=(const GuGuUtf8Str& rhs)
	{
		if (this == &rhs)
			return *this;

		char* tmp = new char[rhs.m_capacity + 1];
		strcpy(tmp, rhs.m_str);

		delete[] m_str;
		m_str = tmp;
		m_len = rhs.m_len;
		m_capacity = rhs.m_capacity;
		m_characterByteCount = rhs.m_characterByteCount;
		return *this;
	}
	GuGuUtf8Str::~GuGuUtf8Str()
	{
		delete[] m_str;
		m_str = nullptr;
		m_len = m_capacity = 0;
		m_totalByteCount = 0;
	}
	int32_t GuGuUtf8Str::len() const
	{
		return m_len;
	}
	int32_t GuGuUtf8Str::capacity() const
	{
		return m_capacity;
	}
	int32_t GuGuUtf8Str::calculateCharacterByteCount()
	{
		m_characterByteCount.clear();
		m_totalByteCount = 0;
		size_t tempIndex = 0;
		size_t totalLength = 0;
		uint32_t flag = 1;
		uint32_t len = 0;
		while (m_str[tempIndex] != '\0')
		{
			uint8_t character = (uint8_t)m_str[tempIndex];
			if (character <= 0x7f) //ascii
			{
				++totalLength;
				m_characterByteCount.push_back(1);
				++m_totalByteCount;
			}
			else if (character >= 0xC0 && character <= 0xDF) //occupy 2 bytes
			{
				flag = 1;
				len = 2;
			}
			else if (character >= 0xE0 && character <= 0xEF) //occupy 3 bytes
			{
				flag = 2;
				len = 3;
			}
			else if (character >= 0xF0 && character <= 0xF7) //occupy 4 bytes
			{
				flag = 3;
				len = 4;
			}
			else if (flag != 0)
			{
				--flag;
				if (flag == 0)
				{
					++totalLength;
					m_characterByteCount.push_back(len);
					m_totalByteCount += len;
				}
			}
			++tempIndex;
		}
		return totalLength;//don't record '\0'
	}
	void GuGuUtf8Str::reserve(size_t newCapacity)
	{
		if (m_capacity < newCapacity)
		{
			char* tmp = new char[newCapacity + 1];
			strcpy(tmp, m_str);
			delete[] m_str;
			m_str = tmp;
			m_capacity = newCapacity;
			m_len = calculateCharacterByteCount();
			m_totalByteCount = m_capacity;
		}
	}
	GuGuUtf8Str& GuGuUtf8Str::pushBack(const char* ch)
	{
		m_totalByteCount = 0;
		//calculate ch len
		size_t tempIndex = 0;
		uint32_t flag = 1;
		uint32_t len = 0;
		while (ch[tempIndex] != '\0')
		{
			uint8_t character = (uint8_t)ch[tempIndex];
			if (character <= 0x7f) //ascii
			{
				len = 1;
				int32_t totalByteCount = strlen(m_str);
				if (totalByteCount + len > m_capacity)
				{
					reserve(m_capacity == 0 ? 4 : m_capacity * 2);
 					strcpy(m_str + totalByteCount, ch);
					++m_len;
					m_characterByteCount.push_back(len);
					m_str[totalByteCount + len] = '\0';
				}
				else
				{
					strcpy(m_str + totalByteCount, ch);
					++m_len;
					m_characterByteCount.push_back(len);
					m_str[totalByteCount + len] = '\0';
				}
				++m_totalByteCount;
			}
			else if (character >= 0xC0 && character <= 0xDF) //occupy 2 bytes
			{
				flag = 1;
				len = 2;
			}
			else if (character >= 0xE0 && character <= 0xEF) //occupy 3 bytes
			{
				flag = 2;
				len = 3;
			}
			else if (character >= 0xF0 && character <= 0xF7) //occupy 4 bytes
			{
				flag = 3;
				len = 4;
			}
			else if (flag != 0)
			{
				--flag;
				if (flag == 0)
				{
					int32_t totalByteCount = strlen(m_str);
					if (totalByteCount + len > m_capacity)
					{
						reserve(m_capacity == 0 ? 4 : m_capacity * 2);
						strcpy(m_str + totalByteCount, ch);
						++m_len;
						m_characterByteCount.push_back(len);
						m_str[totalByteCount + len] = '\0';
					}
					else
					{
						strcpy(m_str + totalByteCount, ch);
						++m_len;
						m_characterByteCount.push_back(len);
						m_str[totalByteCount + len] = '\0';			
					}
					m_totalByteCount += len;
				}
			}
			++tempIndex;
		}

		return *this;
	}
	GuGuUtf8Str& GuGuUtf8Str::append(const char* str)
	{
		m_totalByteCount = 0;
		size_t tempIndex = 0;
		uint32_t flag = 1;
		uint32_t len = 0;
		while (str[tempIndex] != '\0')
		{
			uint8_t character = (uint8_t)str[tempIndex];
			if (character <= 0x7f) //ascii
			{
				flag = 0;
				len = 1;
				int32_t totalByteCount = strlen(m_str);
				if (totalByteCount + len > m_capacity)
				{
					reserve(m_capacity == 0 ? 4 : m_capacity * 2);
					strncpy(m_str + totalByteCount, str + tempIndex - len + 1, len);
					++m_len;
					m_characterByteCount.push_back(len);
					m_str[totalByteCount + len] = '\0';
				}
				else
				{
					strncpy(m_str + totalByteCount, str + tempIndex - len + 1, len);
					++m_len;
					m_characterByteCount.push_back(len);
					m_str[totalByteCount + len] = '\0';
				}
				++m_totalByteCount;
			}
			else if (character >= 0xC0 && character <= 0xDF) //occupy 2 bytes
			{
				flag = 1;
				len = 2;
			}
			else if (character >= 0xE0 && character <= 0xEF) //occupy 3 bytes
			{
				flag = 2;
				len = 3;
			}
			else if (character >= 0xF0 && character <= 0xF7) //occupy 4 bytes
			{
				flag = 3;
				len = 4;
			}
			else if (flag != 0)
			{
				--flag;
				if (flag == 0)
				{
					int32_t totalByteCount = strlen(m_str);
					if (totalByteCount + len > m_capacity)
					{
						reserve(m_capacity == 0 ? 4 : m_capacity * 2);
						strncpy(m_str + totalByteCount, str + tempIndex - len + 1, len);
						++m_len;
						m_characterByteCount.push_back(len);
						m_str[totalByteCount + len] = '\0';
					}
					else
					{
						strncpy(m_str + totalByteCount, str + tempIndex - len + 1, len);
						++m_len;
						m_characterByteCount.push_back(len);
						m_str[totalByteCount + len] = '\0';
					}
					m_totalByteCount += len;
				}
			}
			++tempIndex;
		}

		return *this;
	}
	GuGuUtf8Str& GuGuUtf8Str::append(const GuGuUtf8Str& s)
	{
		append(s.m_str);
		return *this;
	}
	GuGuUtf8Str& GuGuUtf8Str::append(int32_t n, const char* ch)
	{
		int32_t byteCount = strlen(ch);
		reserve(m_totalByteCount + byteCount);
		for (int32_t i = 0; i < n; ++i)
		{
			pushBack(ch);
		}
		return *this;
	}
	GuGuUtf8Str& GuGuUtf8Str::insert(size_t pos, const char* str)
	{
		assert(pos <= m_len);

		size_t byteCount = strlen(str);
		if (m_totalByteCount + byteCount > m_capacity)
		{
			reserve(m_totalByteCount + byteCount >= m_capacity * 2 ? m_totalByteCount + byteCount : m_capacity * 2);
		}
		size_t end = m_totalByteCount + byteCount;
		size_t insertPosByteCount = 0;
		for (int32_t i = 0; i < pos; ++i)
		{
			insertPosByteCount += m_characterByteCount[i];
		}
		while (end >= insertPosByteCount + byteCount)
		{
			m_str[end] = m_str[end - byteCount];
			--end;
		}
		for (int32_t i = insertPosByteCount, j = 0; j < byteCount; ++i, ++j)
		{
			m_str[i] = str[j];
		}

		calculateCharacterByteCount();
		
		return *this;
	}
	GuGuUtf8Str& GuGuUtf8Str::erase(size_t pos, size_t len)
	{
		size_t end = m_totalByteCount;

		if (len >= m_len - pos) //当len大于剩下空间的时候
		{
			int32_t eraseCount = m_characterByteCount.size() - pos;
			for (int32_t i = 0; i < eraseCount; ++i)
				m_characterByteCount.pop_back();
			m_len = pos;
			m_totalByteCount = 0;
			for (int32_t i = 0; i < m_characterByteCount.size(); ++i)
				m_totalByteCount += m_characterByteCount[i];
			m_str[m_totalByteCount] = '\0';
		}
		else
		{
			int32_t startPos = 0;
			int32_t copyByteStarPos = 0;
			int32_t copyStartLen = pos + len;
			for (int32_t i = 0; i < pos; ++i)
				startPos += m_characterByteCount[i];
			for (int32_t i = 0; i < copyStartLen; ++i)
				copyByteStarPos += m_characterByteCount[i];
			strcpy(m_str + startPos, m_str + copyByteStarPos);
			calculateCharacterByteCount();
		}

		return *this;
	}
	size_t GuGuUtf8Str::find(const char* str, size_t pos) const //from pos to find str
	{
		//bool haveFind = false;
		int32_t substrByteCount = strlen(str);
		
		int32_t startPos = 0;
		for (int32_t i = 0; i < pos; ++i)
			startPos += m_characterByteCount[i];

		for (int32_t i = startPos; i < m_totalByteCount; ++i)
		{
			if (startPos + substrByteCount >= m_totalByteCount)
				return -1;
			bool flag = true;
			int32_t j = 0;
			for (j = i; j < i + substrByteCount; ++j)
			{
				if (m_str[j] != str[j - i])
					flag = false;
			}
			if (flag)
			{
				//haveFind = true;
				
				int32_t tmpByteCount = 0;
				int32_t k;
				for (k = 0; k < m_characterByteCount.size(); ++k)
				{
					if (tmpByteCount == i)
						return k;
					tmpByteCount += m_characterByteCount[k];
				}
				if (tmpByteCount == i)
					return k;
				return -1;
			}
		}
		return -1;
	}
	GuGuUtf8Str GuGuUtf8Str::substr(size_t pos, size_t len) const
	{
		if (pos == m_len) return GuGuUtf8Str();
		else if (pos > m_len) return GuGuUtf8Str();
		else if (len == -1)
		{
			GuGuUtf8Str str;
			
			int32_t startPos = 0;
			for (size_t i = 0; i < pos; ++i)
				startPos += m_characterByteCount[i];
			int32_t copyBytes = 0;
			for (int32_t i = pos; i < m_len; ++i)
				copyBytes += m_characterByteCount[i];
			str.reserve(copyBytes);
			strncpy(str.m_str, m_str + startPos, copyBytes);
			str.m_str[copyBytes] = '\0';
			str.calculateCharacterByteCount();
			str.m_len = m_len - pos;
			return str;
		}
		else
		{
			GuGuUtf8Str str;

			int32_t startPos = 0;
			for (size_t i = 0; i < pos; ++i)
				startPos += m_characterByteCount[i];
			int32_t copyBytes = 0;
			for (int32_t i = pos; i < pos + len; ++i)
				copyBytes += m_characterByteCount[i];
			str.reserve(copyBytes);
			strncpy(str.m_str, m_str + startPos, copyBytes);
			str.m_str[copyBytes] = '\0';
			str.calculateCharacterByteCount();
			str.m_len = len;
			return str;
		}
		return GuGuUtf8Str();
	}
	GuGuUtf8Str& GuGuUtf8Str::operator+=(const GuGuUtf8Str& rhs)
	{
		append(rhs);
		return *this;
	}
	GuGuUtf8Str& GuGuUtf8Str::operator+=(const char* str)
	{
		append(str);
		return *this;
	}
    GuGuUtf8Str operator+(const GuGuUtf8Str &lhs, const GuGuUtf8Str &rhs) {
        GuGuUtf8Str res;
        res.append(lhs);
        res.append(rhs);
        return res;
    }
	bool GuGuUtf8Str::operator==(const GuGuUtf8Str& rhs) const {
		//bool result = true;
		if (m_len != rhs.m_len) return false;
		for (int32_t i = 0; m_str[i] && rhs.m_str[i]; ++i)
		{
			if (m_str[i] != rhs.m_str[i]) return false;
		}
		return true;
	}
	GuGuUtf8Str GuGuUtf8Str::operator[](size_t pos) const
	{
		assert(pos < m_len);
		return substr(pos, 1);
	}
	std::wstring GuGuUtf8Str::getUtf16String() const
	{
		std::wstring res;
		uint32_t currentByteCount = 0;
		//covert utf8 to unicode
		for (int32_t i = 0; i < m_characterByteCount.size(); ++i)
		{
			uint32_t unicode = 0;
			if (m_characterByteCount[i] == 1)
			{
				unicode = m_str[currentByteCount];
			}
			else if (m_characterByteCount[i] == 2)
			{
				unicode = ((m_str[currentByteCount] & 0x1F) << 6) | (m_str[currentByteCount + 1] & 0x3F);
			}
			else if (m_characterByteCount[i] == 3)
			{
				unicode = ((m_str[currentByteCount] & 0x0F) << 12) | ((m_str[currentByteCount + 1] & 0x3F) << 6) | (m_str[currentByteCount + 2] & 0x3F);
			}
			else if (m_characterByteCount[i] == 4)
			{
				unicode = ((m_str[currentByteCount] & 0x07) << 18) | ((m_str[currentByteCount + 1] & 0x3F) << 12) | ((m_str[currentByteCount + 2] & 0x3F) << 6) | (m_str[currentByteCount + 3] & 0x3F);
			}
			currentByteCount += m_characterByteCount[i];

			res.push_back(unicode);
		}

		return res;
	}
	std::vector<uint32_t> GuGuUtf8Str::getUnicode() const
	{
		std::vector<uint32_t> res;
		uint32_t currentByteCount = 0;
		//covert utf8 to unicode
		for (int32_t i = 0; i < m_characterByteCount.size(); ++i)
		{
			uint32_t unicode = 0;
			if (m_characterByteCount[i] == 1)
			{
				unicode = m_str[currentByteCount];
			}
			else if (m_characterByteCount[i] == 2)
			{
				unicode = ((m_str[currentByteCount] & 0x1F) << 6) | (m_str[currentByteCount + 1] & 0x3F);
			}
			else if (m_characterByteCount[i] == 3)
			{
				unicode = ((m_str[currentByteCount] & 0x0F) << 12) | ((m_str[currentByteCount + 1] & 0x3F) << 6) | (m_str[currentByteCount + 2] & 0x3F);
			}
			else if (m_characterByteCount[i] == 4)
			{
				unicode = ((m_str[currentByteCount] & 0x07) << 18) | ((m_str[currentByteCount + 1] & 0x3F) << 12) | ((m_str[currentByteCount + 2] & 0x3F) << 6) | (m_str[currentByteCount + 3] & 0x3F);
			}
			currentByteCount += m_characterByteCount[i];

			res.push_back(unicode);
		}

		return res;
	}
	int32_t GuGuUtf8Str::getTotalByteCount() const
	{
		return m_totalByteCount;
	}
    const char *GuGuUtf8Str::getStr() const {
        return m_str;
    }
	void GuGuUtf8Str::clear()
	{
		m_capacity = 0;
		m_len = 0;
		m_characterByteCount.clear();
		m_totalByteCount = 0;
		m_str[0] = '\0';
	}
	std::ostream& operator<<(std::ostream& out, const GuGuUtf8Str& str)
	{
		for (size_t i = 0; i < str.m_totalByteCount; ++i)
			out << str.m_str[i];
		return out;
	}
}

