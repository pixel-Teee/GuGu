namespace GuGu {
	template<typename KeyType, typename ValueType>
	Map<KeyType, ValueType>::Map() 
	{
	}

	template<typename KeyType, typename ValueType>
	Map<KeyType, ValueType>::Map(const std::map<KeyType, ValueType>& rhs)
		: std::map<KeyType, ValueType>(rhs)
	{
	}

	template<typename KeyType, typename ValueType>
	Map<KeyType, ValueType>::Map(const std::map<KeyType, ValueType>&& rhs)
		: std::map<KeyType, ValueType>(std::move(rhs))
	{
	}

}