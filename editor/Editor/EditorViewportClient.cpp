#include <pch.h>

#include "EditorViewportClient.h"
#include <Application/Application.h>

#include <Core/UI/Events.h>
#include <Core/UI/Viewport.h>
#include <Core/UI/WindowWidget.h>
#include <Core/Timer.h>
#include <Core/Collision/Collision3D.h>
#include <Core/GamePlay/InputManager.h>
#include <Core/GamePlay/World.h>
#include <Core/GamePlay/Level.h>
#include <Core/GamePlay/TransformComponent.h>
#include <Core/Model/GeometryHelper.h>
#include <Core/AssetManager/AssetManager.h>

#ifdef WIN32
#include <Application/Platform/Windows/WindowsMisc.h>
#else
#ifdef ANDROID
#include <Application/Platform/Android/AndroidMisc.h>
#endif
#endif

namespace GuGu {
	EditorViewportClient::EditorViewportClient(std::shared_ptr<ViewportWidget> inViewportWidget)
		: m_viewportWidget(inViewportWidget)
	{
		m_fov = 60.0f;
		m_width = 1280.0f;
		m_height = 920.0f;
		m_yaw = 0.0f;
		m_pitch = 0;
		m_bShowGizmos = false;
		m_viewportState = ViewportClient::Editor;
		updateView();
		makeGizmos();
	}
	EditorViewportClient::~EditorViewportClient()
	{
	}
	void EditorViewportClient::move(float fElapsedTimeSecond)
	{
		m_moveOffset = math::float3(0, 0, 0);

		if (InputManager::getInputManager().isMouseDown(Keys::MiddleMouseButton) && InputManager::getInputManager().isKeyDown(Keys::LeftShift))
		{
			math::float2 mouseDelta = InputManager::getInputManager().getMouseDelta();
			math::float2 speedDelta = moveSpeed();
			m_focalPoint += -getRightDirection() * mouseDelta.x * speedDelta.x * m_distance * fElapsedTimeSecond;
			m_focalPoint += getUpDirection() * mouseDelta.y * speedDelta.y * m_distance * fElapsedTimeSecond;
		}
	}
	void EditorViewportClient::zoom(float fElapsedTimeSecond)
	{
		//m_moveOffset = math::float3(0, 0, 0);
		//
		float wheelDelta = InputManager::getInputManager().getWheelDelta();
		//
		//math::float3 direction = m_forward * wheelDelta;
		//
		//m_moveOffset += direction * m_zoomSpeed;

		if (wheelDelta != 0.0f)
		{
			//GuGu_LOGD("%f", wheelDelta);
			m_distance -= fElapsedTimeSecond * wheelDelta * 10;
			if (m_distance < 0.01f)
			{
				m_focalPoint += getForwardDirection();
				m_distance = 0.01f;
			}
		}
	}
	void EditorViewportClient::rotate(float fElapsedTimeSecond)
	{
		if (InputManager::getInputManager().isMouseDown(Keys::MiddleMouseButton) && !InputManager::getInputManager().isKeyDown(Keys::LeftShift))
		{
			math::float2 delta = InputManager::getInputManager().getMouseDelta();
			float yawSign = getUpDirection().y < 0.0f ? -1.0f : 1.0f;
			m_yaw += yawSign * delta.x * 0.05f * fElapsedTimeSecond;
			m_pitch += delta.y * 0.05f * fElapsedTimeSecond;
		}
	}
	void EditorViewportClient::update(float fElapsedTimeSecond)
	{
		move(fElapsedTimeSecond);
		rotate(fElapsedTimeSecond);
		zoom(fElapsedTimeSecond);
		gizmos(fElapsedTimeSecond);

		//save level
		if (InputManager::getInputManager().isKeyDown(Keys::S) && InputManager::getInputManager().isKeyDown(Keys::LeftControl)) //save level
		{
			InputManager::getInputManager().updateKeyboard(Keys::S, false);//立马释放掉
			GuGuUtf8Str ouputDir = "content";
			GuGuUtf8Str initDir = "";
			std::vector<GuGuUtf8Str> filterArray;
			filterArray.push_back("JSON(*.json)\0");
			filterArray.push_back("*.json\0");
			//initDir = sourcesData.substr(initDir.findFirstOf("/"));
			initDir = AssetManager::getAssetManager().getActualPhysicalPath(initDir);
			GuGuUtf8Str fileName;
			GuGuUtf8Str filePath;

			std::shared_ptr<WindowWidget> rootWindow;
			std::shared_ptr<Widget> widget = m_viewportWidget.lock()->getParentWidget();
			while (widget)
			{
				if (widget->getParentWidget() == nullptr)
				{
					rootWindow = std::static_pointer_cast<WindowWidget>(widget);
					break;
				}
				else
					widget = widget->getParentWidget();
			}

			PlatformMisc::getSaveFilePathAndFileName(rootWindow, initDir, filePath, fileName, filterArray);

			if (fileName != "")
			{
				//get current level
				std::shared_ptr<Level> currentLevel = World::getWorld()->getCurrentLevel();
				nlohmann::json levelJson = AssetManager::getAssetManager().serializeJson(currentLevel);
				GuGuUtf8Str guidStr = GGuid::generateGuid().getGuid();
				levelJson["Version"] = std::to_string(GuGu_Version);
				levelJson["GUID"] = guidStr.getStr();
				GuGuUtf8Str fileContent = levelJson.dump();

				GuGuUtf8Str noFileExtensionsFileName = fileName;
				int32_t dotPos = noFileExtensionsFileName.findLastOf(".");
				if (dotPos != -1)
				{
					noFileExtensionsFileName = noFileExtensionsFileName.substr(0, dotPos);
				}

				//GuGuUtf8Str registerFilePath = filePath;
				//dotPos = filePath.findLastOf(".");
				//if (dotPos != -1)
				//{
				//	registerFilePath = filePath.substr(0, dotPos);
				//}					
				GuGuUtf8Str outputFilePath = ouputDir + "/" + noFileExtensionsFileName + ".json";

				AssetManager::getAssetManager().registerAsset(guidStr, outputFilePath, noFileExtensionsFileName + ".json", meta::Type(meta::TypeIDs<Level>().ID));
				//输出到目录
				AssetManager::getAssetManager().getRootFileSystem()->OpenFile(outputFilePath, GuGuFile::FileMode::OnlyWrite);
				AssetManager::getAssetManager().getRootFileSystem()->WriteFile((void*)fileContent.getStr(), fileContent.getTotalByteCount());
				AssetManager::getAssetManager().getRootFileSystem()->CloseFile();
			}		
		}

		updateView();

		//m_forward = math::normalize(m_forward);
		//m_right = math::normalize(math::cross(m_up, m_forward));
		//m_up = math::normalize(math::cross(m_forward, m_right));

		if (m_viewportWidget.lock())
			m_viewportWidget.lock()->setRenderTarget(m_renderTarget);
	}
	math::float4x4 EditorViewportClient::getWorldToViewMatrix() const
	{
		//math::affine3 worldToView = math::affine3::from_cols(m_right, m_up, m_forward, -m_position);
		//return worldToView;

		math::quat orientation = getOrientation();
		math::affine3 worldToView = orientation.toAffine() * math::translation(m_position);	
		return math::inverse(math::affineToHomogeneous(worldToView));
	}
	math::float4x4 EditorViewportClient::getPespectiveMatrix() const
	{
		math::matrix perspectiveMatrix = math::perspProjD3DStyle(getFov(),
			getAspectRatio(), getNearPlane(), getFarPlane()
		);
		return perspectiveMatrix;
	}
	float EditorViewportClient::getAspectRatio() const
	{
		return (float)m_width / m_height;
	}
	math::float3 EditorViewportClient::getCamPos() const
	{
		return m_position;
	}
	float EditorViewportClient::getFov() const
	{
		return math::radians(m_fov);
	}
	void EditorViewportClient::resizeViewport(int32_t width, int32_t height)
	{
		m_width = width;
		m_height = height;
	}
	math::float2 EditorViewportClient::getViewportSize() const
	{
		return math::float2(m_width, m_height);
	}
	void EditorViewportClient::setRenderTarget(nvrhi::TextureHandle viewportRenderTarget, nvrhi::TextureHandle depthRenderTarget, nvrhi::FramebufferHandle frameBuffer)
	{
		m_renderTarget = viewportRenderTarget;
		m_depthTarget = depthRenderTarget;
		m_frameBuffer = frameBuffer;
	}
	nvrhi::TextureHandle EditorViewportClient::getRenderTarget() const
	{
		return m_renderTarget;
	}
	nvrhi::TextureHandle EditorViewportClient::getDepthTarget() const
	{
		return m_depthTarget;
	}
	nvrhi::FramebufferHandle EditorViewportClient::getFramebuffer() const
	{
		return m_frameBuffer;
	}
	math::float2 EditorViewportClient::getRenderTargetSize() const
	{
		return math::float2(m_renderTarget->getDesc().width, m_renderTarget->getDesc().height);
	}
	float EditorViewportClient::getNearPlane() const
	{
		return m_nearPlane;
	}
	float EditorViewportClient::getFarPlane() const
	{
		return m_farPlane;
	}
	void EditorViewportClient::updateView()
	{
		m_position = calculatePosition();
	}
	math::float3 EditorViewportClient::calculatePosition() const
	{
		return m_focalPoint - getForwardDirection() * m_distance;
	}
	math::float3 EditorViewportClient::getForwardDirection() const
	{
		return math::applyQuat(getOrientation(), math::float3(0.0f, 0.0f, 1.0f));
	}
	math::quat EditorViewportClient::getOrientation() const
	{
		return math::rotationQuat(math::float3(m_pitch, m_yaw, 0.0f));
	}
	math::float3 EditorViewportClient::getRightDirection() const
	{
		return math::applyQuat(getOrientation(), math::float3(1.0f, 0.0f, 0.0f));
	}
	math::float3 EditorViewportClient::getUpDirection() const
	{
		return math::applyQuat(getOrientation(), math::float3(0.0f, 1.0f, 0.0f));
	}
	math::float2 EditorViewportClient::moveSpeed()
	{
		float x = std::min(m_width / 1000.0f, 2.4f);//max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_height / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}
	float EditorViewportClient::zoomSpeed()
	{
		float distance = m_distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f);//max speed = 100
		return speed;
	}
	void EditorViewportClient::gizmos(float fElapsedTimeSecond)
	{
		if (m_bShowGizmos)
		{
			if (InputManager::getInputManager().isKeyDown(Keys::G)) //移动
			{
				m_gizmos = Gizmos::Move;
				m_currentGizmos = m_moveGizmos;
			}
			else if (InputManager::getInputManager().isKeyDown(Keys::R)) //旋转
			{
				m_gizmos = Gizmos::Rotation;
				m_currentGizmos = m_rotateGizmos;
			}
			else if (InputManager::getInputManager().isKeyDown(Keys::S)) //缩放
			{
				m_gizmos = Gizmos::Scale;
				m_currentGizmos = m_scaleGizmos;
			}

			//和gizmos先做碰撞检测
			if (InputManager::getInputManager().isMouseDown(Keys::LeftMouseButton))
			{
				math::float2 mousePosition = InputManager::getInputManager().getMousePosition();
			
				if (!m_bdragging)
				{
					if (m_gizmos == Gizmos::Move)
					{
						math::float3 translation;
						math::float3 scaling;
						math::quat rotation;
						std::shared_ptr<TransformComponent> transform = m_pickedGameObject->getComponent<TransformComponent>();
						translation = math::float3(transform->getTranslation());
						scaling = math::float3(transform->getScaling());
						rotation = math::quat(transform->getRotation());
						math::affine3 noScalingAffine;//gizmos 不需要缩放
						scaling = math::float3(getScreenScaleCompensation(translation)) * 100.0f;//新的缩放，根据屏幕高度来调整
						noScalingAffine = math::scaling(scaling) * rotation.toAffine() * math::translation(translation);

						std::shared_ptr<GStaticMesh> gStaticMesh = Collision3D::pick(mousePosition.x, mousePosition.y, m_width, m_height,
							getPespectiveMatrix(), getWorldToViewMatrix(),
							m_currentGizmos, math::float4x4(math::affineToHomogeneous(noScalingAffine)), m_debugDrawWorldPos);

						if (gStaticMesh) //点击到了
						{
							//判断是选取了哪个轴
							for (uint32_t i = 0; i < m_currentGizmos.size(); ++i)
							{
								if (m_currentGizmos[i] == gStaticMesh)
								{
									m_currentGizmosIndex = i;
									m_currentMousePositionX = mousePosition.x;
									m_currentMousePositionY = mousePosition.y;
									if (m_gizmos == Gizmos::Move)
									{
										math::float3 worldRayPos;
										//math::float3 worldRayDir;
										Collision3D::calculateRayOriginAndRayDir(m_width / 2.0f, m_height / 2.0f, m_width, m_height,
											getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, m_planeNormal);
										//射线和经过物体原点的平面相交
										Collision3D::intersectsWithPlane(worldRayPos, getForwardDirection(), m_planeNormal,
											math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), m_lastRayIntersectPoint);
										m_pickedObjectDragStartWorldPosition = m_pickedGameObject->getComponent<TransformComponent>()->getTranslation();
										if (m_currentGizmosIndex <= 1) //y
										{
											math::quat tmpQuat = math::quat(m_pickedGameObject->getComponent<TransformComponent>()->getRotation());
											m_worldObjectAxis = math::normalize(math::applyQuat(tmpQuat, math::float3(0, 1, 0)));
										}
										else if (m_currentGizmosIndex <= 3) //x
										{
											math::quat tmpQuat = math::quat(m_pickedGameObject->getComponent<TransformComponent>()->getRotation());
											m_worldObjectAxis = math::normalize(math::applyQuat(tmpQuat, math::float3(1, 0, 0)));
										}
										else if (m_currentGizmosIndex <= 5) //z
										{
											math::quat tmpQuat = math::quat(m_pickedGameObject->getComponent<TransformComponent>()->getRotation());
											m_worldObjectAxis = math::normalize(math::applyQuat(tmpQuat, math::float3(0, 0, 1)));
										}
									}
									m_bdragging = true;//开始拖动
									//GuGu_LOGD("trigger dragging");
									break;
								}
							}
						}
					}				
					else if (m_gizmos == Gizmos::Rotation)
					{
						math::float3 translation;
						math::float3 scaling;
						math::quat rotation;
						std::shared_ptr<TransformComponent> transform = m_pickedGameObject->getComponent<TransformComponent>();
						translation = math::float3(transform->getTranslation());
						scaling = math::float3(transform->getScaling());
						rotation = math::quat(transform->getRotation());
						math::affine3 noScalingNoRotationAffine;//gizmos 不需要缩放
						scaling = math::float3(getScreenScaleCompensation(translation)) * 100.0f;//新的缩放，根据屏幕高度来调整
						noScalingNoRotationAffine = math::scaling(scaling)  * math::translation(translation);

						std::shared_ptr<GStaticMesh> gStaticMesh = Collision3D::pick(mousePosition.x, mousePosition.y, m_width, m_height,
							getPespectiveMatrix(), getWorldToViewMatrix(),
							m_currentGizmos, math::float4x4(math::affineToHomogeneous(noScalingNoRotationAffine)), m_debugDrawWorldPos);

						if (gStaticMesh) //点击到了
						{
							//判断是选取了哪个轴
							for (uint32_t i = 0; i < m_currentGizmos.size(); ++i)
							{
								if (m_currentGizmos[i] == gStaticMesh)
								{
									m_currentGizmosIndex = i;
									m_currentMousePositionX = mousePosition.x;
									m_currentMousePositionY = mousePosition.y;

									math::float3 worldRayPos;
									math::float3 worldRayNormal;
									//math::float3 worldRayDir;
									Collision3D::calculateRayOriginAndRayDir(mousePosition.x, mousePosition.y, m_width, m_height,
										getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, worldRayNormal);
									//射线和x环对应的yz平面相交，或者y环对应的xz平面相交，或者z环对应的xy平面
									if (i == 0) //绿色，绕y轴旋转
									{
										Collision3D::intersectsWithPlane(worldRayPos, worldRayNormal, math::float3(0, 1, 0),
											math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), m_lastRayIntersectPoint);
										m_lastTheta = std::atan2f(m_lastRayIntersectPoint.z, m_lastRayIntersectPoint.x);
									}
									else if (i == 1) //红色，绕x轴旋转
									{
										Collision3D::intersectsWithPlane(worldRayPos, worldRayNormal, math::float3(1, 0, 0),
											math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), m_lastRayIntersectPoint);
										m_lastTheta = std::atan2f(m_lastRayIntersectPoint.z, m_lastRayIntersectPoint.y);
									}
									else if (i == 3) //蓝色，绕z轴旋转
									{
										Collision3D::intersectsWithPlane(worldRayPos, worldRayNormal, math::float3(0, 0, 1),
											math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), m_lastRayIntersectPoint);
										m_lastTheta = std::atan2f(m_lastRayIntersectPoint.y, m_lastRayIntersectPoint.x);
									}			
									m_bdragging = true;//开始拖动
									//GuGu_LOGD("trigger dragging");
									break;
								}
							}
						}
					}
					else if (m_gizmos == Gizmos::Scale)
					{
						//scale
						math::float3 translation;
						math::float3 scaling;
						math::quat rotation;

						std::shared_ptr<TransformComponent> transform = m_pickedGameObject->getComponent<TransformComponent>();
						translation = math::float3(transform->getTranslation());
						scaling = math::float3(transform->getScaling());
						rotation = math::quat(transform->getRotation());
						math::affine3 noScalingAffine;//gizmos 不需要缩放
						scaling = math::float3(getScreenScaleCompensation(translation)) * 100.0f;//新的缩放，根据屏幕高度来调整
						noScalingAffine = math::scaling(scaling) * rotation.toAffine() * math::translation(translation);

						std::shared_ptr<GStaticMesh> gStaticMesh = Collision3D::pick(mousePosition.x, mousePosition.y, m_width, m_height,
							getPespectiveMatrix(), getWorldToViewMatrix(),
							m_currentGizmos, math::float4x4(math::affineToHomogeneous(noScalingAffine)), m_debugDrawWorldPos);

						if (gStaticMesh) //点击到了
						{
							//判断是选取了哪个轴
							for (uint32_t i = 0; i < m_currentGizmos.size(); ++i)
							{
								if (m_currentGizmos[i] == gStaticMesh)
								{
									m_currentGizmosIndex = i;
									m_currentMousePositionX = mousePosition.x;
									m_currentMousePositionY = mousePosition.y;
									if (m_gizmos == Gizmos::Scale)
									{
										math::float3 worldRayPos;
										//math::float3 worldRayDir;
										Collision3D::calculateRayOriginAndRayDir(m_width / 2.0f, m_height / 2.0f, m_width, m_height,
											getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, m_planeNormal);
										//射线和经过物体原点的平面相交
										Collision3D::intersectsWithPlane(worldRayPos, getForwardDirection(), m_planeNormal,
											math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), m_lastRayIntersectPoint);
										m_pickedObjectDragStartWorldPosition = m_pickedGameObject->getComponent<TransformComponent>()->getTranslation();
										if (m_currentGizmosIndex <= 1) //y
										{
											math::quat tmpQuat = math::quat(m_pickedGameObject->getComponent<TransformComponent>()->getRotation());
											m_worldObjectAxis = math::normalize(math::applyQuat(tmpQuat, math::float3(0, 1, 0)));
										}
										else if (m_currentGizmosIndex <= 3) //x
										{
											GuGu_LOGD("X Axis");
											math::quat tmpQuat = math::quat(m_pickedGameObject->getComponent<TransformComponent>()->getRotation());
											m_worldObjectAxis = math::normalize(math::applyQuat(tmpQuat, math::float3(1, 0, 0)));
										}
										else if (m_currentGizmosIndex <= 5) //z
										{
											math::quat tmpQuat = math::quat(m_pickedGameObject->getComponent<TransformComponent>()->getRotation());
											m_worldObjectAxis = math::normalize(math::applyQuat(tmpQuat, math::float3(0, 0, 1)));
										}
									}
									m_bdragging = true;//开始拖动
									//GuGu_LOGD("trigger dragging");
									break;
								}
							}
						}
					}
				}	
			}
			else
			{
				m_bdragging = false;//停止拖动
			}
			if (m_bdragging)
			{
				if (m_gizmos == Gizmos::Move)
				{
					math::float2 mousePosition = InputManager::getInputManager().getMousePosition();
					math::float3 worldRayPos;
					math::float3 worldRayDir;
					Collision3D::calculateRayOriginAndRayDir(mousePosition.x, mousePosition.y, m_width, m_height,
						getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, worldRayDir);
					math::float3 intersectPos;
					if (Collision3D::intersectsWithPlane(worldRayPos, worldRayDir, m_planeNormal,
						math::float3(m_pickedObjectDragStartWorldPosition), intersectPos))
					{
						math::float3 delta = intersectPos - m_lastRayIntersectPoint;
						m_lastRayIntersectPoint = intersectPos;
						float projection = math::dot(delta, m_worldObjectAxis);
						//GuGu_LOGD("%f\n", projection);
						math::float3 movement = projection * m_worldObjectAxis * m_distance * fElapsedTimeSecond * 10.0f;
						math::double3 translation = m_pickedGameObject->getComponent<TransformComponent>()->getTranslation();
						translation = translation + math::double3(movement);
						m_pickedGameObject->getComponent<TransformComponent>()->SetTranslation(translation);
					}
				}
				else if (m_gizmos == Gizmos::Rotation)
				{
					math::float2 mousePosition = InputManager::getInputManager().getMousePosition();
					if (m_currentGizmosIndex <= 0) //y
					{
						math::float3 worldRayPos;
						math::float3 worldRayNormal;
						//math::float3 worldRayDir;
						Collision3D::calculateRayOriginAndRayDir(mousePosition.x, mousePosition.y, m_width, m_height,
							getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, worldRayNormal);

						math::float3 intersectPos;
						if (Collision3D::intersectsWithPlane(worldRayPos, worldRayNormal, math::float3(0, 1, 0),
							math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), intersectPos))
						{
							float theta = std::atan2f(intersectPos.z, intersectPos.x);

							float deltaTheta = theta - m_lastTheta;

							m_lastTheta = theta;
							//GuGu_LOGD("变化幅度{%f}", deltaTheta);
							//转换成旋转
							math::dquat rotation = m_pickedGameObject->getComponent<TransformComponent>()->getRotation();
							math::dquat newrotation = rotation * math::rotationQuat(math::double3(0, deltaTheta * fElapsedTimeSecond * 100.0f, 0));//叠加在新的旋转上
							m_pickedGameObject->getComponent<TransformComponent>()->SetRotation(newrotation);
						}
						//if (mouseDelta.y != 0)
						//{
						//	math::dquat rotation = m_pickedGameObject->getComponent<TransformComponent>()->getRotation();
						//	//translation[1] += -mouseDelta.y * fElapsedTimeSecond * 0.2;
						//	math::dquat newrotation = rotation * math::rotationQuat(math::double3(0.0f, mouseDelta.x * fElapsedTimeSecond * 0.2, 0.0f));
						//	m_pickedGameObject->getComponent<TransformComponent>()->SetRotation(newrotation);
						//}
					}
					else if (m_currentGizmosIndex <= 1) //x
					{
						math::float3 worldRayPos;
						math::float3 worldRayNormal;
						//math::float3 worldRayDir;
						Collision3D::calculateRayOriginAndRayDir(mousePosition.x, mousePosition.y, m_width, m_height,
							getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, worldRayNormal);

						math::float3 intersectPos;
						if (Collision3D::intersectsWithPlane(worldRayPos, worldRayNormal, math::float3(1, 0, 0),
							math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), intersectPos))
						{
							float theta = std::atan2f(intersectPos.z, intersectPos.y);

							float deltaTheta = theta - m_lastTheta;
							//GuGu_LOGD("绕x轴旋转{%f}", deltaTheta);
							m_lastTheta = theta;
							//转换成旋转
							math::dquat rotation = m_pickedGameObject->getComponent<TransformComponent>()->getRotation();
							math::dquat newrotation = rotation * math::rotationQuat(math::double3(deltaTheta * fElapsedTimeSecond * 100.0f, 0, 0));//叠加在新的旋转上
							m_pickedGameObject->getComponent<TransformComponent>()->SetRotation(newrotation);
						}
						//if (mouseDelta.x != 0)
						//{
						//	math::dquat rotation = m_pickedGameObject->getComponent<TransformComponent>()->getRotation();
						//	//translation[1] += -mouseDelta.y * fElapsedTimeSecond * 0.2;
						//	math::dquat newrotation = rotation * math::rotationQuat(math::double3(mouseDelta.y * fElapsedTimeSecond * 0.2, 0.0f, 0.0f));
						//	m_pickedGameObject->getComponent<TransformComponent>()->SetRotation(newrotation);
						//}
					}
					else if (m_currentGizmosIndex <= 2) //z
					{
						math::float3 worldRayPos;
						math::float3 worldRayNormal;
						//math::float3 worldRayDir;
						Collision3D::calculateRayOriginAndRayDir(mousePosition.x, mousePosition.y, m_width, m_height,
							getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, worldRayNormal);

						math::float3 intersectPos;
						if (Collision3D::intersectsWithPlane(worldRayPos, worldRayNormal, math::float3(0, 0, 1),
							math::float3(m_pickedGameObject->getComponent<TransformComponent>()->getTranslation()), intersectPos))
						{
							float theta = std::atan2f(intersectPos.y, intersectPos.x);

							float deltaTheta = theta - m_lastTheta;
							//GuGu_LOGD("绕z轴旋转{%f}", deltaTheta);
							m_lastTheta = theta;
							//转换成旋转
							math::dquat rotation = m_pickedGameObject->getComponent<TransformComponent>()->getRotation();
							math::dquat newrotation = rotation * math::rotationQuat(math::double3(0, 0, deltaTheta * fElapsedTimeSecond * 100.0f));//叠加在新的旋转上
							m_pickedGameObject->getComponent<TransformComponent>()->SetRotation(newrotation);
						}
						//if (mouseDelta.x != 0)
						//{
						//	math::dquat rotation = m_pickedGameObject->getComponent<TransformComponent>()->getRotation();
						//	//translation[1] += -mouseDelta.y * fElapsedTimeSecond * 0.2;
						//	math::dquat newrotation = rotation * math::rotationQuat(math::double3(0.0f, 0.0f, mouseDelta.y * fElapsedTimeSecond * 0.2));
						//	m_pickedGameObject->getComponent<TransformComponent>()->SetRotation(newrotation);
						//}
					}
				}
				else if (m_gizmos == Gizmos::Scale)
				{
					math::float2 mousePosition = InputManager::getInputManager().getMousePosition();
					math::float3 worldRayPos;
					math::float3 worldRayDir;
					Collision3D::calculateRayOriginAndRayDir(mousePosition.x, mousePosition.y, m_width, m_height,
						getPespectiveMatrix(), getWorldToViewMatrix(), worldRayPos, worldRayDir);
					math::float3 intersectPos;
					if (Collision3D::intersectsWithPlane(worldRayPos, worldRayDir, m_planeNormal,
						math::float3(m_pickedObjectDragStartWorldPosition), intersectPos))
					{				
						math::float3 delta = intersectPos - m_lastRayIntersectPoint;
						m_lastRayIntersectPoint = intersectPos;
						float projection = math::dot(delta, m_worldObjectAxis);
						
						float scaleFactor = 1.0f + projection * fElapsedTimeSecond * 20.0f;
						math::double3 scaling = m_pickedGameObject->getComponent<TransformComponent>()->getScaling();
						if (m_currentGizmosIndex <= 1)
						{
							//GuGu_LOGD("y axis");
							scaling.y *= scaleFactor;
						}
						else if (m_currentGizmosIndex <= 3)
						{
							//GuGu_LOGD("x axis");
							scaling.x *= scaleFactor;
						}
						else if (m_currentGizmosIndex <= 5)
						{
							//GuGu_LOGD("z axis");
							scaling.z *= scaleFactor;
						}
						m_pickedGameObject->getComponent<TransformComponent>()->SetScaling(scaling);
					}
				}
			}
		}

		if (InputManager::getInputManager().isMouseDown(Keys::LeftMouseButton) && !m_bdragging)//不处于拖动
		{
			math::float2 mousePosition = InputManager::getInputManager().getMousePosition();
			//GuGu_LOGD("%f %f", mousePosition.x, mousePosition.y);
			std::shared_ptr<GameObject> pickedItem = Collision3D::pick(mousePosition.x, mousePosition.y, m_width, m_height, getPespectiveMatrix(), getWorldToViewMatrix(), World::getWorld()->getCurrentLevel()->getGameObjects(), m_debugDrawWorldPos);
			if (pickedItem)
			{
				m_bShowGizmos = true;
				m_pickedGameObject = pickedItem;
			}
			else if(!m_bdragging)
			{
				m_bShowGizmos = false;
				m_pickedGameObject = nullptr;
			}

			//debugPitchAndYaw();
		}

	}
	const std::vector<std::shared_ptr<GStaticMesh>>& EditorViewportClient::getGizmos() const
	{
		if (m_gizmos == Gizmos::Move)
			return m_moveGizmos;
		else if (m_gizmos == Gizmos::Rotation)
			return m_rotateGizmos;
		else if (m_gizmos == Gizmos::Scale)
			return m_scaleGizmos;
		return m_moveGizmos;
	}
	std::vector<std::shared_ptr<GStaticMesh>>& EditorViewportClient::getGizmos()
	{
		if (m_gizmos == Gizmos::Move)
			return m_moveGizmos;
		else if (m_gizmos == Gizmos::Rotation)
			return m_rotateGizmos;
		else if (m_gizmos == Gizmos::Scale)
			return m_scaleGizmos;
		return m_moveGizmos;
	}
	math::float4 EditorViewportClient::getGizmosColor(uint32_t index) const 
	{
		if (m_gizmos == Gizmos::Move)
		{
			if (index <= 1)
				return math::float4(0.0f, 1.0f, 0.0f, 1.0f);
			else if (index <= 3)
				return math::float4(1.0f, 0.0f, 0.0f, 1.0f);
			else
				return math::float4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		else if (m_gizmos == Gizmos::Rotation)
		{
			if (index <= 0)
				return math::float4(0.0f, 1.0f, 0.0f, 1.0f);
			else if (index <= 1)
				return math::float4(1.0f, 0.0f, 0.0f, 1.0f);
			else
				return math::float4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		if (m_gizmos == Gizmos::Scale)
		{
			if (index <= 1)
				return math::float4(0.0f, 1.0f, 0.0f, 1.0f);
			else if (index <= 3)
				return math::float4(1.0f, 0.0f, 0.0f, 1.0f);
			else
				return math::float4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		
		return math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	bool EditorViewportClient::gizmosIsVisible() const
	{
		return m_bShowGizmos;
	}
	std::shared_ptr<GameObject> EditorViewportClient::getSelectedItems() const
	{
		return m_pickedGameObject;
	}
	void EditorViewportClient::debugPitchAndYaw()
	{
		float pitchRad = math::radians(m_pitch);
		float yawRad = math::radians(m_yaw);

		float cosPitch = std::cos(pitchRad);
		float sinPitch = std::sin(pitchRad);
		float cosYaw = std::cos(yawRad);
		float sinYaw = std::sin(yawRad);

		math::float3x3 rotationMatrix = math::float3x3(
			cosYaw, 0, sinYaw,
			sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw,
			-cosPitch * sinYaw, sinPitch, cosPitch * cosYaw
		);

		math::float3 forward = math::float3(0, 0, 1);
		math::float3 right = math::float3(1, 0, 0);
		math::float3 up = math::float3(0, 1, 0);

		forward = math::normalize(forward * rotationMatrix);
		right = math::normalize(right * rotationMatrix);
		up = math::normalize(up * rotationMatrix);

// 		GuGu_LOGD("forward:(%f, %f, %f), right:(%f, %f, %f), up:(%f, %f, %f)", forward.x, forward.y, forward.z,
// 			right.x, right.y, right.z,
// 			up.x, up.y, up.z);
	}

	std::vector<uint32_t> EditorViewportClient::getGizmosRenderSort() const 
{
		if (m_gizmos == Gizmos::Move)
		{
			std::vector<uint32_t> gizmosRenderSort = { 0, 1, 2, 3, 4, 5 };

			std::sort(gizmosRenderSort.begin(), gizmosRenderSort.end(), [&](const uint32_t& lhs, const uint32_t& rhs) {
				math::float3 pos1 = m_moveGizmoPos[lhs];
				math::float3 pos2 = m_moveGizmoPos[rhs];
				math::float3 camPos = getCamPos();
				float distance = (camPos.x - pos1.x) * (camPos.x - pos1.x) + (camPos.y - pos1.y) * (camPos.y - pos1.y) + (camPos.z - pos1.z) * (camPos.z - pos1.z);
				float distance2 = (camPos.x - pos2.x) * (camPos.x - pos2.x) + (camPos.y - pos2.y) * (camPos.y - pos2.y) + (camPos.z - pos2.z) * (camPos.z - pos2.z);
				return distance < distance2;
			});
			return gizmosRenderSort;
		}
		else if (m_gizmos == Gizmos::Rotation)
		{
			return { 0, 1, 2 };//0, 1, 2
		}
		else if (m_gizmos == Gizmos::Scale)
		{
			std::vector<uint32_t> gizmosRenderSort = { 0, 1, 2, 3, 4, 5 };
			std::sort(gizmosRenderSort.begin(), gizmosRenderSort.end(), [&](const uint32_t& lhs, const uint32_t& rhs) {
				math::float3 pos1 = m_scaleGizmoPos[lhs];
				math::float3 pos2 = m_scaleGizmoPos[rhs];
				math::float3 camPos = getCamPos();
				float distance = (camPos.x - pos1.x) * (camPos.x - pos1.x) + (camPos.y - pos1.y) * (camPos.y - pos1.y) + (camPos.z - pos1.z) * (camPos.z - pos1.z);
				float distance2 = (camPos.x - pos2.x) * (camPos.x - pos2.x) + (camPos.y - pos2.y) * (camPos.y - pos2.y) + (camPos.z - pos2.z) * (camPos.z - pos2.z);
				return distance < distance2;
			});
			return gizmosRenderSort;
		}

		return std::vector<uint32_t>();
	}

	ViewportClient::ViewportState EditorViewportClient::getViewportState() const
	{
		return m_viewportState;
	}

	void EditorViewportClient::setViewportState(ViewportState state)
	{
		m_viewportState = state;
		if (state == ViewportClient::Editor)
		{
			World::getWorld()->switchState(World::Runtime);
		}
		else
		{
			World::getWorld()->switchState(World::Editor);
		}
	}

	float EditorViewportClient::getScreenScaleCompensation(math::float3 objWorldPos) const
	{
		float distance = math::dot(objWorldPos - m_position, getForwardDirection());

		return distance * std::tanf(math::radians(m_fov) * 0.5f) / m_height;
	}

	ViewportClient::Gizmos EditorViewportClient::getCurrentGizmosType() const
	{
		return m_gizmos;
	}

	void EditorViewportClient::makeGizmos()
	{
		//绿色
		GStaticMesh gstaticMesh = GeometryHelper::createCylinder(0.2f, 0.05f, 0.2f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.7f, 0.0f))));//轴头
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));//垂直于xy平面
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.6f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.3f, 0.0f))));//圆柱
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		m_moveGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.5f, 0.0f))));
		m_moveGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.2f, 0.0f))));

		//红色
		gstaticMesh = GeometryHelper::createCylinder(0.2f, 0.05f, 0.2f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.7f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));//轴头
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));//垂直于yz平面
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.6f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.3f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));//圆柱
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		m_moveGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.5f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));
		m_moveGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.2f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));

		//蓝色
		gstaticMesh = GeometryHelper::createCylinder(0.2f, 0.05f, 0.2f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.7f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));//轴头
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));//垂直于yz平面
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.6f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.3f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));//圆柱
		m_moveGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		m_moveGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.5f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));
		m_moveGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.2f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));

		//旋转轴
		//绿色，绕y轴旋转
		gstaticMesh = GeometryHelper::createToru(1.0f, 0.015f, 16, 16);
		m_rotateGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));

		//红色，绕x轴旋转
		gstaticMesh = GeometryHelper::createToru(1.0f, 0.015f, 16, 16, math::affineToHomogeneous(math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));
		m_rotateGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));

		//蓝色，绕z轴旋转
		gstaticMesh = GeometryHelper::createToru(1.0f, 0.015f, 16, 16, math::affineToHomogeneous(math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));
		m_rotateGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));

		//绿色，沿着y轴缩放
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.6f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.3f, 0.0f))));//圆柱
		m_scaleGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		gstaticMesh = GeometryHelper::createBox(0.2f, 0.2f, 0.2f, 0, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.7f, 0.0f))));//方块
		m_scaleGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		m_scaleGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.5f, 0.0f))));
		m_scaleGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.2f, 0.0f))));

		//红色，沿着x轴缩放
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.6f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.3f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));//圆柱
		m_scaleGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		gstaticMesh = GeometryHelper::createBox(0.2f, 0.2f, 0.2f, 0, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.7f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));//方块
		m_scaleGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		m_scaleGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.5f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));
		m_scaleGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.2f, 0.0f)) * math::rotation(math::float3(0, 0, 1), math::radians(90.0f))));

		//蓝色，沿着z轴缩放
		gstaticMesh = GeometryHelper::createCylinder(0.015f, 0.015f, 0.6f, 16, 4, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.3f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));//圆柱
		m_scaleGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		gstaticMesh = GeometryHelper::createBox(0.2f, 0.2f, 0.2f, 0, math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.7f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));//方块
		m_scaleGizmos.push_back(std::shared_ptr<GStaticMesh>(static_cast<GStaticMesh*>(gstaticMesh.Clone())));
		m_scaleGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.5f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));
		m_scaleGizmoPos.push_back(math::float4(0.0f, 0.0f, 0.0f, 1.0f) * math::affineToHomogeneous(math::translation(math::float3(0.0f, 0.2f, 0.0f)) * math::rotation(math::float3(1, 0, 0), math::radians(90.0f))));

		m_currentGizmos = m_moveGizmos;
	}
}