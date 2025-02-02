#pragma once

#include "TypeID.h"

#include "TypeConfig.h"
#include "InvokableConfig.h"

#include <vector>

#include <Core/GuGuUtf8Str.h>

#include <json.hpp> //json

namespace GuGu {
	//class GuGuUtf8Str;
	namespace meta
	{
		class Enum;
		class Variant;
		class Constructor;
		class Type
		{
		public:
			typedef std::vector<Type> List;
			typedef std::set<Type> Set;

			Type(void);
			Type(const Type& rhs);
			Type(TypeID id, bool isArray = false);

			operator bool(void) const; //如果是InvalidTypeID，返回false，否则返回true

			//判断m_id是否相等
			bool operator<(const Type& rhs) const;
			bool operator>(const Type& rhs) const;
			bool operator<=(const Type& rhs) const;
			bool operator>=(const Type& rhs) const;
			bool operator==(const Type& rhs) const;
			bool operator!=(const Type& rhs) const;

			//获取一个 invalid type 的实例
			static const Type& Invalid(void);

			//获取类型的内部 id
			TypeID GetID(void) const;

			//获取所有注册在主反射数据库的类型
			static List GetTypes(void);

			//获取一个类型，基于修饰的字符串名
			//参数是类型的名称
			static Type GetFromName(const GuGuUtf8Str& name);

			//通过推导object的类型来获取类型
			template<typename T>
			static Type Get(T&& obj);

			const Enum& GetEnum(void) const;

			bool IsValid(void) const;

			bool IsPrimitive(void) const;

			bool IsFloatingPoint(void) const;//从TypeData查询类型数据

			bool IsSigned(void) const;

			bool IsEnum(void) const;

			bool IsPointer(void) const;

			bool IsSharedPtr(void) const;

			bool IsWeakPtr(void) const;

			bool IsClass(void) const;

			bool IsArray(void) const;

			//获取对于这个类型可读的名称
			GuGuUtf8Str GetName() const;

			const Set& GetBaseClasses() const;

			Type GetDecayedType(void) const;

			Type GetArrayType(void) const;

			const Constructor& GetArrayConstructor() const;

			std::vector<Constructor> GetConstructos(void) const;

			const Constructor& GetConstructor(const InvokableSignature& signature = InvokableSignature()) const;

			const Constructor& GetDynamicConstructor(const InvokableSignature& signature = InvokableSignature()) const;

            nlohmann::json SerializeJson(const Variant& instance, bool invokeHook = true) const;

			template<typename ClassType>
			static nlohmann::json SerializeJson(const ClassType& instance, bool invokeHook = true);

			template<typename ClassType>
			static ClassType DeserializeJson(const nlohmann::json& value);

			Variant DeserializeJson(const nlohmann::json& value) const;
			Variant DeserializeJson(const nlohmann::json& value, const Constructor& ctor) const;
			//Variant DeserializeJson(const nlohmann::json& value, const )
			void DeserializeJson(Variant& instance, const nlohmann::json& value) const;
		private:
			friend struct TypeData;
			//一个无符号整数
			TypeID m_id;
			//是否为数组
			bool m_isArray;
		};
	}
}

#include "Type.hpp"