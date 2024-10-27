#include <pch.h>

#include "TypeData.h"

namespace GuGu {
	namespace meta
	{
		TypeData::TypeData(void)
			: isEnum(false)
			, isPrimitive(false)
			, isPointer(false)
			, isClass(false)
			//todo:添加enumeration的初始化
			, enumeration(nullptr)
		{
		}
		TypeData::TypeData(const GuGuUtf8Str& name)
			: isEnum(false)
			, isPrimitive(false)
			, isPointer(false)
			, isClass(false)
			, name(name)
			, enumeration(nullptr)
			//todo:添加enumeration的初始化
		{
		}

		const Constructor& TypeData::GetConstructor(const InvokableSignature& signature)
		{
			auto search = constructors.find(signature);

			if (search == constructors.end())
				return Constructor::Invalid();

			return search->second;
		}

		const Field& TypeData::GetField(const std::string& name) const
		{
			for (auto& field : fields)
				if (field.GetName() == name)
					return field;

			return Field::Invalid();
		}
	}
}