// Ebus for the object to database synchronisation demo. Provides control over the demo scene and database records.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#pragma once

#include <AzCore/EBus/EBus.h>

namespace CaelumMagnaVR
{
    class SyncDemoComponentRequests
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////

        //Reset the scene, removing all dynamically created objects, and returning permanent objects to their starting position.
		virtual void Reset() = 0;
    };
    using SyncDemoComponentBus = AZ::EBus<SyncDemoComponentRequests>;
} // namespace CaelumMagnaVR
