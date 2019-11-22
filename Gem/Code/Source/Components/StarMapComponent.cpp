// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Component/TransformBus.h>

#include <LmbrCentral/Scripting/SpawnerComponentBus.h>
#include <LmbrCentral/Rendering/MeshComponentBus.h>

#include <LyShine/Bus/UiSpawnerBus.h>

#include <Components/StarMapComponent.h>

#include <PLY/PLYRequestBus.h>
#include <PLY/PLYTools.h>
#include <PLY/PLYTypes.h>

#include "CMLog.h"

using namespace CaelumMagnaVR;

StarMapComponent::StarMapComponent()
	: m_areaExtentParsecs(16000.0),
	m_viewAreaExtentMetres(500.0),
	m_cubeDensity(4),
	m_maxStarsMovedPerFrame(100),
	m_originOffsetParsecsX(0),
	m_originOffsetParsecsY(0),
	m_originOffsetParsecsZ(0),
	m_initialised(false),
	m_starPoolInitialised(false),
	m_starPoolInitialising(false),
	m_starPoolSize(12000),
	m_moved(false),
	m_lastMovedSeconds(0),
	m_getNewStarsAfterMoveSeconds(2),
	m_starScale(1)
{	

}

StarMapComponent::~StarMapComponent()
{
}

void StarMapComponent::Reflect(AZ::ReflectContext* context)
{
	AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
	if (serialize)
	{
		// Reflect the class fields that you want to serialize.
		// In this example, m_runtimeStateNoSerialize is not reflected for serialization.
		// Base classes with serialized data should be listed as additional template
		// arguments to the Class< T, ... >() function.
		serialize->Class<StarMapComponent, AZ::Component>()
			->Version(1)
			->Field("AreaExtentParsecs", &StarMapComponent::m_areaExtentParsecs)
			->Field("AreaExtentMetres", &StarMapComponent::m_viewAreaExtentMetres)
			->Field("OriginOffsetParsecsX", &StarMapComponent::m_originOffsetParsecsX)
			->Field("OriginOffsetParsecsY", &StarMapComponent::m_originOffsetParsecsY)
			->Field("OriginOffsetParsecsZ", &StarMapComponent::m_originOffsetParsecsZ)
			->Field("CubeDensity", &StarMapComponent::m_cubeDensity)
			->Field("MaxStarsMovedPerFrame", &StarMapComponent::m_maxStarsMovedPerFrame)
			->Field("GetStarsAfterSeconds", &StarMapComponent::m_getNewStarsAfterMoveSeconds)
			->Field("StarPoolSize", &StarMapComponent::m_starPoolSize)
			->Field("StarSize", &StarMapComponent::m_starScale)
			;

		AZ::EditContext* edit = serialize->GetEditContext();
		if (edit)
		{
			edit->Class<StarMapComponent>("Star Map", "Core Star Map component")
				->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_areaExtentParsecs, "Extent pc", "Area extent in parsecs")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_viewAreaExtentMetres, "Extent Game Units", "Extent in game engine units")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_originOffsetParsecsX, "Origin Offset X pc", "Offset X in parsecs")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_originOffsetParsecsY, "Origin Offset Y pc", "Offset Y in parsecs")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_originOffsetParsecsZ, "Origin Offset Z pc", "Offset Z in parsecs")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_cubeDensity, "Cube Density", "Search cube density in one dimension")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_maxStarsMovedPerFrame, "Max Stars Moved Per Frame", "Max stars moved to new location per frame")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_getNewStarsAfterMoveSeconds, "Get Stars After Seconds", "Seconds to wait after last movement to get new stars")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_starPoolSize, "Stars Pool Size", "Stars in prespawned star pool")
				->DataElement(AZ::Edit::UIHandlers::Default, &StarMapComponent::m_starScale, "Stars Scale", "Stars display scale")
				;
		}
	}

	//Expose class methods to Script Canvas via behaviour context.
	AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
	if (behaviorContext)
	{
		behaviorContext->EBus < StarMapComponentRequestBus >("StarMapComponentRequestBus")
			// This is the category that appears in the Node Palette window
			->Attribute(AZ::Script::Attributes::Category, "Miscellaneous")
			->Event("MoveStars", &StarMapComponentRequestBus::Events::MoveStars, 
					{ { { "DeltaTime" , "DeltaTime since last frame" }, 
					{ "Speed", "Scroll speed" },
					{ "Direction", "Movement direction" } } })
			->Event("ZoomStars", &StarMapComponentRequestBus::Events::ZoomStars,
				{ { { "DeltaTime" , "DeltaTime since last frame" },
				{ "Speed", "Zoom speed" } } })
			->Event("ZoomVisArea", &StarMapComponentRequestBus::Events::ZoomVisArea,
				{ { { "DeltaTime" , "DeltaTime since last frame" },
				{ "Speed", "Zoom speed" } } })
			->Event("ChangeStarScale", &StarMapComponentRequestBus::Events::ChangeStarScale,
				{ { { "DeltaTime" , "DeltaTime since last frame" },
				{ "Speed", "Scale change speed" } } })
			->Event("ChangeMaxStarCount", &StarMapComponentRequestBus::Events::ChangeMaxStarCount,
				{ { { "DeltaTime" , "DeltaTime since last frame" },
				{ "Speed", "Star count change speed" } } })
			->Event("Reset", &StarMapComponentRequestBus::Events::Reset)
			->Event("GetViewAreaExtentMetres", &StarMapComponentRequestBus::Events::GetViewAreaExtentMetres,
				{ { { "ViewExtentMetres" , "View extent size in metres" } } })
			->Event("GetAreaExtentParsecs", &StarMapComponentRequestBus::Events::GetAreaExtentParsecs,
				{ { { "AreaExtentParsecs" , "Area extent size in parsecs" } } })
			->Event("GetStarPoolSize", &StarMapComponentRequestBus::Events::GetStarPoolSize,
				{ { { "StarPoolSize" , "Number of stars in star pool" } } })
			->Event("GetParsecCoords", &StarMapComponentRequestBus::Events::GetParsecCoords,
				{ { { "ParsecCoords" , "Parsec coordinates of the middle of the view area" } } })
			->Event("GetSolVisible", &StarMapComponentRequestBus::Events::GetSolVisible,
				{ { { "SolVisible" , "Is Sol visible in the view area?" } } })
			->Event("GetSolViewPosition", &StarMapComponentRequestBus::Events::GetSolViewPosition,
				{ { { "SolViewPosition" , "Position of Sol in the view area" } } })
			->Event("GetStarScale", &StarMapComponentRequestBus::Events::GetStarScale,
				{ { { "StarScale" , "Scale of star objects" } } })
			->Event("GetVRControllerPositionRight", &StarMapComponentRequestBus::Events::GetVRControllerPositionRight,
				{ { { "VRControllerPositionRight" , "Position of right VR controller" } } })
			->Event("GetVRControllerPositionLeft", &StarMapComponentRequestBus::Events::GetVRControllerPositionLeft,
				{ { { "VRControllerPositionLeft" , "Position of left VR controller" } } })
			->Event("GetVRControllerVelocityRight", &StarMapComponentRequestBus::Events::GetVRControllerVelocityRight,
				{ { { "VRControllerVelocityRight" , "Velocity of right VR controller" } } })
			->Event("GetVRControllerVelocityLeft", &StarMapComponentRequestBus::Events::GetVRControllerVelocityLeft,
				{ { { "VRControllerVelocityLeft" , "Velocity of left VR controller" } } })
			->Event("GetVRControllerAccelerationRight", &StarMapComponentRequestBus::Events::GetVRControllerAccelerationRight,
				{ { { "VRControllerAccelerationRight" , "Acceleration of right VR controller" } } })
			->Event("GetVRControllerAccelerationLeft", &StarMapComponentRequestBus::Events::GetVRControllerAccelerationLeft,
				{ { { "VRControllerAccelerationLeft" , "Acceleration of left VR controller" } } })
			;
	}
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GAIAPosToGamePos(const Coord3D &pos) const
{
	double ratio = m_viewAreaExtentMetres / m_areaExtentParsecs;

	double offsetX = m_originOffsetParsecsX * ratio;
	double offsetY = m_originOffsetParsecsY * ratio;
	double offsetZ = m_originOffsetParsecsZ * ratio;

	return AZ::Vector3(static_cast<float>(pos.x * ratio) - static_cast<float>(offsetX),
		static_cast<float>(pos.y * ratio) - static_cast<float>(offsetY),
		static_cast<float>(pos.z * ratio) - static_cast<float>(offsetZ));
}

