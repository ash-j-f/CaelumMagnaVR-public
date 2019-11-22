// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include "SyncDemoComponent.h"

#include <AZCore/EBus/Results.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/Component/TransformBus.h>

#include <LyShine/Bus/UiSpawnerBus.h>

#include <LmbrCentral/Rendering/MeshComponentBus.h>

#include <PLY/PLYObjectSyncSaveLoadBus.h>
#include <PLY/PLYObjectSyncEntitiesBus.h>
#include <PLY/PLYRequestBus.h>
#include <PLY/PLYTools.h>
#include <PLY/PLYTypes.h>

#include "CMObjectSyncComponent.h"

#include <CMLog.h>

using namespace CaelumMagnaVR;

SyncDemoComponent::SyncDemoComponent() :
	m_sceneInitialised(false),
	m_sceneInitialising(false),
	m_spawnTimer(0),
	m_sceneInitQueryID(0),
	m_resetqueryID(0),
	m_spawnTimeSeconds(1),
	m_maxObjectCount(10)
{	

}

SyncDemoComponent::~SyncDemoComponent()
{
}

void SyncDemoComponent::Reflect(AZ::ReflectContext* context)
{
	AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context);
	if (serialize)
	{
		// Reflect the class fields that you want to serialize.
		// In this example, m_runtimeStateNoSerialize is not reflected for serialization.
		// Base classes with serialized data should be listed as additional template
		// arguments to the Class< T, ... >() function.
		serialize->Class<SyncDemoComponent, AZ::Component>()
			->Version(1)
			->Field("SpawnTime", &SyncDemoComponent::m_spawnTimeSeconds)
			->Field("MaxObjects", &SyncDemoComponent::m_maxObjectCount)
			->Field("TableName", &SyncDemoComponent::m_tableName)
			->Field("IDColumnName", &SyncDemoComponent::m_IDColumnName)
			->Field("DataColumnName", &SyncDemoComponent::m_dataColumnName)
			;

		AZ::EditContext* edit = serialize->GetEditContext();
		if (edit)
		{
			edit->Class<SyncDemoComponent>("PLY Object sync Demo controller", "PLY Object sync Demo controller")
				->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
				->DataElement(AZ::Edit::UIHandlers::Default, &SyncDemoComponent::m_spawnTimeSeconds, "Spawn Time", "The spawn time in seconds.")
					->Attribute(AZ::Edit::Attributes::Min, 0.01)
				->DataElement(AZ::Edit::UIHandlers::Default, &SyncDemoComponent::m_maxObjectCount, "Max Objects", "Max objects allowed in the scene.")
					->Attribute(AZ::Edit::Attributes::Min, 1)
				->DataElement(AZ::Edit::UIHandlers::Default, &SyncDemoComponent::m_tableName, "Table Name", "Storage table name")
				->DataElement(AZ::Edit::UIHandlers::Default, &SyncDemoComponent::m_IDColumnName, "ID Column Name", "Object identifier column name")
				->DataElement(AZ::Edit::UIHandlers::Default, &SyncDemoComponent::m_dataColumnName, "Data Column Name", "Object storage column name")
				;
		}
	}
}

void SyncDemoComponent::Init()
{
	AZ_Printf("Script", "%s", "Sync Demo has initialised");
}

void SyncDemoComponent::Activate()
{
	AZ_Printf("Script", "%s", "Sync Demo has activated");

	AZ::TickBus::Handler::BusConnect();

	PLY::PLYResultBus::Handler::BusConnect();

	LmbrCentral::SpawnerComponentNotificationBus::Handler::BusConnect(GetEntityId());

	SyncDemoComponentBus::Handler::BusConnect();

	PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::InitialisePool);

}

void SyncDemoComponent::Deactivate()
{

	AZ::TickBus::Handler::BusDisconnect();

	PLY::PLYResultBus::Handler::BusDisconnect();

	LmbrCentral::SpawnerComponentNotificationBus::Handler::BusDisconnect();

	SyncDemoComponentBus::Handler::BusDisconnect();

	PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::DeInitialisePool);

	AZ_Printf("Script", "%s", "Sync Demo has deactivated");
}

