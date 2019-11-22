// Core Lumberyard module for the project. Loads all required components.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <CaelumMagnaVRSystemComponent.h>
#include <Components/StarMapComponent.h>
#include <Components/StarMapBoundaryScalerComponent.h>
#include <Components/CMObjectSyncComponent.h>
#include <Components/SyncDemoComponent.h>

namespace CaelumMagnaVR
{
    class CaelumMagnaVRModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(CaelumMagnaVRModule, "{EBD4636D-0CC9-4BA2-8DFC-751E0264BA48}", AZ::Module);
        AZ_CLASS_ALLOCATOR(CaelumMagnaVRModule, AZ::SystemAllocator, 0);

        CaelumMagnaVRModule()
            : AZ::Module()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                CaelumMagnaVRSystemComponent::CreateDescriptor(),
				StarMapComponent::CreateDescriptor(),
				StarMapBoundaryScalerComponent::CreateDescriptor(),
				CMObjectSyncComponent::CreateDescriptor(),
				SyncDemoComponent::CreateDescriptor()
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<CaelumMagnaVRSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(CaelumMagnaVR_04780ec16d434ce3b12ba58e2307d2e8, CaelumMagnaVR::CaelumMagnaVRModule)