void CaelumMagnaVR::StarMapComponent::MoveStars(float deltaTime, float speed, AZ::Vector3 direction)
{
	//Normalise star field movement direction, while preserving relative magnitude.
	//This is essantial so the magnitude of movements in a single axis is preserved.
	AZ::Vector3 directionNormalised = AZ::Vector3(1,1,1);
	directionNormalised.Normalize();
	direction = direction * directionNormalised;

	float moveAmount = deltaTime * speed;

	if (m_areaExtentParsecs == 0)
	{
		CMLOG(PLY::Log::PLY_ERROR, "m_areaExtentParsecs is zero in MoveStars.");
		return;
	}

	float ratio = static_cast<float>(m_viewAreaExtentMetres / m_areaExtentParsecs);

	if (ratio == 0)
	{
		CMLOG(PLY::Log::PLY_ERROR, "Move ratio is zero in MoveStars.");
		return;
	}

	AZ::Vector3 scaledDirection = direction * (moveAmount / ratio);

	bool moved = false;

	if (m_originOffsetParsecsX - (double)scaledDirection.GetX() > -8000.0
		&& m_originOffsetParsecsX - (double)scaledDirection.GetX() < 8000.0
		&& m_originOffsetParsecsX - (double)scaledDirection.GetX() != m_originOffsetParsecsX)
	{
		m_originOffsetParsecsX -= scaledDirection.GetX();
		moved = true;
	}

	if (m_originOffsetParsecsY - (double)scaledDirection.GetY() > -8000.0
		&& m_originOffsetParsecsY - (double)scaledDirection.GetY() < 8000.0
		&& m_originOffsetParsecsY - (double)scaledDirection.GetY() != m_originOffsetParsecsY)
	{
		m_originOffsetParsecsY -= scaledDirection.GetY();
		moved = true;
	}
	
	if (m_originOffsetParsecsZ - (double)scaledDirection.GetZ() > -8000.0
		&& m_originOffsetParsecsZ - (double)scaledDirection.GetZ() < 8000.0
		&& m_originOffsetParsecsZ - (double)scaledDirection.GetZ() != m_originOffsetParsecsZ)
	{
		m_originOffsetParsecsZ -= scaledDirection.GetZ();
		moved = true;
	};

	if (!moved) return;

	for (auto &e : m_starPool)
	{
		//Get entity current translation.
		AZ::Vector3 t;
		AZ::TransformBus::EventResult(t, e, &AZ::TransformBus::Events::GetLocalTranslation);

		//Move star position.
		t = t + (direction * moveAmount);
		RedrawStarAt(e, t);
	}

	m_moved = true;
	m_lastMovedSeconds = 0;

}

