#pragma once

namespace GuGu {
	//------third party------

	static Priority mathfloat2Priority;

	static Priority mathfloat3Priority;

	static Priority mathfloat4Priority;

	static Priority mathdouble3Priority;

	static Priority mathdquatPriority;

	static Priority mathuint16_4Priority;

	static Priority mathrotatorPriority;
	//------third party------

	//------struct------
	static Priority uiPaddingPriority;

	static Priority uiAnchorsPriority;

	static Priority uiAnchorDataPrority;
	//------struct------

	void registerThirdParty();

	void gamePlayerReflectionRegister();
}