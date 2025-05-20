#pragma once

#include "json.hpp" //json

//构造一个variant去包裹一个object
#define ObjectVariant(object) GuGu::meta::Variant {object, GuGu::meta::variant_policy::WrapObject()}

namespace GuGu {
	namespace meta {
		class Type;

		class Object : public std::enable_shared_from_this<Object>
		{
		public:
			virtual ~Object() {}

			virtual void Update(float fElapsedTimeSeconds) = 0;

			virtual Type GetType() const = 0;
			virtual Object* Clone(void) const = 0;

			virtual void PostLoad() = 0;

			//undo/redo 功能
			virtual void modify() {}

			//todo:add serialize and deserialize
			virtual void OnSerialize(nlohmann::json& output) const {}
			virtual void OnDeserialize(const nlohmann::json& input) {}
		};
	}
}