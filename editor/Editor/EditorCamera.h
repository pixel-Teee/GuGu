#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct KeyEvent;
	class EditorCamera
	{
	public:
		EditorCamera();

		virtual ~EditorCamera();

		void move(float fElapsedTimeSecond); //移动

		void update(float fElapsedTimeSecond);

		math::affine3 getWorldToViewMatrix();
	private:

		//move
		math::float3 m_moveOffset = math::float3(0, 0, 0);
		float m_moveSpeed = 1.0f;
		float m_smoothMoveSpeed = 5.0f;

		//camera
		math::float3 m_forward = math::float3(0, 0, -1.0); //摄像机的方向向量 
		math::float3 m_right = math::float3(0, 0, 0); //摄像机空间的x轴方向
		math::float3 m_up = math::float3(0.0, 1.0, 0.0);
		math::float3 m_position = math::float3(0, 0, -10);

		math::float3 m_moveTarget = m_position;
	};
}