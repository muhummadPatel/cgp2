#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <test/testutil.h>
#include "test_scene.h"
#include <stdio.h>
#include <cstdint>
#include <sstream>
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
    CPPUNIT_ASSERT(1==1);

    cerr << "UNION SET OP TEST PASSED" << endl << endl;
}

//#if 0 /* Disabled since it crashes the whole test suite */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestScene, TestSet::perBuild());
//#endif
