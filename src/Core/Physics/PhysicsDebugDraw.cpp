#include <pch.h>

#include "PhysicsDebugDraw.h"

#include <Application/Application.h>
#include <Renderer/Renderer.h>
#include <Renderer/Demo.h>

namespace GuGu {

	PhysicsDebugDraw::PhysicsDebugDraw()
	{

	}

	PhysicsDebugDraw::~PhysicsDebugDraw()
	{

	}

	void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		std::shared_ptr<Application> app = Application::getApplication();
		Demo* demo = app->getRenderer()->getDemoPass();
		if (demo)
		{
			demo->addDebugPhysicsInfo(math::float3(from.x(), from.y(), from.z()), math::float3(to.x(), to.y(), to.z()), math::float4(color.x(), color.y(), color.z(), 1.0f));
		}
	}

	void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		
	}

	void PhysicsDebugDraw::setDebugMode(int debugMode)
	{
		m_debugMode = debugMode;
	}

	int PhysicsDebugDraw::getDebugMode() const
	{
		return m_debugMode;
	}

	void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
	{
		
	}

	void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
	{
		
	}

}