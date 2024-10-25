#pragma once

//构造一个variant去包裹一个object
#define ObjectVariant(object) GuGu::meta::Variant {object, GuGu::meta::variant_policy::WrapObject()}

namespace GuGu {
	namespace meta {
		class Type;

		class Object
		{
		public:
			virtual ~Object() {}

			virtual Type GetType() const = 0;
			virtual Object* Clone(void) const = 0;

			//todo:add serialize and deserialize
			virtual void OnSerialize(nlohmann::json& output) const {}
			virtual void OnDeserialize(const nlohmann::json& input) {}
		};
	}
}