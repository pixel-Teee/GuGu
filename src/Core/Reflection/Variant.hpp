namespace GuGu {
	namespace meta {
		template<typename T>
		Variant::Variant(T* data, variant_policy::WrapObject, typename std::enable_if<std::is_base_of<Object, T>::value>::type*)
			: m_isConst(std::is_const<T>::value)

		{

		}

		template<typename T>
		T& Variant::GetValue(void) const
		{
			return *static_cast<T*>(getPtr());
		}
	}
}