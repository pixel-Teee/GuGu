#include <pch.h>

#include "PhysicsManager.h"

#include "PhysicsDebugDraw.h"

namespace GuGu {

	PhysicsManager::PhysicsManager()
	{

	}

	PhysicsManager::~PhysicsManager()
	{

	}

	PhysicsManager& PhysicsManager::getPhysicsManager()
	{
		static PhysicsManager physicsManager;
		return physicsManager;
	}

	void PhysicsManager::init()
	{
		//1.创建碰撞配置(定义碰撞算法堆栈)
		m_collisionConfig = std::make_shared<btDefaultCollisionConfiguration>();

		//2.创建碰撞分发器(处理碰撞检测)
		m_dispatcher = std::make_shared<btCollisionDispatcher>(m_collisionConfig.get());

		//3.粗筛碰撞检测
		m_broadphase = std::make_shared<btDbvtBroadphase>(); //aabb

		//4.约束求解器(处理力和约束)
		m_solver = std::make_shared<btSequentialImpulseConstraintSolver>();

		//5.创建物理世界
		m_dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(
			m_dispatcher.get(),
			m_broadphase.get(),
			m_solver.get(),
			m_collisionConfig.get()
		);

		//6.设置重力
		m_dynamicsWorld->setGravity(btVector3(0, -9.8f, 0));

		m_debugDraw = std::make_shared<PhysicsDebugDraw>();
		m_dynamicsWorld->setDebugDrawer(m_debugDraw.get());

		int32_t currentDebugMode = m_debugDraw->getDebugMode();
		currentDebugMode &= ~btIDebugDraw::DBG_DrawNormals;
		m_debugDraw->setDebugMode(currentDebugMode);
	}

	void PhysicsManager::destroy()
	{
		m_dynamicsWorld = nullptr;
		m_collisionConfig = nullptr;
		m_dispatcher = nullptr;
		m_broadphase = nullptr;
		m_solver = nullptr;
		m_debugDraw = nullptr;
	}

	std::shared_ptr<btDynamicsWorld> PhysicsManager::getDynamicsWorld()
	{
		return m_dynamicsWorld;
	}

	void PhysicsManager::stepSimulation(float deltaTime)
	{
		//(1 / 60秒模拟)
		m_dynamicsWorld->stepSimulation(deltaTime, 10, 1.0f / 60.0f);
	}

}