void CaelumMagnaVR::StarMapComponent::ZoomStars(float deltaTime, float speed)
{
	m_moved = true;
	m_lastMovedSeconds = 0;

	float zoomAmount = deltaTime * speed;

	if (m_areaExtentParsecs == 0)
	{
		CMLOG(PLY::Log::PLY_ERROR, "m_areaExtentParsecs is zero in ZoomStars.");
		return;
	}

	float ratio = static_cast<float>(m_viewAreaExtentMetres / m_areaExtentParsecs);

	if (ratio == 0)
	{
		CMLOG(PLY::Log::PLY_ERROR, "Zoom ratio is zero in ZoomStars.");
		return;
	}

	zoomAmount = zoomAmount / ratio;

	double oldAreaExtentParsecs = m_areaExtentParsecs;
	double newAreaExtentParsecs = m_areaExtentParsecs + zoomAmount;

	//Impose sensible limits.
	if (newAreaExtentParsecs < 1.0) newAreaExtentParsecs = 1.0;
	if (newAreaExtentParsecs > 20000.0) newAreaExtentParsecs = 20000.0;

	double newRatio = oldAreaExtentParsecs / newAreaExtentParsecs;

	for (auto &e : m_starPool)
	{
		//Get entity current translation.
		AZ::Vector3 t;
		AZ::TransformBus::EventResult(t, e, &AZ::TransformBus::Events::GetLocalTranslation);

		//Set star position.
		t = t * (float)newRatio;
		RedrawStarAt(e, t);
	}

	m_areaExtentParsecs = newAreaExtentParsecs;
}

