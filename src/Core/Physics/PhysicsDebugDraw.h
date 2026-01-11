#include <LinearMath/btIDebugDraw.h>

namespace GuGu {
	class PhysicsDebugDraw : public btIDebugDraw
	{
	public:
		PhysicsDebugDraw();

		virtual ~PhysicsDebugDraw();

		virtual void reportErrorWarning(const char* warningString) override;

		virtual void draw3dText(const btVector3& location, const char* textString) override;

		virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;

		void setDebugMode(int debugMode) override;

		int getDebugMode() const override;

	private:
		int32_t m_debugMode;
	};
}