#pragma once

#include <Core/Math/MyMath.h>

namespace GuGu {
	struct KeyEvent;
	class EditorCamera
	{
	public:
		EditorCamera();

		virtual ~EditorCamera();

		bool move(float fElapsedTimeSecond); //移动

		bool zoom(float fElapsedTimeSecond); //聚焦

		bool rotate(float fElapsedTimeSecond); //旋转

		void update(float fElapsedTimeSecond);

		math::affine3 getWorldToViewMatrix();

		float getFov() const;
	private:

		//move
		math::float3 m_moveOffset = math::float3(0, 0, 0);
		float m_moveSpeed = 0.1f;
		float m_smoothMoveSpeed = 5.0f;

		//zoom
		float m_zoomSpeed = 2.0f;

		//camera
		math::float3 m_forward = math::float3(0, 0, -1.0); //摄像机的方向向量 
		math::float3 m_right = math::float3(0, 0, 0); //摄像机空间的x轴方向
		math::float3 m_up = math::float3(0.0, 1.0, 0.0);
		math::float3 m_position = math::float3(0, 0, -10);

		math::float3 m_moveTarget = m_position;

		bool m_bMiddleMouseButtonDown = false;

		float m_yaw, m_pitch;

		float m_fov;
	};
}