#pragma once

#include "MetaProperty.h"
#include "Type.h"

#include <string>
#include <map>
#include <vector>

namespace GuGu {
	namespace meta {
		class Type;
		class Variant;

		class MetaManager
		{
		public:
			//一个类型到一个具体的 meta property
			//meta manager 构造函数的初始化参数
			typedef std::initializer_list<std::pair<Type, const MetaProperty*>> Initializer;

			typedef std::vector<Variant> PropertyList;

			MetaManager(void);
			MetaManager(const Initializer& properties);//构造函数的初始化参数

			~MetaManager(void);

			template<typename PropertyType>
			const PropertyType* GetProperty(void) const;

			//设置给定的类型
			void SetProperty(Type type, const MetaProperty* value);
		private:

			//enable register disable (white list methods) 等属性
			//一个 meta type 及其对应的创建的对象
			std::map<Type, const MetaProperty*> m_properties;
		};
	}
}

#include "MetaManager.hpp"