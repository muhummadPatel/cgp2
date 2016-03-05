#ifndef TILER_TEST_SCENE_H
#define TILER_TEST_SCENE_H


#include <string>
#include <cppunit/extensions/HelperMacros.h>
#include "tesselate/csg.h"

/// Test code for @ref Scene
class TestScene : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestScene);
    CPPUNIT_TEST(testVoxSetOp_union);
    CPPUNIT_TEST_SUITE_END();

private:
    Scene * scene;

public:

    /// Initialization before unit tests
    void setUp();

    /// Tidying up after unit tests
    void tearDown();

    /**
     * Test that the UNION set operation produces the expected results
     */
    void testVoxSetOp_union();
};

#endif /* !TILER_TEST_SCENE_H */
