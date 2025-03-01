#include <pch.h>

#include "ObjectMode.h"

namespace GuGu {

	ObjectMode::ObjectMode(const ObjectModeParams& params)
		: ISceneOutlinerMode(params.m_sceneoutliner)
		, m_bHideComponents(params.m_bHideComponents)
	{

	}

	ObjectMode::~ObjectMode()
	{

	}

}