void SyncDemoComponent::ResultReady(const unsigned long long queryID)
{
	if (queryID == m_sceneInitQueryID)
	{
		m_sceneInitQueryID = 0;

		std::shared_ptr<PLY::PLYResult> result;

		//Check query completed ok.
		PLY::PLYRequestBus::BroadcastResult(result, &PLY::PLYRequestBus::Events::GetResult, queryID);
		if (result == nullptr || result->errorType != PLY::PLYResult::NONE || result->errorMessage != "")
		{
			CMLOG(CMLog::PLY_ERROR, "Couldn't initialise scene objects. Database query failed.");
			return;
		}

		//Get a list of database object ids already in use by spawned entities.
		AZ::EBusAggregateResults<int> existingOIDs;
		PLY::PLYObjectSyncEntitiesBus::BroadcastResult(existingOIDs, &PLY::PLYObjectSyncEntitiesBus::Events::GetAllObjectIDs);

		if (result->resultSet.size() > 0)
		{
			for (auto &row : result->resultSet)
			{
				if (row.size() != 2)
				{
					CMLOG(CMLog::PLY_ERROR, "Not enough columns returned by query in demo scene setup.");
					break;
				}

				int object_id = row[0].as<int>();

				//If an object with this database ID has already been spawned, then skip this object.
				if (std::find(existingOIDs.values.begin(), existingOIDs.values.end(), object_id) != existingOIDs.values.end()) continue;

				const char *data = row[1].c_str();

				//Parse the serialised data from the JSON string.
				CMObjectSyncComponent::TransformAsVecs tv = CMObjectSyncComponent::ParseJSONDataString(data);
				
				//Set the object's scale and translation from the database data.
				AZ::Transform tr = AZ::Transform::CreateScale(tv.scale);
				tr.SetTranslation(tv.translation);

				//Spawn the object in the scene.
				AzFramework::SliceInstantiationTicket sit;
				LmbrCentral::SpawnerComponentRequestBus::BroadcastResult(sit,
					&LmbrCentral::SpawnerComponentRequestBus::Events::SpawnAbsolute, tr);

				//Record the instantiation ticket and database object ID in a map 
				//so the database id can be assigned to the entity when it has finished spawning.
				ObjectIDAndDataString oidstring;
				oidstring.object_id = object_id;
				oidstring.data = data;
				m_instantiationMap[sit] = oidstring;

			}

		}

		//Remove the result set from the results queue.
		PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::RemoveResult, queryID);

		m_sceneInitialised = true;
		m_sceneInitialising = false;
	}

	//Check if the result was from a "reset scene" query.
	if (queryID == m_resetqueryID)
	{
		m_resetqueryID = 0;

		std::shared_ptr<PLY::PLYResult> result;

		//Check query completed ok.
		PLY::PLYRequestBus::BroadcastResult(result, &PLY::PLYRequestBus::Events::GetResult, queryID);
		if (result == nullptr || result->errorType != PLY::PLYResult::NONE || result->errorMessage != "")
		{
			CMLOG(CMLog::PLY_ERROR, "Couldn't reset scene objects. Database query failed.");
			return;
		}

		//Remove the result set from the results queue.
		PLY::PLYRequestBus::Broadcast(&PLY::PLYRequestBus::Events::RemoveResult, queryID);

		//Reset the scene state.
		m_sceneInitialised = false;
		m_sceneInitialising = false;
	}
}

void CaelumMagnaVR::SyncDemoComponent::OnEntitySpawned(const AzFramework::SliceInstantiationTicket &ticket, const AZ::EntityId &spawnedEntity)
{
	//Assign the correct database object ID to the spawned entity.

	if (m_instantiationMap.count(ticket) == 0)
	{
		return;
	}

	PLY::PLYObjectSyncSaveLoadBus::Event(spawnedEntity, &PLY::PLYObjectSyncSaveLoadBus::Events::SetObjectID, 
		m_instantiationMap[ticket].object_id);

	PLY::PLYObjectSyncDataStringBus::Event(spawnedEntity, &PLY::PLYObjectSyncDataStringBus::Events::SetPropertiesFromDataString, 
		m_instantiationMap[ticket].data);

	m_instantiationMap.erase(ticket);

	//Make the object visible.
	LmbrCentral::MeshComponentRequestBus::Event(spawnedEntity, &LmbrCentral::MeshComponentRequestBus::Events::SetVisibility, false);

}

void CaelumMagnaVR::SyncDemoComponent::Reset()
{
	m_sceneInitialised = false;
	m_sceneInitialising = true;

	//Delete all spawned entities.
	LmbrCentral::SpawnerComponentRequestBus::Broadcast(&LmbrCentral::SpawnerComponentRequestBus::Events::DestroyAllSpawnedSlices);

	//Move permanent objects back to their start positions.
	PLY::PLYObjectSyncEntitiesBus::Broadcast(&PLY::PLYObjectSyncEntitiesBus::Events::Reset);

	//Remove all sync data from the database.
	AZStd::string query = "delete from " + m_tableName + ";";
	unsigned long long queryID = 0;
	PLY::PLYRequestBus::BroadcastResult(queryID, &PLY::PLYRequestBus::Events::SendQuery, query);

	m_resetqueryID = queryID;
}

void SyncDemoComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
{
	//Initialise scene if required.
	if (!m_sceneInitialised && !m_sceneInitialising)
	{
		m_sceneInitialising = true;

		AZStd::string query = "select " + m_IDColumnName + ", " + m_dataColumnName + " from " + m_tableName + ";";

		unsigned long long queryID = 0;

		PLY::PLYRequestBus::BroadcastResult(queryID, &PLY::PLYRequestBus::Events::SendQuery, query);

		m_sceneInitQueryID = queryID;
	}

	//Increment timer used to spawn new entities at regular intervals.
	if (m_sceneInitialised)
	{
		m_spawnTimer += deltaTime;
	}

	//If spawn timer has reached its limit, spawn a new entity at a random location and orientation.
	if (m_sceneInitialised && m_spawnTimer > m_spawnTimeSeconds)
	{
		m_spawnTimer = 0;

		//Count how many synced objects are in the scene. If too few, spawn new objects at a random location.
		//Get a list of database object ids already in use by spawned entities.
		AZ::EBusAggregateResults<int> existingOIDs;
		PLY::PLYObjectSyncEntitiesBus::BroadcastResult(existingOIDs, &PLY::PLYObjectSyncEntitiesBus::Events::GetAllObjectIDs);

		int maxObjectTarget = m_maxObjectCount;

		//Find max object ID.
		int maxOID = 0;
		for (auto &oid : existingOIDs.values)
		{
			if (oid > maxOID) maxOID = oid;
		}

		int newOID = maxOID + 1;

		std::mt19937 generator(std::random_device{}());

		std::uniform_real_distribution<float> distX(-5.0f, 5.0f);
		std::uniform_real_distribution<float> distY(-5.0f, 5.0f);
		std::uniform_real_distribution<float> distZ(20.0f, 30.0f);

		std::uniform_real_distribution<float> distRotX(0.0f, 6.28319f);
		std::uniform_real_distribution<float> distRotY(0.0f, 6.28319f);
		std::uniform_real_distribution<float> distRotZ(0.0f, 6.28319f);

		if (existingOIDs.values.size() < maxObjectTarget)
		{
			int spawnCount = 1; // maxObjectTarget - static_cast<int>(existingOIDs.values.size());
			for (int i = 0; i < spawnCount; i++)
			{

				AZ::Transform tr = AZ::Transform::CreateScale(AZ::Vector3(1,1,1));
			
				//Set random position.

				float x = static_cast<float>(distX(generator));
				float y = static_cast<float>(distY(generator));
				float z = static_cast<float>(distZ(generator));

				float RotX = static_cast<float>(distRotX(generator));
				float RotY = static_cast<float>(distRotY(generator));
				float RotZ = static_cast<float>(distRotZ(generator));

				AZ::Vector3 v = AZ::Vector3(x, y, z);

				tr.SetTranslation(v);

				AzFramework::SliceInstantiationTicket sit;
				LmbrCentral::SpawnerComponentRequestBus::BroadcastResult(sit,
					&LmbrCentral::SpawnerComponentRequestBus::Events::SpawnAbsolute, tr);

				//Record the instantiation ticket and database object ID in a map 
				//so the database id can be assigned to the entity when it has finished spawning.
				ObjectIDAndDataString oidstring;
				oidstring.object_id = newOID++;
				oidstring.data = "{\"Transform\":{\""
					"Translation\":{\"X\":" + std::to_string(x) + ",\"Y\":" + std::to_string(y) + ",\"Z\":" + std::to_string(z) + "},"
					"\"Rotation\":{\"X\":" + std::to_string(RotX) + ",\"Y\":" + std::to_string(RotY) + ",\"Z\":" + std::to_string(RotZ) + "},"
					"\"Scale\":{\"X\":1.0,\"Y\":1.0,\"Z\":1.0}},"
					"\"Physics\":{\"AngVel\":{\"X\":0.0,\"Y\":0.0,\"Z\":0.0},\"Vel\":{\"X\":0.0,\"Y\":0.0,\"Z\":0.0}}}";
				m_instantiationMap[sit] = oidstring;

			}
		}
	}
}
