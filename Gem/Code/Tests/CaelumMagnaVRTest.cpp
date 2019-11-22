// A simple test to see if the PLY Gem is available to the main project API.
// @author Ashley Flynn - https://ajflynn.io/ - The Academy of Interactive Entertainment and the Canberra Institute of Technology - 2019
#include <AzTest/AzTest.h>

#include <PLY/PLYRequestBus.h>

class CaelumMagnaVRTest
    : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }
};

//TEST_F(CaelumMagnaVRTest, ExampleTest)
//{
//    ASSERT_TRUE(true);
//}


/**
* Test if PLY gem can be called, as well as if the Libpq implemetation has been compiled thread safe.
*/
TEST_F(CaelumMagnaVRTest, LibpqThreadSafe)
{
	bool result = false;
	PLY::PLYRequestBus::BroadcastResult(result, &PLY::PLYRequestBus::Events::GetLibpqThreadsafe);
	ASSERT_TRUE(result);
}

AZ_UNIT_TEST_HOOK();
