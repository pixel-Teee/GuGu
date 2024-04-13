#pragma once

#include <utility>
#include <functional>
#include <memory>

namespace GuGu {
	template<typename ObjectType>
	class Attribute
	{
	public:
		Attribute()
			: m_value()
			, m_bIsSet(false)
			, m_getter()
		{}

		template<typename OtherType>
		Attribute(const OtherType& inInitialValue)
			: m_value(static_cast<ObjectType>(inInitialValue))
			, m_bIsSet(true)
			, m_getter()
		{}

		Attribute(ObjectType&& inInitialValue)
			: m_value(std::move(inInitialValue))
			, m_bIsSet(false)
			, m_getter()
		{}

		template<typename SourceType>
		Attribute(std::shared_ptr<SourceType> inUserObject,  ObjectType(SourceType::*inConstMethodPtr)()const)
			: m_value()
			, m_bIsSet(true)
			, m_getter(std::bind(inConstMethodPtr, inUserObject.get()))
		{}

		template<class SourceType>
		Attribute(SourceType* inUserObject, void (SourceType::*inConstMethodPtr)()const)
			: m_value()
			, m_bIsSet(true)
			, m_getter(std::bind(inConstMethodPtr, inUserObject))
		{}

		static Attribute Create(const std::function<ObjectType(void)>& inGetter)
		{
			return Attribute(inGetter, true);
		}

		template<typename SourceType, typename SourceTypeOrBase>
		static Attribute CreateSP(const SourceType* inObject, ObjectType(SourceTypeOrBase::*inConstMethodPtr)()const) //weak ptr
		{
			return Create(std::bind(inConstMethodPtr, inObject));
		}

		template<typename LambdaType>
		static Attribute CreateLambda(LambdaType&& inCallable)
		{
			return Create(inCallable);//lambda
		}

		const ObjectType& Get(const ObjectType& defaultValue) const
		{
			return m_bIsSet ? Get() : defaultValue;
		}

		const ObjectType& Get() const
		{
			if (m_getter)
			{
				m_value = m_getter();
			}

			return m_value;
		}

		template<typename OtherType>
		void Set(const OtherType& inNewValue)
		{
			m_getter = nullptr;
			m_value = inNewValue;
			m_bIsSet = true;
		}

		void Set(ObjectType&& inNewValue)
		{
			m_getter = nullptr;
			m_value = std::move(inNewValue);
			m_bIsSet = true;
		}

		bool IsSet() const
		{
			return m_bIsSet;
		}

		bool IsBound() const
		{
			return m_getter.operator bool();
		}

	private:
		Attribute(const std::function<ObjectType(void)>& inGetter, bool bExplicitConstructor)
			: m_value()
			, m_bIsSet(true)
			, m_getter(inGetter)
		{}

		friend class TAttribute;

		mutable ObjectType m_value;

		bool m_bIsSet;

		std::function<ObjectType(void)> m_getter;
	};
}