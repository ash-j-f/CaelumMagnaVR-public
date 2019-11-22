// Star map core functionality. Provides all of the essential methods and behaviours for star display and navigation.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <vector>
#include <list>
#include <map>
#include <mutex>

#include <VRControllerBus.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <LmbrCentral/Scripting/SpawnerComponentBus.h>

#include <CaelumMagnaVR/StarMapComponentBus.h>

#include <PLY/PLYResultBus.h>

namespace CaelumMagnaVR
{
	
	class StarMapComponent
		: public AZ::Component, 
		protected PLY::PLYResultBus::Handler,
		public AZ::TickBus::Handler,
		protected StarMapComponentRequestBus::Handler,
		protected LmbrCentral::SpawnerComponentNotificationBus::Handler
	{

	public:

		StarMapComponent();
		~StarMapComponent();

		//UUID crested using VS > Tools > Create GUID. This can be any GUID unique to the project.
		AZ_COMPONENT(StarMapComponent, "{6D7C9A0B-36FA-42C0-AC6D-F7F470CF13A8}", AZ::Component);

		// Required Reflect function.
		static void Reflect(AZ::ReflectContext* context);

		//High precision 3D corrdinates class.
		class Coord3D
		{
		public:

			double x;
			double y;
			double z;

			Coord3D() : x(0.0), y(0.0), z(0.0) {};
			Coord3D(double nx, double ny, double nz) : x(nx), y(ny), z(nz) {};
		};

		//Convert star map position in parsecs to game world position in metres.
		//Takes into account the current zoom levels, view position offset, etc.
		//@param pos The star position in parsecs.
		//@returns The star position in the game world, in metres.
		AZ::Vector3 GAIAPosToGamePos(const Coord3D &pos) const;

		//Move the star field in a given direction.
		//@param deltaTime Time since last frame.
		//@param speed Movement speed in game units (metres) per second.
		//@param direction Movement direction.
		void MoveStars(float deltaTime, float speed, AZ::Vector3 direction) override;

		//Zoom the star field within the current view area.
		//@param deltaTime Time since last frame.
		//@param speed Zoom speed in game units (metres) per second.
		void ZoomStars(float deltaTime, float speed) override;

		//Zoom the size of the visible area cube.
		//@param deltaTime Time since last frame.
		//@param speed Zoom speed in game units (metres) per second.
		void ZoomVisArea(float deltaTime, float speed) override;

		//Change the scale (size) of all star objects.
		//@param deltaTime Time since last frame.
		//@param speed Speed to change star scale in units per second.
		void ChangeStarScale(float deltaTime, float speed) override;

		//Change the maximum number of stars to display at once.
		//@param deltaTime Time since last frame.
		//@param speed Speed to change max star count in units per second.
		void ChangeMaxStarCount(float deltaTime, float speed);

		//Reset the game world to refault starting position and settings.
		void Reset() override;

		//Get the extent (distance from centre to edge) of the game view area in metres.
		double GetViewAreaExtentMetres() override;

		//Get the extent (distance from centre to edge) of the star field in parsecs.
		double GetAreaExtentParsecs() override { return m_areaExtentParsecs; };

		//The the size (number of elements) of the star object pool.
		int GetStarPoolSize() override { return m_starPoolSize; };

		//Is Sol (Earth's Sun) currently visible in the view area?
		bool GetSolVisible() override;

		//Get the position of Sol (Earth's Sun) in the view area, in game units.
		AZ::Vector3 GetSolViewPosition() override;

		//Get the current view offset from the origin in parsecs.
		AZ::Vector3 GetParsecCoords() override;

		//The the current scale (size) of star objects.
		float GetStarScale() override;

		//Get right VR controller velocity.
		AZ::Vector3 GetVRControllerVelocityRight() override;

		//Get left VR controller velocity.
		AZ::Vector3 GetVRControllerVelocityLeft() override;

		//Get right VR controller position.
		AZ::Vector3 GetVRControllerPositionRight() override;

		//Get left VR controller position.
		AZ::Vector3 GetVRControllerPositionLeft() override;

		//Get right VR controller acceleration.
		AZ::Vector3 GetVRControllerAccelerationRight() override;

		//Get left VR controller acceleration.
		AZ::Vector3 GetVRControllerAccelerationLeft() override;

	protected:
		
		//Advertises a result ID is ready.
		//@param queryID The ID of the ready result.
		void ResultReady(const unsigned long long queryID) override;

		//AZ::Component interface implementation.
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		//Actions to perform when an entity is spawned.
		//@param ticket The slice instantiation ticket.
		//@spawnedEntity The ID of the spawned entity.
		void OnEntitySpawned(const AzFramework::SliceInstantiationTicket& ticket, const AZ::EntityId& spawnedEntity) override;

		// Optional functions for defining provided and dependent services.
		/*
		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		*/

	private:

		//Offset from origin in parsecs.
		double m_originOffsetParsecsX;
		double m_originOffsetParsecsY;
		double m_originOffsetParsecsZ;

		//Star view area extent (distance from centre to edge) in parsecs.
		double m_areaExtentParsecs;

		//Star view area extent (distance from centre to edge) in metres.
		double m_viewAreaExtentMetres;

		//Saved values to use for view reset.
		double m_original_originOffsetParsecsX;
		double m_original_originOffsetParsecsY;
		double m_original_originOffsetParsecsZ;
		double m_original_areaExtentParsecs;
		double m_original_viewAreaExtentMetres;
		float m_orginial_starScale;

		//Current star scale (visible size).
		float m_starScale;

		//Maximum stars to move per frame. Stars are moved in batches per frame to avoid stuttering.
		int m_maxStarsMovedPerFrame;

		//Number of spatial cubes to search at once per star data refresh.
		//This value is directly proportional to the number of query threads that 
		//will be employed simultaneously (up to the max allowed query worker threads)
		//for the search.
		int m_cubeDensity;

		//Is the scene initialised?
		bool m_initialised;

		//Has the star field offset or zoom changed?
		bool m_moved;

		//How long ago the star field offset or zoom last changed.
		float m_lastMovedSeconds;

		//Time to wait after star field offset or zoom has changed before loading new stars in the visible area.
		float m_getNewStarsAfterMoveSeconds;

		//Mutex to lock the star query IDs list.
		std::mutex m_starQueryIDsMutex;
		//List of query IDs associated with star data searches.
		std::vector<unsigned long long> m_starQueryIDs;

		//Positions to move stars to, from the star pool.
		std::vector<AZ::Vector3> m_starsToMove;

		//Current max star pool size selection in the UI.
		//Must be a float to allow fractions of a whole number to be scaled up or down between frames.
		float m_starPoolSize_UI_slidervalue;
		
		//Current max star pool size.
		int m_starPoolSize;
		
		//Is the star pool initialising?
		bool m_starPoolInitialising;

		//Has the star pool initialised?
		bool m_starPoolInitialised;

		//Star pool. The star pool is initialised at the start of the application loading.
		std::list<AZ::EntityId> m_starPool;

		//Instantiation tickets and their associated entity IDs.
		//Used to identify the ticket associated with an entity when it needs to be de-spawned at a later time.
		std::map<AZ::EntityId, AzFramework::SliceInstantiationTicket> m_entityIDsTickets;

		//Tick order definition. This value sets where in global tick order this component is called.
		//TICK_PLACEMENT is fairly early in the tick order.
		//TICK_DEFAULT is the default position for components.
		//TICK_GAME is recommended for "game related components".
		inline int GetTickOrder() override { return AZ::TICK_GAME; };

		//Tick handler.
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

		//Load stars from the database for the currently viewed volume in space.
		void GetStars() override;

		//Add a chosen number of stars to the star pool.
		//@param count The number of stars to add.
		void AddToStarPool(int count);

		//Remove a chosen number of stars from the star pool.
		//@param count The number of stars to remove.
		void RemoveFromStarPool(int count);

		//Move the oldest star from the star pool to a new location.
		//This star then becomes the newest star in the star pool.
		//@param pos The new position to move the star to.
		void MoveNextStarInPool(AZ::Vector3 pos);

		//Move a specific star from its current position to a new position.
		//@param e The entity ID of the star.
		//@param p The new position to move the star to.
		void RedrawStarAt(AZ::EntityId e, AZ::Vector3 p) const;

	};
}