void CaelumMagnaVR::StarMapComponent::ZoomVisArea(float deltaTime, float speed)
{
	float zoomAmount = deltaTime * speed;

	double oldViewAreaExtentMetres = m_viewAreaExtentMetres;
	double newViewAreaExtentMetres = m_viewAreaExtentMetres + zoomAmount;

	//Impose sensible limits.
	if (newViewAreaExtentMetres < 0.5) newViewAreaExtentMetres = 0.5;
	if (newViewAreaExtentMetres > 5.0) newViewAreaExtentMetres = 5.0;

	double newRatio = oldViewAreaExtentMetres / newViewAreaExtentMetres;

	m_viewAreaExtentMetres = newViewAreaExtentMetres;

	for (auto &e : m_starPool)
	{
		//Get entity current translation.
		AZ::Vector3 t;
		AZ::TransformBus::EventResult(t, e, &AZ::TransformBus::Events::GetLocalTranslation);

		//Set star position.
		t = t / (float)newRatio;
		RedrawStarAt(e, t);
	}
}

void CaelumMagnaVR::StarMapComponent::ChangeStarScale(float deltaTime, float speed)
{
	float amount = deltaTime * speed;
	m_starScale += amount;

	if (m_starScale < 0.01f) m_starScale = 0.01f;
	if (m_starScale > 10.0f) m_starScale = 10.0f;

	for (auto &e : m_starPool)
	{
		AZ::TransformBus::Event(e, &AZ::TransformBus::Events::SetLocalScale, AZ::Vector3(m_starScale, m_starScale, m_starScale));
	}
}

void CaelumMagnaVR::StarMapComponent::ChangeMaxStarCount(float deltaTime, float speed)
{
	float amount = deltaTime * speed;
	if (amount == 0) return;

	int oldStarPoolSize = m_starPoolSize;

	m_starPoolSize_UI_slidervalue += amount;

	//Impose limits.
	if (m_starPoolSize_UI_slidervalue < 1.0f) m_starPoolSize_UI_slidervalue = 1.0f;
	if (m_starPoolSize_UI_slidervalue > 24000.0f) m_starPoolSize_UI_slidervalue = 24000.0f;

	if (m_starPoolSize_UI_slidervalue == m_starPoolSize) return;

	m_starPoolSize = static_cast<int>(round(m_starPoolSize_UI_slidervalue));

	int difference = abs(m_starPoolSize - oldStarPoolSize);

	if (amount < 0)
	{
		RemoveFromStarPool(difference);
		m_lastMovedSeconds = 0;
	}
	else
	{
		m_moved = true;
		m_lastMovedSeconds = 0;
	}
}

void CaelumMagnaVR::StarMapComponent::Reset()
{
	m_originOffsetParsecsX = m_original_originOffsetParsecsX;
	m_originOffsetParsecsY = m_original_originOffsetParsecsY;
	m_originOffsetParsecsZ = m_original_originOffsetParsecsZ;
	m_areaExtentParsecs = m_original_areaExtentParsecs;
	m_viewAreaExtentMetres = m_original_viewAreaExtentMetres;
	
	m_starScale = m_orginial_starScale;
	for (auto &e : m_starPool)
	{
		AZ::TransformBus::Event(e, &AZ::TransformBus::Events::SetLocalScale, AZ::Vector3(m_starScale, m_starScale, m_starScale));
	}

	m_moved = true;
	m_lastMovedSeconds = 0;
}

