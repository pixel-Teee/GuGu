#pragma once

namespace GuGu {
	//meta
	class meta::Object;
	class meta::Variant;
	class GuGuUtf8Str;
	class Applier {
	public:
		virtual ~Applier() = default;
		virtual void Apply(std::shared_ptr<meta::Object> inObject, const meta::Variant& value, const GuGuUtf8Str& fieldName) = 0;
	};
}