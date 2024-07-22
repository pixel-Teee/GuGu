#include <pch.h>

#include "Type.h"
#include "ReflectionDatabase.h"

namespace GuGu {
	namespace meta {
		namespace
		{
		//确认我们总是有 gDatabase 的一个引用
#define gDatabase ReflectionDatabase::Instance()
		}
		Type::Type(void)
			: m_id(InvalidTypeID)
			, m_isArray(false) 
		{
		}
		Type::Type(const Type& rhs)
			: m_id(rhs.m_id)
			, m_isArray(rhs.m_isArray)
		{
		}
		Type::Type(TypeID id, bool isArray)
			: m_id(id)
			, m_isArray(isArray)
		{
		}
		Type::operator bool(void) const
		{
			return m_id != InvalidTypeID;
		}
		bool Type::operator<(const Type& rhs) const
		{
			return m_id < rhs.m_id;
		}
		bool Type::operator>(const Type& rhs) const
		{
			return m_id > rhs.m_id;
		}
		bool Type::operator<=(const Type& rhs) const
		{
			return m_id <= rhs.m_id;
		}
		bool Type::operator>=(const Type& rhs) const
		{
			return m_id >= rhs.m_id;
		}
		bool Type::operator==(const Type& rhs) const
		{
			return m_id == rhs.m_id;
		}
		bool Type::operator!=(const Type& rhs) const
		{
			return m_id != rhs.m_id;
		}
		const Type& Type::Invalid(void)
		{
			static const Type invalid{ InvalidTypeID };
			return invalid;
		}
		TypeID Type::GetID(void) const
		{
			return m_id;
		}
		Type::List Type::GetTypes(void)
		{
			auto count = gDatabase.types.size();//vector<TypeData>

			List types;//vector<Type>

			for (TypeID i = 1; i < count; ++i)
			{
				types.emplace_back(i);
			}

			return types;
		}
		Type Type::GetFromName(const GuGuUtf8Str& name)
		{
			auto search = gDatabase.ids.find(name);//name to typeID

			if (search == gDatabase.ids.end())
				return Invalid();

			return { search->second };
		}
		bool Type::IsValid(void) const
		{
			return m_id != InvalidTypeID;
		}
		bool Type::IsArray(void) const
		{
			return m_isArray;
		}
		bool Type::IsPrimitive(void) const
		{
			return gDatabase.types[m_id].isPrimitive;
		}
		bool Type::IsFloatingPoint(void) const
		{
			return gDatabase.types[m_id].isFloatingPoint;
		}
		bool Type::IsSigned(void) const
		{
			return gDatabase.types[m_id].isSigned;
		}
		bool Type::IsEnum(void) const
		{
			return gDatabase.types[m_id].isEnum;
		}
		bool Type::IsPointer(void) const
		{
			return gDatabase.types[m_id].isPointer;
		}
		bool Type::IsClass(void) const
		{
			return gDatabase.types[m_id].isClass;
		}
		GuGuUtf8Str Type::GetName() const
		{
			auto& name = gDatabase.types[m_id].name;

			if (IsArray())
				return "Array<" + name + ">";

			return name;
		}
		Type Type::GetDecayedType(void) const
		{
			GuGu_LOGD("Type::GetDecayedType() not implemented");

			return Type();
		}
		nlohmann::json Type::SerializeJson(const Variant& instance, bool invokeHook) const
		{
			nlohmann::json jsonObject{};
			return jsonObject;
		}
	}
}


