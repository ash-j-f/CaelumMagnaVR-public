// Game object to database synchronisation demo component. Provides core functionality for the serialisation, 
// de-serialisation and synchronisation demo.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <vector>
#include <mutex>

#include <AzCore/Math/Transform.h>
#include <AzCore/Math/Quaternion.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <AzFramework/Entity/EntityContextBus.h>

#include <LmbrCentral/Scripting/SpawnerComponentBus.h>

#include <CaelumMagnaVR\SyncDemoComponentBus.h>

#include <PLY/PLYResultBus.h>
#include <PLY/PLYRequestBus.h>

namespace CaelumMagnaVR
{

	class SyncDemoComponent
		: public AZ::Component,
		protected AZ::TickBus::Handler,
		protected PLY::PLYResultBus::Handler,
		protected LmbrCentral::SpawnerComponentNotificationBus::Handler,
		protected SyncDemoComponentBus::Handler
	{

	public:

		SyncDemoComponent();
		~SyncDemoComponent();

		//UUID crested using VS > Tools > Create GUID. This can be any GUID unique to the project.
		AZ_COMPONENT(SyncDemoComponent, "{C7A7EC97-4404-4E3D-8015-55EC0BB17E51}", AZ::Component);

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		//Object ID and serialised data sting pair.
		struct ObjectIDAndDataString
		{
			int object_id;
			std::string data;
		};

	protected:

		// AZ::Component interface implementation.
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// Optional functions for defining provided and dependent services.
		/*
		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		*/

		//Advertises a result ID is ready.
		//@param queryID The ID of the ready result.
		void ResultReady(const unsigned long long queryID) override;

		//Actions to perform when an entity is spawned.
		//@param ticket The slice instantiation ticket.
		//@spawnedEntity The ID of the spawned entity.
		void OnEntitySpawned(const AzFramework::SliceInstantiationTicket& ticket, const AZ::EntityId& spawnedEntity) override;

		//Reset the scene, removing all dynamically created objects, and returning permanent objects to their starting position.
		void Reset() override;

	private:

		//Table and column names to use for object data storage and retrieval.
		AZStd::string m_tableName; //The table in which object data is stored.
		AZStd::string m_IDColumnName; //Unique ID column. Must be an INT and the PRIMARY KEY so as not to allow duplicates.
		AZStd::string m_dataColumnName; //The column in which serialised JSON data for the object is stored. Must be of TEXT type.

		//Time to wait between spawning new randomly placed objects.
		float m_spawnTimeSeconds;

		//Maximum objects to spawn in the scene.
		int m_maxObjectCount;

		//Has the demo scene been initialised?
		bool m_sceneInitialised;

		//Is the scene in the process of initialising?
		bool m_sceneInitialising;

		//Time since last object spawned, in seconds.
		float m_spawnTimer;

		//Scene initialisation query ID.
		unsigned long long m_sceneInitQueryID;

		//Scene reset query ID.
		unsigned long long m_resetqueryID;

		//Map of instantiation tickets and object information from the database.
		//Used in conjuction with the ebus call to OnEntitySpawned to assign database object ID and JSON 
		//data to objects as they are spawned.
		AZStd::unordered_map<AzFramework::SliceInstantiationTicket, ObjectIDAndDataString> m_instantiationMap;

		//Tick handler.
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		//Tick order for the tick handler.
		inline int GetTickOrder() override { return AZ::ComponentTickBus::TICK_GAME; };

	};
}