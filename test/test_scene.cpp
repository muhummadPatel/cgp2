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
    scene->testSetOpScene(SetOp::UNION);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) == 1);

    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, (ydim/2)-65, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, (ydim/2)+65, zdim/2) == 1);

    cerr << "UNION SET OP TEST PASSED" << endl << endl;
}

void TestScene::testVoxSetOp_difference(){
    scene->testSetOpScene(SetOp::DIFFERENCE);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, (ydim/2)-65, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, (ydim/2)+65, zdim/2) == 0);

    cerr << "DIFFERENCE SET OP TEST PASSED" << endl << endl;
}

void TestScene::testVoxSetOp_intersection(){
    scene->testSetOpScene(SetOp::INTERSECTION);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  0);

    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, (ydim/2)-65, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, (ydim/2)+65, zdim/2) == 0);

    cerr << "INTERSECTION SET OP TEST PASSED" << endl << endl;
}

void TestScene::testVoxelise1Shape(){
    scene->testVoxeliseScene(1);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    cerr << "VOXELISE 1 SHAPE CSG TREE TEST PASSED" << endl << endl;
}

void TestScene::testVoxelise2Shape(){
    scene->testVoxeliseScene(2, SetOp::UNION);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, ydim/2, zdim/2) == 1);

    cerr << "VOXELISE 2 SHAPE CSG TREE TEST PASSED" << endl << endl;
}

void TestScene::testVoxelise3Shape(){
    scene->testVoxeliseScene(3, SetOp::UNION, SetOp::DIFFERENCE);
    scene->voxelise(0.1f);

    int xdim, ydim, zdim;
    scene->vox.getDim(xdim, ydim, zdim);

    CPPUNIT_ASSERT(scene->vox.get(0, 0, 0) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim, ydim, zdim) == 0);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)+35) == 1);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, ydim/2, (zdim/2)-35) ==  1);

    CPPUNIT_ASSERT(scene->vox.get((xdim/2)-65, ydim/2, zdim/2) == 1);
    CPPUNIT_ASSERT(scene->vox.get((xdim/2)+65, ydim/2, zdim/2) == 1);

    CPPUNIT_ASSERT(scene->vox.get(xdim/2, (ydim/2)-65, zdim/2) == 0);
    CPPUNIT_ASSERT(scene->vox.get(xdim/2, (ydim/2)+65, zdim/2) == 0);

    cerr << "VOXELISE 3 SHAPE CSG TREE TEST PASSED" << endl << endl;
}

void TestScene::testVoxeliseEmptyTree(){
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
}

//#if 0 /* Disabled since it crashes the whole test suite */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestScene, TestSet::perBuild());
//#endif
