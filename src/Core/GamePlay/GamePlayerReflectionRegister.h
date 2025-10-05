#pragma once

namespace GuGu {
	//------third party------

	static Priority mathfloat2Priority;

	static Priority mathfloat3Priority;

	static Priority mathfloat4Priority;

	static Priority mathdouble2Priority;

	static Priority mathdouble3Priority;

	static Priority mathdquatPriority;

	static Priority mathuint16_4Priority;

	static Priority mathint16_4Priority;

	static Priority mathrotatorPriority;

	static Priority mathfloat4x4Priority;
	//------third party------

	//------struct------
	static Priority uiPaddingPriority;

	static Priority uiAnchorsPriority;

	static Priority uiAnchorDataPrority;

	static Priority boneInfoPriority;

	static Priority keyPositionPriority;
	
	static Priority keyRotationPriority;
	
	static Priority keyScalePriority;

	static Priority channelPriority;
	//------struct------

	void registerThirdParty();

	void gamePlayerReflectionRegister();
}