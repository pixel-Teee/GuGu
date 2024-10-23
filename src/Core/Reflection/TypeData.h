#pragma once

#include "Type.h"
#include "Field.h"
#include "Enum.h"

#include <Core/GuGuUtf8Str.h>

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
			bool isClass : 1; //是否为类

			//todo:添加 meta 管理器

			GuGuUtf8Str name;

			//enum type
			Enum enumeration;

			//class type
			Type::Set baseClasses; //基类
			Type::Set derivedClasses; //派生类

			//构造函数

			//析构函数

			//字段
			std::vector<Field> fields;

			//静态字段

			//方法

			//静态方法

			TypeData(void);
			TypeData(const GuGuUtf8Str& name);

			// Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void),
				void (ClassType::* methodSetter)(SetterArgumentType)
			);

			// Const Method Getter, Method Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType, typename SetterArgumentType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void) const,
				void (ClassType::* methodSetter)(SetterArgumentType)
			);


			// Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void),
				typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter
			);

			// Const Method Getter, Field Setter
			template<typename ClassType, typename FieldType, typename GetterReturnType>
			void AddField(
				const std::string& name,
				GetterReturnType(ClassType::* methodGetter)(void) const,
				typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter
			);

			// Field Getter, Method Setter
			template<typename ClassType, typename FieldType, typename SetterArgumentType>
			void AddField(
				const std::string& name,
				typename FieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
				void (ClassType::* methodSetter)(SetterArgumentType)
			);

			// Field Getter, Field Setter
			template<typename ClassType, typename FieldType>
			void AddField(
				const std::string& name,
				typename FieldGetter<ClassType, FieldType, false>::Signature fieldGetter,
				typename FieldSetter<ClassType, FieldType, false>::Signature fieldSetter
			);

			const Field& GetField(const std::string& name) const;
		};
	}
}

#include "TypeData.hpp"