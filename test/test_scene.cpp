#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <test/testutil.h>
#include "test_scene.h"
#include <stdio.h>
#include <cstdint>
#include <sstream>
#include <regex>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

void TestScene::setUp()
{
    scene = new Scene();
}

void TestScene::tearDown()
{
    delete scene;
}

void TestScene::testVoxSetOp_union(){
    // Generate the setop test scene and try to voxelise it
    scene->testSetOpScene(SetOp::UNION);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    // Voxels that never had any shape in them should still be empty
    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    // Voxels where the shapes overlapped should be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);

    // Voxels of the sphere should be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) == 1);

    // voxels of the cylinder should be on
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, (ydim/2)-65, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, (ydim/2)+65, zdim/2) == 1);

    cerr << "UNION SET OP TEST PASSED" << endl << endl;
}

void TestScene::testVoxSetOp_difference(){
    // Generate the setop test scene and try to voxelise it
    scene->testSetOpScene(SetOp::DIFFERENCE);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    // Voxels that never had any shape in them should still be empty
    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    // Voxels where the shapes overlapped should be off (They were subtracted)
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 0);

    // Voxels of the sphere (outside the overlapped volume) should be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    // voxels of the cylinder should be off (It was used to subtract from the sphere)
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, (ydim/2)-65, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, (ydim/2)+65, zdim/2) == 0);

    cerr << "DIFFERENCE SET OP TEST PASSED" << endl << endl;
}

void TestScene::testVoxSetOp_intersection(){
    // Generate the setop test scene and try to voxelise it
    scene->testSetOpScene(SetOp::INTERSECTION);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    // Voxels that never had any shape in them should still be empty
    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    // Voxels where the shapes overlapped/intersected should be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);

    // Voxels of the sphere (outside the overlapped volume) should be off
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  0);

    // voxels of the cylinder (outside the intersect volume) should be off
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, (ydim/2)-65, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, (ydim/2)+65, zdim/2) == 0);

    cerr << "INTERSECTION SET OP TEST PASSED" << endl << endl;
}

void TestScene::testVoxelise1Shape(){
    // Generate a test scene with only 1 shape (and no operations) and try to voxelise it
    scene->testVoxeliseScene(1);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    // Voxels that never had any shape in them should still be empty
    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    // Voxels where the one sphere was should all be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    cerr << "VOXELISE 1 SHAPE CSG TREE TEST PASSED" << endl << endl;
}

void TestScene::testVoxelise2Shape(){
    // We want to test that the tree is being evaluated correectly
    // Generate a test scene with 2 shapes and a union operation
    scene->testVoxeliseScene(2, SetOp::UNION);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    // Voxels that never had any shape in them should still be empty
    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    // voxels where the sphere and cylinder overlapped should be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);

    // The sphere voxels should also still be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    // The cylinder voxels should also still be on
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, ydim/2, zdim/2) == 1);

    cerr << "VOXELISE 2 SHAPE CSG TREE TEST PASSED" << endl << endl;
}

void TestScene::testVoxelise3Shape(){
    // We want to test that larger trees are walked and evaluated correctly
    // Generate a test scene with 3 shapes and union and difference operations
    scene->testVoxeliseScene(3, SetOp::UNION, SetOp::DIFFERENCE);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    // voxels that never had any shapes in them should still be off
    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    // voxels where the difference shapes overlapped, should be off
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 0);

    // Voxels where the sphere wasn't differenced should still be on
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    // Voxels where the unioned cylinder was should be on (because the ends of the cylinder weren't subtracted)
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, ydim/2, zdim/2) == 1);

    // Voxels where the differenced cylinder was should be off becuase it was subtracted from the scene
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, (ydim/2)-65, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, (ydim/2)+65, zdim/2) == 0);

    cerr << "VOXELISE 3 SHAPE CSG TREE TEST PASSED" << endl << endl;
}

void TestScene::testVoxeliseEmptyTree(){
    // We want to test that the program doesn't fail badly/segfault if we try to voxelise
    // an empty CSG tree. It should *calmly* tell the user that the tree was empty.
    // Redirect cerr to stringstream so we can inspect it
    std::ostringstream oss;
    std::streambuf* orig_buf(cerr.rdbuf(oss.rdbuf()));
    scene->voxelise(0.1f); // try to voxelise an empty csg tree
    cerr.rdbuf(orig_buf); // Reset cerr to print to the screen

    const string output = oss.str();
    cerr << output; // Show the user the captured output

    // look for expected output telling user that the tree was empty
    std::regex regex("CSG tree empty\\.");
    std::smatch match;
    bool found_match = std::regex_search(output.begin(), output.end(), match, regex);
    CPPUNIT_ASSERT(found_match);

    cerr << "VOXELISE EMPTY CSG TREE TEST PASSED" << endl << endl;
}

//#if 0 /* Disabled since it crashes the whole test suite */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestScene, TestSet::perBuild());
//#endif
