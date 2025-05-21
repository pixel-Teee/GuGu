#include <pch.h>

#include "Type.h"
#include "ReflectionDatabase.h"

#include "ArrayWrapper.h"
#include "TypeCreator.h"

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

		bool Type::equalGuid(const Type& rhs) const
		{
			return gDatabase.types[m_id].typeGuid == gDatabase.types[rhs.m_id].typeGuid;
		}

		GuGu::GGuid Type::getGuid() const
		{
			return gDatabase.types[m_id].typeGuid;
		}

		const Type& Type::getType(const GGuid& guid)
		{
			auto& guids = gDatabase.guids;
			//return guids[guid];
			if (guids.find(guid) != guids.end())
				return guids[guid];
			return InvalidTypeID;
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
		const Enum& Type::GetEnum(void) const
		{
			return gDatabase.types[m_id].enumeration;
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
		bool Type::IsSharedPtr(void) const
		{
			return gDatabase.types[m_id].isSharedPointer;
		}
		bool Type::IsWeakPtr(void) const
		{
			return gDatabase.types[m_id].isWeakPointer;
		}
		bool Type::IsClass(void) const
		{
			return gDatabase.types[m_id].isClass;
		}

		bool Type::IsStruct(void) const
		{
			return gDatabase.types[m_id].isStruct;
		}

		GuGuUtf8Str Type::GetName() const
		{
			auto& name = gDatabase.types[m_id].name;

			if (IsArray())
				return "Array<" + name + ">";

			return name;
		}

		const meta::MetaManager& Type::GetMeta(void) const
		{
			return gDatabase.types[m_id].meta;
		}

		const Type::Set& Type::GetBaseClasses() const
		{
			return gDatabase.types[m_id].baseClasses;
		}

		const Type::Set& Type::GetDerivedClasses() const
		{
			return gDatabase.types[m_id].derivedClasses;
		}

		Type Type::GetDecayedType(void) const
		{
			GuGu_LOGD("Type::GetDecayedType() not implemented");

			return Type();
		}
		Type Type::GetArrayType(void) const
		{
			return Type(m_id, false);
		}
		const Constructor& Type::GetArrayConstructor() const
		{
			return gDatabase.types[m_id].arrayConstructor;
		}
		std::vector<Constructor> Type::GetConstructos(void) const
		{
			auto& handle = gDatabase.types[m_id].constructors;

			std::vector<Constructor> constructors;

			for (auto& constructor : handle)
				constructors.emplace_back(constructor.second);

			return constructors;
		}
		const Constructor& Type::GetConstructor(const InvokableSignature& signature) const
		{
			return gDatabase.types[m_id].GetConstructor(signature);
		}
		const Constructor& Type::GetDynamicConstructor(const InvokableSignature& signature) const
		{
			return gDatabase.types[m_id].GetDynamicConstructor(signature);
		}
		nlohmann::json Type::SerializeJson(const Variant& instance, bool invokeHook) const
		{
			if (*this != instance.GetType())
				GuGu_LOGE("Serializing incompatible variant instance.\n Got '%s', expected '%s'", instance.GetType().GetName().getStr(),
					GetName().getStr());
			if (m_isArray)
			{
				nlohmann::json array = nlohmann::json::array();

				auto wrapper = instance.GetArray();//从 variant 获取 array, array wrapper
				auto size = wrapper.Size();

				for (size_t i = 0; i < size; ++i)
				{
					auto value = wrapper.GetValue(i);//variant

					array.emplace_back(
						value.GetType().SerializeJson(value, invokeHook)
					);
				}

				return array;
			}

			if (*this == typeof(bool))
			{
				return instance.ToBool();
			}

			//auto& meta = GetMeta();
			auto isEnum = IsEnum();

			//number, or non-associative enum
			if (IsPrimitive()) //todo:add get property serialize as number
			{
				if (IsFloatingPoint() || !IsSigned())
				{
					return instance.ToDouble();
				}

				return instance.ToInt();
			}

			//associative enum value
			if (isEnum)
			{
				return GetEnum().GetKey(instance).getStr();//str to json
			}

			if (*this == typeof(GuGuUtf8Str))
			{
				return instance.ToString().getStr();
			}

			nlohmann::json object{};

			auto& fields = gDatabase.types[m_id].fields;

			for (auto& field : fields)
			{
				auto value = field.GetValue(instance);//variant

				auto json = value.SerializeJson();

				value.m_base->OnSerialize(const_cast<nlohmann::json &>(json));

				object[field.GetName().getStr()] = json;
			}

			if (invokeHook)
				instance.m_base->OnSerialize(object);
			return object;
		}
		Variant Type::DeserializeJson(const nlohmann::json& value) const
		{
			auto& ctor = GetConstructor();//无参构造一个

			return DeserializeJson(value, ctor);
		}
		Variant Type::DeserializeJson(const nlohmann::json& value, const Constructor& ctor) const
		{
			//数组类型需要特殊的情况
			if (IsArray())
			{
				auto nonArrayType = GetArrayType();
				auto arrayCtor = GetArrayConstructor();

				auto instance = arrayCtor.Invoke();
				auto wrapper = instance.GetArray();

				size_t i = 0;
				for (auto& item : value) //遍历json数组
				{
					wrapper.Insert(i++, nonArrayType.DeserializeJson(item, ctor));
				}

				return instance;
			}
			//处理所有原子类型
			else if (IsPrimitive())
			{
				if (*this == typeof(int))
					return { value.get<int>() };
				else if (*this == typeof(unsigned int))
					return { static_cast<unsigned int>(value.get<unsigned int>()) };
				else if (*this == typeof(bool))
					return { value.get<bool>() };
				else if (*this == typeof(float))
					return { static_cast<float>(value.get<float>()) };
				else if (*this == typeof(double))
					return { value.get<double>() };
			}
			else if (IsEnum())
			{
				// number literal
				if (value.is_number())
					return { value.get<int>() };

				// associative value
				auto enumValue = GetEnum().GetValue(value.get<std::string>());

				// make sure we can find the key
				if (enumValue.IsValid())
					return enumValue;

				// use the default value as we couldn't find the key
				return TypeCreator::Create(*this);
			}
			else if (*this == typeof(GuGuUtf8Str))
			{
				return GuGuUtf8Str(value.get<std::string>());
			}

			auto instance = ctor.Invoke();

			DeserializeJson(instance, value); //反序列化普通对象

			return instance;
		}

		void Type::DeserializeJson(Variant& instance, const nlohmann::json& value) const
		{
			auto& fields = gDatabase.types[m_id].fields;

			for (auto& field : fields)
			{
				auto fieldType = field.GetType();

//				assert(fieldType.IsValid(),
//					"Unknown type for field '%s' in base type '%s'. Is this type reflected?",
//					fieldType.GetName().c_str(),
//					GetName().c_str()
//				);

				auto& fieldData = value[field.GetName().getStr()];

				if (!fieldData.is_null())
				{
					auto fieldValue = fieldType.DeserializeJson(fieldData);

					fieldValue.m_base->OnDeserialize(fieldData);

					field.SetValue(instance, fieldValue);
				}
			}

			instance.m_base->OnDeserialize(value);
		}
	}
}


