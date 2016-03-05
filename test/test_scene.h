#ifndef TILER_TEST_SCENE_H
#define TILER_TEST_SCENE_H


#include <string>
#include <cppunit/extensions/HelperMacros.h>

// So that we can inspect the private members of Scene
#define private public
#include "tesselate/csg.h"
#define private private

/// Test code for @ref Scene
class TestScene : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestScene);
    CPPUNIT_TEST(testVoxSetOp_union);
    CPPUNIT_TEST(testVoxSetOp_difference);
    CPPUNIT_TEST(testVoxSetOp_intersection);
    CPPUNIT_TEST(testVoxelise1Shape);
    CPPUNIT_TEST(testVoxelise2Shape);
    CPPUNIT_TEST(testVoxelise3Shape);
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

    /**
     * Test that the DIFFERENCE set operation produces the expected results
     */
    void testVoxSetOp_difference();

    /**
     * Test that the INTERSECTION set operation produces the expected results
     */
    void testVoxSetOp_intersection();

    /**
     * Test that a csg tree with 1 shape is properly voxelised
     */
    void testVoxelise1Shape();

    /**
     * Test that a csg tree with 2 shapes is properly voxelised
     */
    void testVoxelise2Shape();

    /**
     * Test that a csg tree with 3 shapes is properly voxelised
     */
    void testVoxelise3Shape();
};

#endif /* !TILER_TEST_SCENE_H */
