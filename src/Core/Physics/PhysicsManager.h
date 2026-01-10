#pragma once

#include <btBulletDynamicsCommon.h>

namespace GuGu {
	class PhysicsDebugDraw;
	//physics manager
	class PhysicsManager
	{
	public:
		PhysicsManager();

		~PhysicsManager();

		static PhysicsManager& getPhysicsManager();

		void init();//init physics world

		void destroy();
	private:
		std::shared_ptr<btDefaultCollisionConfiguration> m_collisionConfig;

		std::shared_ptr<btCollisionDispatcher> m_dispatcher;

		std::shared_ptr<btBroadphaseInterface> m_broadphase;

		std::shared_ptr<btSequentialImpulseConstraintSolver> m_solver;

		std::shared_ptr<btDynamicsWorld> m_dynamicsWorld;

		std::shared_ptr<PhysicsDebugDraw> m_debugDraw;
	};
}