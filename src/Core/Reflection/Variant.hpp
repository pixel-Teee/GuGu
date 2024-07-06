namespace GuGu {
	namespace meta {
		template<typename T>
		Variant::Variant(T* data, variant_policy::WrapObject, typename std::enable_if<std::is_base_of<Object, T>::value>::type* = nullptr)
			: m_isConst(std::is_const<T>::value)

		{

		}
	}
}