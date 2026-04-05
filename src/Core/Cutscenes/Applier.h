#pragma once

namespace GuGu {
    namespace meta{
        class Object;
        class Variant;
    }
	class GuGuUtf8Str;
	class Applier {
	public:
		virtual ~Applier() = default;
		virtual void Apply(std::shared_ptr<meta::Object> inObject, const meta::Variant& value, const GuGuUtf8Str& fieldName) = 0;
	};

	std::shared_ptr<Applier> createApplier(const GuGuUtf8Str& typeGuid);
}