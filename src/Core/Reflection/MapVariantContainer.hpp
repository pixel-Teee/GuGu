namespace GuGu
{
	namespace meta
	{
		template<typename T, typename StorageType>
		MapVariantContainer<T, StorageType>::MapVariantContainer(StorageType& rhs)
			: m_array(rhs)
		{

		}
	}
}
