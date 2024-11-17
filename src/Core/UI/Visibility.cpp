#include <pch.h>

#include "Visibility.h"

namespace GuGu {
	const Visibility Visibility::Visible(Visibility::VIS_Visible);
	const Visibility Visibility::Collapsed(Visibility::VIS_Collapsed);
	const Visibility Visibility::Hidden(Visibility::VIS_Hidden);
	const Visibility Visibility::HitTestInvisible(Visibility::VIS_HitTestInvisible);
	const Visibility Visibility::SelfHitTestInvisible(Visibility::VIS_SelfHiTestInvsible);
	const Visibility Visibility::All(Visibility::VIS_All);
}