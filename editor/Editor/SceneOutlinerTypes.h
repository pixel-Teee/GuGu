#pragma once

#include <Core/GuGuUtf8Str.h>

namespace GuGu {
	namespace SceneOutlinerNameSpace
	{
		struct BuiltInColumnTypes
		{
			//gutter column
			static const GuGuUtf8Str& Gutter()
			{
				static GuGuUtf8Str Gutter("Gutter");
				return Gutter;
			}

			//item label column
			static const GuGuUtf8Str& Label()
			{
				static GuGuUtf8Str Label("ItemLabel");
				return Label;
			}

			//generic actor info column
			static const GuGuUtf8Str& ActorInfo()
			{
				static GuGuUtf8Str ActorInfo("ActorInfo");
				return ActorInfo;
			}
		};
	}
}