double CaelumMagnaVR::StarMapComponent::GetViewAreaExtentMetres()
{
	return m_viewAreaExtentMetres;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetParsecCoords()
{
	return AZ::Vector3(
		static_cast<float>(m_originOffsetParsecsX), 
		static_cast<float>(m_originOffsetParsecsY),
		static_cast<float>(m_originOffsetParsecsZ)
	);
}

float CaelumMagnaVR::StarMapComponent::GetStarScale()
{
	return m_starScale;
}

bool CaelumMagnaVR::StarMapComponent::GetSolVisible()
{
	bool isVisible = true;

	AZ::Vector3 pos = GAIAPosToGamePos(Coord3D(0, 0, 0));

	//Is star being drawn outside view area?
	if (pos.GetX() < -m_viewAreaExtentMetres ||
		pos.GetX() > m_viewAreaExtentMetres ||
		pos.GetY() < -m_viewAreaExtentMetres ||
		pos.GetY() > m_viewAreaExtentMetres ||
		pos.GetZ() < -m_viewAreaExtentMetres ||
		pos.GetZ() > m_viewAreaExtentMetres
		)
	{
		isVisible = false;
	}

	return isVisible;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetSolViewPosition()
{
	return GAIAPosToGamePos(Coord3D(0,0,0));
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetVRControllerVelocityRight()
{
	AZ::VR::TrackingState* ts;
	AZ::VR::ControllerRequestBus::BroadcastResult(ts, &AZ::VR::ControllerRequestBus::Events::GetTrackingState, AZ::VR::ControllerIndex::RightHand);
	return ts->dynamics.linearVelocity;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetVRControllerVelocityLeft()
{
	AZ::VR::TrackingState* ts;
	AZ::VR::ControllerRequestBus::BroadcastResult(ts, &AZ::VR::ControllerRequestBus::Events::GetTrackingState, AZ::VR::ControllerIndex::LeftHand);
	return ts->dynamics.linearVelocity;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetVRControllerPositionRight()
{
	AZ::VR::TrackingState* ts;
	AZ::VR::ControllerRequestBus::BroadcastResult(ts, &AZ::VR::ControllerRequestBus::Events::GetTrackingState, AZ::VR::ControllerIndex::RightHand);
	return ts->pose.position;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetVRControllerPositionLeft()
{
	AZ::VR::TrackingState* ts;
	AZ::VR::ControllerRequestBus::BroadcastResult(ts, &AZ::VR::ControllerRequestBus::Events::GetTrackingState, AZ::VR::ControllerIndex::LeftHand);
	return ts->pose.position;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetVRControllerAccelerationRight()
{
	AZ::VR::TrackingState* ts;
	AZ::VR::ControllerRequestBus::BroadcastResult(ts, &AZ::VR::ControllerRequestBus::Events::GetTrackingState, AZ::VR::ControllerIndex::RightHand);
	return ts->dynamics.linearAcceleration;
}

AZ::Vector3 CaelumMagnaVR::StarMapComponent::GetVRControllerAccelerationLeft()
{
	AZ::VR::TrackingState* ts;
	AZ::VR::ControllerRequestBus::BroadcastResult(ts, &AZ::VR::ControllerRequestBus::Events::GetTrackingState, AZ::VR::ControllerIndex::LeftHand);
	return ts->dynamics.linearAcceleration;
}

void CaelumMagnaVR::StarMapComponent::ResultReady(const unsigned long long queryID)
{
	std::unique_lock<std::mutex> lockF(m_starQueryIDsMutex);
	bool resultIsOurs = (std::find(m_starQueryIDs.begin(), m_starQueryIDs.end(), queryID) != m_starQueryIDs.end());
	lockF.unlock();

	//Skip results that are not a star query.
	if (!resultIsOurs) return;

	std::shared_ptr<PLY::PLYResult> r;
	PLY::PLYRequestBus::BroadcastResult(r, &PLY::PLYRequestBus::Events::GetResult, queryID);

	if (r != nullptr)
	{
		if (r->errorType == PLY::PLYResult::ResultErrorType::NONE && r->resultSet.size() > 0)
		{
			try
			{
				//Display incoming stars.
				for (auto &star : r->resultSet)
				{
					if (star.size() != 3)
					{
						CMLOG(CMLog::PLY_ERROR, "Incorrect number of columns returned for star data.");
						continue;
					}

					m_starsToMove.push_back(GAIAPosToGamePos(Coord3D(star[0].as<double>(), star[1].as<double>(), star[2].as<double>())));
				}

				//Shuffle star move list so stars are moved evenly over time across the whole view space not block by block.
				std::random_shuffle(m_starsToMove.begin(), m_starsToMove.end());
			}
			catch (pqxx::conversion_error &e)
			{
				CMLOG(CMLog::PLY_ERROR, "Libpqxx conversion error: " + AZStd::string(e.what()));
			}
		}

		double ms = r->queryEndTime.GetMilliseconds() - r->queryStartTime.GetMilliseconds();

		AZ_Printf("CM", "Query ID %u: %u stars in %f ms", queryID, r->resultSet.size(), ms);

		PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::RemoveResult, queryID);

		std::unique_lock<std::mutex> lockR(m_starQueryIDsMutex);
		m_starQueryIDs.erase(std::remove(m_starQueryIDs.begin(), m_starQueryIDs.end(), queryID), m_starQueryIDs.end());
		lockR.unlock();
	}
}

void StarMapComponent::Init()
{
	AZ_Printf("Script", "%s", "Star Map has initialised");
}

void StarMapComponent::Activate()
{
	m_starPoolSize_UI_slidervalue = static_cast<float>(m_starPoolSize);

	AZ_Printf("Script", "%s", "Star Map has activated");

	StarMapComponentRequestBus::Handler::BusConnect();

	AZ::TickBus::Handler::BusConnect();

	PLY::PLYResultBus::Handler::BusConnect();

	LmbrCentral::SpawnerComponentNotificationBus::Handler::BusConnect(GetEntityId());

	PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::InitialisePool);

	//Save original settings to use for scene reset.
	m_original_originOffsetParsecsX = m_originOffsetParsecsX;
	m_original_originOffsetParsecsY = m_originOffsetParsecsY;
	m_original_originOffsetParsecsZ = m_originOffsetParsecsZ;
	m_original_areaExtentParsecs = m_areaExtentParsecs;
	m_original_viewAreaExtentMetres = m_viewAreaExtentMetres;
	m_orginial_starScale = m_starScale;
}

void StarMapComponent::Deactivate()
{
	StarMapComponentRequestBus::Handler::BusDisconnect();

	AZ::TickBus::Handler::BusDisconnect();

	PLY::PLYResultBus::Handler::BusDisconnect();

	LmbrCentral::SpawnerComponentNotificationBus::Handler::BusDisconnect();

	PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::DeInitialisePool);

	AZ_Printf("Script", "%s", "Star Map has deactivated");
}

void CaelumMagnaVR::StarMapComponent::OnEntitySpawned(const AzFramework::SliceInstantiationTicket &ticket, const AZ::EntityId &spawnedEntity)
{
	if (m_starPool.size() < m_starPoolSize)
	{
		m_entityIDsTickets[spawnedEntity] = ticket;

		//Add star pointer to entity pool.
		m_starPool.push_front(spawnedEntity);

		//Set object invisible.
		LmbrCentral::MeshComponentRequestBus::Event(spawnedEntity, &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, false);

		if (m_starPool.size() == m_starPoolSize) GetStars();
	}
	else
	{
		//Star pool is already full, so delete this excess entity.
		LmbrCentral::SpawnerComponentRequestBus::Broadcast(&LmbrCentral::SpawnerComponentRequestBus::Events::DestroySpawnedSlice, ticket);
	}
}

void StarMapComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
{

	//Load new stars if view area offset was moved, and a sufficient time has passed since last move.
	if (m_moved)
	{
		m_lastMovedSeconds += deltaTime;

		if (m_lastMovedSeconds > m_getNewStarsAfterMoveSeconds)
		{
			m_moved = false;
			m_lastMovedSeconds = 0;

			if (m_starPoolSize > m_starPool.size())
			{
				AddToStarPool(m_starPoolSize - (int)m_starPool.size());
			}

			GetStars();
		}
	}

	if (m_starPoolInitialised && !m_initialised)
	{
		m_initialised = true;
	}

	//Initialise star object pool.
	if (!m_starPoolInitialised && !m_starPoolInitialising)
	{
		m_starPoolInitialising = true;

		AddToStarPool(m_starPoolSize);
	}

	//Finish star pool inisialisation when pool is filled to max allowed size.
	if (!m_starPoolInitialised && m_starPoolInitialising)
	{
		if (m_starPool.size() >= m_starPoolSize)
		{
			m_starPoolInitialising = false;
			m_starPoolInitialised = true;
		}
	}

	//Move stars to new positions in batches per frame, to prevent stuttering.
	//Moving too many stars at once can cause significant frame performance drops.
	//This is especially noticable in VR.
	if (m_initialised)
	{
		std::vector<AZ::Vector3>::iterator it = m_starsToMove.begin();
		int count = 1;
		while (it != m_starsToMove.end())
		{
			MoveNextStarInPool(*it);

			count++;

			it = m_starsToMove.erase(it);

			if (count == m_maxStarsMovedPerFrame) break;
		}
	}
}

void CaelumMagnaVR::StarMapComponent::GetStars()
{
	AZStd::string xmin = AZStd::string::format("%.15lf", m_originOffsetParsecsX - m_areaExtentParsecs);
	AZStd::string ymin = AZStd::string::format("%.15lf", m_originOffsetParsecsY - m_areaExtentParsecs);
	AZStd::string zmin = AZStd::string::format("%.15lf", m_originOffsetParsecsZ - m_areaExtentParsecs);
	AZStd::string xmax = AZStd::string::format("%.15lf", m_originOffsetParsecsX + m_areaExtentParsecs);
	AZStd::string ymax = AZStd::string::format("%.15lf", m_originOffsetParsecsY + m_areaExtentParsecs);
	AZStd::string zmax = AZStd::string::format("%.15lf", m_originOffsetParsecsZ + m_areaExtentParsecs);

	int offset = 0;
	int offsetStep = m_starPoolSize / m_cubeDensity;
	for (int i = 0; i < m_cubeDensity; i++)
	{
		//Send query to get stars in the subcube area.

		//Use &&& operator for comparison in 3D - May not always use indexes.
		AZStd::string starQuery = (std::string("select ST_X(geom), ST_Y(geom), ST_Z(geom) from gaia_main_2 \
			where ST_3DMakeBox(\
			'pointz(") + xmin.c_str() + " " + ymin.c_str() + " " + zmin.c_str() + ")'::geometry,\
			'pointz(" + xmax.c_str() + " " + ymax.c_str() + " " + zmax.c_str() + ")'::geometry\
			) &&& geom order by random_index offset " + std::to_string(offset) + " limit " + std::to_string(offsetStep) + ";").c_str();

		//Use ST_3DIntersects for comparison in 3D - May use indexes more efficiently in Postgres 12 / PostGIS 3.0 than &&& operator?
		/*AZStd::string starQuery = (std::string("select ST_X(geom), ST_Y(geom), ST_Z(geom) from gaia_main \
			where ST_3DIntersects(ST_3DMakeBox(\
			'pointz(") + xmin.c_str() + " " + ymin.c_str() + " " + zmin.c_str() + ")'::geometry,\
			'pointz(" + xmax.c_str() + " " + ymax.c_str() + " " + zmax.c_str() + ")'::geometry\
			), geom) limit " + std::to_string(maxStarsPerSubCube) + ";").c_str();*/

		unsigned long long queryID = 0;

		PLY::PLYRequestBus::BroadcastResult(queryID, &PLY::PLYRequestBus::Events::SendQuery, starQuery);

		//Record this query ID in the list of star query IDs so we can identify them when query results are returned.
		std::unique_lock<std::mutex> lockB(m_starQueryIDsMutex);
		m_starQueryIDs.push_back(queryID);
		lockB.unlock();

		offset += offsetStep;

	}

}

void CaelumMagnaVR::StarMapComponent::AddToStarPool(int count)
{
	for (int i = 0; i < count; i++)
	{
		AZ::Transform t = AZ::Transform::CreateScale(AZ::Vector3(m_starScale, m_starScale, m_starScale));

		//Move new star well outside view area so it won't be seen until it is used.
		t.SetPosition(AZ::Vector3(-9000, -9000, -9000));

		LmbrCentral::SpawnerComponentRequestBus::Broadcast(&LmbrCentral::SpawnerComponentRequestBus::Events::SpawnAbsolute, t);
	}
}

void CaelumMagnaVR::StarMapComponent::RemoveFromStarPool(int count)
{
	std::list<AZ::EntityId>::iterator it = m_starPool.begin();
	int numDeleted = 0;
	while (it != m_starPool.end() && numDeleted < count)
	{
		//Delete excess entity.
		if (m_entityIDsTickets.count(*it) != 0)
		{
			LmbrCentral::SpawnerComponentRequestBus::Broadcast(&LmbrCentral::SpawnerComponentRequestBus::Events::DestroySpawnedSlice, m_entityIDsTickets[*it]);

			m_entityIDsTickets.erase(*it);

			it = m_starPool.erase(it);

			numDeleted++;
		}
		else
		{
			it++;
		}
	}
}

void CaelumMagnaVR::StarMapComponent::MoveNextStarInPool(AZ::Vector3 pos)
{

	//Don't spawn stars outside view area.
	if (pos.GetX() < -m_viewAreaExtentMetres ||
		pos.GetX() > m_viewAreaExtentMetres ||
		pos.GetY() < -m_viewAreaExtentMetres ||
		pos.GetY() > m_viewAreaExtentMetres ||
		pos.GetZ() < -m_viewAreaExtentMetres ||
		pos.GetZ() > m_viewAreaExtentMetres
		)
	{
		return;
	}

	//Get oldest star and move it to front of the list.
	AZ::EntityId ent = m_starPool.back();
	m_starPool.pop_back();
	m_starPool.push_front(ent);

	AZ::TransformBus::Event(ent, &AZ::TransformBus::Events::SetLocalTranslation, pos);
	AZ::TransformBus::Event(ent, &AZ::TransformBus::Events::SetLocalScale, AZ::Vector3(m_starScale, m_starScale, m_starScale));

	//Set object visible.
	LmbrCentral::MeshComponentRequestBus::Event(ent, &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, true);
}

void CaelumMagnaVR::StarMapComponent::RedrawStarAt(AZ::EntityId e, AZ::Vector3 p) const
{
	
	AZ::TransformBus::Event(e, &AZ::TransformBus::Events::SetLocalTranslation, p);

	//Hide stars that move outside the view area, and show any that move back into it.
	if (p.GetX() < -m_viewAreaExtentMetres ||
		p.GetX() > m_viewAreaExtentMetres ||
		p.GetY() < -m_viewAreaExtentMetres ||
		p.GetY() > m_viewAreaExtentMetres ||
		p.GetZ() < -m_viewAreaExtentMetres ||
		p.GetZ() > m_viewAreaExtentMetres
		)
	{
		LmbrCentral::MeshComponentRequestBus::Event(e, &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, false);
	}
	else
	{
		LmbrCentral::MeshComponentRequestBus::Event(e, &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, true);
	}
}
