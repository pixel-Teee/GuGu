#include <pch.h>

#include "TypeData.h"
#include "ReflectionDatabase.h"

namespace GuGu {
	namespace meta
	{
		TypeData::TypeData(void)
			: isEnum(false)
			, isPrimitive(false)
			, isPointer(false)
			, isClass(false)
			, isSharedPointer(false)
			, isWeakPointer(false)
			//todo:添加enumeration的初始化
			, enumeration(nullptr)
		{
		}
		TypeData::TypeData(const GuGuUtf8Str& name)
			: isEnum(false)
			, isPrimitive(false)
			, isPointer(false)
			, isClass(false)
			, isSharedPointer(false)
			, isWeakPointer(false)
			, name(name)
			, enumeration(nullptr)
			//todo:添加enumeration的初始化
		{
		}

		void TypeData::LoadBaseClasses(ReflectionDatabase& db, TypeID thisType, const std::initializer_list<Type>& classes)
		{
			for (auto& base : classes)
			{
				if(base == Type::Invalid())
					continue;

				baseClasses.insert(base);

				db.types[base.m_id].derivedClasses.insert(thisType);
			}
		}

		const Constructor& TypeData::GetConstructor(const InvokableSignature& signature)
		{
			auto search = constructors.find(signature);

			if (search == constructors.end())
				return Constructor::Invalid();

			return search->second;
		}

		const Constructor& TypeData::GetDynamicConstructor(const InvokableSignature& signature)
		{
			auto search = dynamicConstructors.find(signature);

			if (search == dynamicConstructors.end())
				return Constructor::Invalid();

			return search->second;
		}

		const Method& TypeData::GetMethod(const GuGuUtf8Str& name)
		{
			auto& base = methods[name];

			if (!base.size())
				return Method::Invalid();

			return base.begin()->second;
		}

		const Method& TypeData::GetMethod(const GuGuUtf8Str& name, const InvokableSignature& signature)
		{
			auto& base = methods[name];

			auto search = base.find(signature);

			if (search == base.end())
				return Method::Invalid();

			return search->second;
		}

		const Field& TypeData::GetField(const std::string& name) const
		{
			for (auto& field : fields)
				if (field.GetName() == name)
					return field;
			return Field::Invalid();
		}

		bool TypeData::haveField(const std::string& fieldName, meta::Type fieldType) const
		{
			for (auto& field : fields)
				if (field.GetName() == fieldName && field.GetType() == fieldType)
					return true;
				else
				{
					//recursive
					meta::TypeData typeData = meta::ReflectionDatabase::Instance().types[field.GetType().GetID()];
					if (typeData.haveField(fieldName, fieldType))
						return true;
				}
			return false;
		}

	}
}