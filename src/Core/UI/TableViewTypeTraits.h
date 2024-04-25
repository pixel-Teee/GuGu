#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	template<typename T, typename Enable = void>
	struct IsValidListItem
	{
		enum
		{
			Value = false
		};
	};

	template<typename T>
	struct IsValidListItem<std::shared_ptr<T>>
	{
		enum
		{
			Value = false
		};
	};

	template<typename T>
	struct IsValidListItem<std::weak_ptr<T>>
	{
		enum
		{
			Value = true
		};
	};

	template<>
	struct IsValidListItem<GuGuUtf8Str>
	{
		enum
		{
			Value = true
		};
	};

	template<typename T, typename Enable = void> struct ListTypeTraits
	{
		static_assert(IsValidListItem<T>::Value, "item type t must be std::shared_ptr, std::weak_ptr or GuGuUtf8Str");
	};

	template<typename T> struct ListTypeTraits<std::shared_ptr<T>>
	{
	public:
		using NullableType = std::shared_ptr<T>;

		static bool isPtrValid(const std::shared_ptr<T>& inPtr)
		{
			return inPtr != nullptr; //check valid
		}

		static void resetPtr(std::shared_ptr<T>& inPtr)
		{
			inPtr.reset();
		}

		static std::shared_ptr<T> makeNullPtr()
		{
			return std::shared_ptr<T>(nullptr);
		}

		static std::shared_ptr<T> nullableItemTypeConverToItemType(const std::shared_ptr<T>& inPtr)
		{
			return inPtr;
		}
	};

	template<> struct ListTypeTraits<GuGuUtf8Str>
	{
	public:
		using NullableType = GuGuUtf8Str;

		static bool isPtrValid(const GuGuUtf8Str& inPtr)
		{
			return inPtr != ""; //check valid
		}

		static void resetPtr(GuGuUtf8Str& inPtr)
		{
			inPtr = GuGuUtf8Str();
		}

		static GuGuUtf8Str makeNullPtr()
		{
			return GuGuUtf8Str();
		}

		static GuGuUtf8Str nullableItemTypeConverToItemType(const GuGuUtf8Str& inPtr)
		{
			return inPtr;
		}
	};
}