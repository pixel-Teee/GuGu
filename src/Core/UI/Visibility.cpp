#include <pch.h>

#include "Visibility.h"

namespace GuGu {
	const Visibility Visibility::Visible(Visibility::VIS_Visible);
	const Visibility Visibility::Collapsed(Visibility::VIS_Collapsed);
	const Visibility Visibility::Hidden(Visibility::Hidden);
	const Visibility Visibility::HitTestInvisible(Visibility::HitTestInvisible);
	const Visibility Visibility::SelfHitTestInvisible(Visibility::SelfHitTestInvisible);
	const Visibility Visibility::All(Visibility::VIS_All);
}