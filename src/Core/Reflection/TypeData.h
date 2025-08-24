#pragma once

#include "Type.h"
#include "Field.h"
#include "Enum.h"
#include "Constructor.h"
#include "MetaManager.h"
#include "Method.h"
#include "Function.h"

#include <Core/GuGuUtf8Str.h>
#include <Core/Guid.h>

namespace GuGu {
	
	namespace meta
	{
		class ReflectionDatabase;

		struct TypeData
		{
			bool isEnum : 1; //是否为枚举类型
			bool isPrimitive : 1; //是否为原子类型
			bool isSigned : 1; //是否有符号
			bool isFloatingPoint : 1; //是否为浮点数
			bool isPointer : 1; //是否为指针
			bool isSharedPointer : 1;//是否为共享指针
			bool isWeakPointer : 1;//是否为弱指针
			bool isClass : 1; //是否为类
			bool isStruct : 1; //是否为标准布局

			MetaManager meta;

			GuGuUtf8Str name;

			//enum type
			Enum enumeration;

			//class type
			Type::Set baseClasses; //基类
			Type::Set derivedClasses; //派生类

			//构造函数
			Constructor arrayConstructor;

			std::unordered_map<InvokableSignature, Constructor> constructors;

			std::unordered_map<InvokableSignature, Constructor> dynamicConstructors;

			//析构函数

			//字段
			std::vector<Field> fields;

			//静态字段

			//方法
			std::unordered_map<GuGuUtf8Str, InvokableOverloadMap<Method>> methods;

			//静态方法
			std::unordered_map<GuGuUtf8Str, InvokableOverloadMap<Function>> staticMethods;

			//GUID(用于序列化/反序列化唯一ID)
			GuGu::GGuid typeGuid;

			TypeData(void);
			TypeData(const GuGuUtf8Str& name);

			void LoadBaseClasses(ReflectionDatabase& db,
				TypeID thisType,
				const std::initializer_list<Type>& classes);

			template<typename ClassType>
			void SetArrayConstructor(void);

			template<typename ClassType, bool IsDynamic, bool IsWrapped, typename ...Args>
			void AddConstructor(const MetaManager::Initializer& meta);

			const Constructor& GetConstructor(
				const InvokableSignature& signature
			);

			const Constructor& GetDynamicConstructor(
				const InvokableSignature& signature
			);

			// Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void),
				void (ClassType::* methodSetter)(SetterArgumentType),
				const MetaManager::Initializer& meta
			);

			// Const Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void) const,
				void (ClassType::* methodSetter)(SetterArgumentType),
				const MetaManager::Initializer& meta
			);


			// Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void),
				typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter,
				const MetaManager::Initializer& meta
			);

			// Const Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void) const,
				typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter,
				const MetaManager::Initializer& meta
			);

			// Field Getter, Method Setter
			template<typename ClassType, typename FieldType, typename SetterArgumentType>
			void AddField(
				const std::string& name,
				typename FieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
				void (ClassType::* methodSetter)(SetterArgumentType),
				const MetaManager::Initializer& meta
			);

			// Field Getter, Field Setter
			template<typename ClassType, typename FieldType>
			void AddField(
				const std::string& name,
				typename FieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
				typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter,
				const MetaManager::Initializer& meta
			);

			//------method------
			template<typename MethodType>
			void AddMethod(const GuGuUtf8Str& name, MethodType method, const MetaManager::Initializer& meta);

			const Method& GetMethod(const GuGuUtf8Str& name);

			const Method& GetMethod(const GuGuUtf8Str& name, const InvokableSignature& signature);
			//------method------

			//------static method------
			template<
				typename ClassType,
				typename FunctionType
			>
			void AddStaticMethod(
				const GuGuUtf8Str& name,
				FunctionType function,
				const MetaManager::Initializer& meta
			);

			const Function& GetStaticMethod(
				const GuGuUtf8Str& name
			);

			const Function& GetStaticMethod(
				const GuGuUtf8Str& name,
				const InvokableSignature& signature
			);
			//------static method------

			const Field& GetField(const std::string& name) const;

			bool haveField(const std::string& fieldName, meta::Type fieldType) const;
		};
	}
}

#include "TypeData.hpp"