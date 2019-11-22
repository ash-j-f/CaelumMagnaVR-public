// Ebus for the star map. Provides a range of useful tools to intract with the star map and its data.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <AzCore/EBus/EBus.h>

namespace CaelumMagnaVR
{
    class StarMapComponentRequests
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////

        //Load stars from the database for the currently viewed volume in space.
		virtual void GetStars() = 0;
		
		//Move the star field in a given direction.
		//@param deltaTime Time since last frame.
		//@param speed Movement speed in game units (metres) per second.
		//@param direction Movement direction.
		virtual void MoveStars(float deltaTime, float speed, AZ::Vector3 direction) = 0;
		
		//Zoom the star field within the current view area.
		//@param deltaTime Time since last frame.
		//@param speed Zoom speed in game units (metres) per second.
		virtual void ZoomStars(float deltaTime, float speed) = 0;

		//Zoom the size of the visible area cube.
		//@param deltaTime Time since last frame.
		//@param speed Zoom speed in game units (metres) per second.
		virtual void ZoomVisArea(float deltaTime, float speed) = 0;

		//Change the scale (size) of all star objects.
		//@param deltaTime Time since last frame.
		//@param speed Speed to change star scale in units per second.
		virtual void ChangeStarScale(float deltaTime, float speed) = 0;

		//Change the maximum number of stars to display at once.
		//@param deltaTime Time since last frame.
		//@param speed Speed to change max star count in units per second.
		virtual void ChangeMaxStarCount(float deltaTime, float speed) = 0;

		//Reset the game world to refault starting position and settings.
		virtual void Reset() = 0;

		//Get the extent (distance from centre to edge) of the game view area in metres.
		virtual double GetViewAreaExtentMetres() = 0;
		
		//Get the extent (distance from centre to edge) of the star field in parsecs.
		virtual double GetAreaExtentParsecs() = 0;

		//The the size (number of elements) of the star object pool.
		virtual int GetStarPoolSize() = 0;

		//Is Sol (Earth's Sun) currently visible in the view area?
		virtual bool GetSolVisible() = 0;

		//Get the position of Sol (Earth's Sun) in the view area, in game units.
		virtual AZ::Vector3 GetSolViewPosition() = 0;

		//Get the current view offset from the origin in parsecs.
		virtual AZ::Vector3 GetParsecCoords() = 0;

		//The the current scale (size) of star objects.
		virtual float GetStarScale() = 0;

		//Get right VR controller velocity.
		virtual AZ::Vector3 GetVRControllerVelocityRight() = 0;

		//Get left VR controller velocity.
		virtual AZ::Vector3 GetVRControllerVelocityLeft() = 0;

		//Get right VR controller position.
		virtual AZ::Vector3 GetVRControllerPositionRight() = 0;

		//Get left VR controller position.
		virtual AZ::Vector3 GetVRControllerPositionLeft() = 0;

		//Get right VR controller acceleration.
		virtual AZ::Vector3 GetVRControllerAccelerationRight() = 0;

		//Get left VR controller acceleration.
		virtual AZ::Vector3 GetVRControllerAccelerationLeft() = 0;
    };
    using StarMapComponentRequestBus = AZ::EBus<StarMapComponentRequests>;
} // namespace CaelumMagnaVR
