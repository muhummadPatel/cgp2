//
// csg
//

#include "csg.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <limits>
#include <stack>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace std;
// using namespace cgp;

GLfloat defaultCol[] = {0.243f, 0.176f, 0.75f, 1.0f};

bool Scene::genVizRender(View * view, ShapeDrawData &sdd)
{
    std::vector<ShapeNode *> leaves;
    int i;

    geom.clear();
    geom.setColour(defaultCol);

    // note: this displays all the constituent shapes in the tree but doesn't apply any set operations to them
    // so it is purely a pre-visualization
    // Walk through the csg tree if it's not empty and push all shapeNodes that
    // we encounter into the leaves vector to be rendered.
    if(csgroot != NULL){
        std::vector<SceneNode*> to_visit;
        to_visit.push_back(csgroot);

        while(!to_visit.empty()){
            SceneNode* curr = to_visit[to_visit.size()-1];
            to_visit.erase(to_visit.begin() + (to_visit.size()-1));

            if(OpNode* op = dynamic_cast<OpNode*>(curr)){
                to_visit.push_back(op->right);
                to_visit.push_back(op->left);
            }else if(ShapeNode* shp = dynamic_cast<ShapeNode*>(curr)){
                leaves.push_back(shp);
            }
        }
    }

    // traverse leaf shapes generating geometry
    for(i = 0; i < (int) leaves.size(); i++)
    {
        leaves[i]->shape->genGeometry(&geom, view);
    }

    // bind geometry to buffers and return drawing parameters, if possible
    if(geom.bindBuffers(view))
    {
        sdd = geom.getDrawParameters();
        return true;
    }
    else
        return false;
}

bool Scene::genVoxRender(View * view, ShapeDrawData &sdd)
{
    int x, y, z, xdim, ydim, zdim;
    glm::mat4 tfm, idt;
    glm::vec3 trs;
    cgp::Point pnt;

    geom.clear();
    geom.setColour(defaultCol);

    if(voxactive)
    {
        idt = glm::mat4(1.0f); // identity matrix

        vox.getDim(xdim, ydim, zdim);

        // place a sphere at filled voxels but subsample to avoid generating too many spheres
        for(x = 0; x < xdim; x+=10)
            for(y = 0; y < ydim; y+=10)
                for(z = 0; z < zdim; z+=10)
                {
                    if(vox.get(x, y, z))
                    {
                        pnt = vox.getVoxelPos(x, y, z); // convert from voxel space to world coordinates
                        trs = glm::vec3(pnt.x, pnt.y, pnt.z);
                        tfm = glm::translate(idt, trs);
                        geom.genSphere(voxsidelen * 5.0f, 3, 3, tfm);
                    }
                }

    }

    // bind geometry to buffers and return drawing parameters, if possible
    if(geom.bindBuffers(view))
    {
        sdd = geom.getDrawParameters();
        return true;
    }
    else
        return false;
}

Scene::Scene()
{
    csgroot = NULL;
    col = defaultCol;
    voldiag = cgp::Vector(20.0f, 20.0f, 20.0f);
    voxsidelen = 0.0f;
    voxactive = false;
}

Scene::~Scene()
{
    clear();
}

void Scene::clear()
{
    geom.clear();
    vox.clear();

    // This is sufficient to deallocate the tree because the destructor of the root node
    // will cascade down calling the destructor of all branches. I tested this by adding print statements
    // to the destructor methods of the ShapeNode and OpNode classes and verifying that all
    // allocated shapes and operations were altually being deleted.
    delete csgroot;
}

bool Scene::bindGeometry(View * view, ShapeDrawData &sdd)
{
    if(voxactive)
    {
        return genVoxRender(view, sdd);
    }
    else
        return genVizRender(view, sdd);
}

void Scene::voxSetOp(SetOp op, VoxelVolume *leftarg, VoxelVolume *rightarg)
{
    // Get the dimensions of the left VoxelVolume (for use as the loop extents)
    int xdim, ydim, zdim;
    leftarg->getDim(xdim, ydim, zdim);

    // Perform the requested op on the given VoxelVolumes and "return" the result in the leftarg
    if(op == SetOp::UNION){
        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    bool val = (leftarg->get(x, y, z) || rightarg->get(x, y, z));
                    leftarg->set(x, y, z, val);
                }
            }
        }
    }else if(op == SetOp::INTERSECTION){
        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    bool val = (leftarg->get(x, y, z) && rightarg->get(x, y, z));
                    leftarg->set(x, y, z, val);
                }
            }
        }
    }else if(op == SetOp::DIFFERENCE){
        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    bool val = (leftarg->get(x, y, z) && (!rightarg->get(x, y, z)));
                    leftarg->set(x, y, z, val);
                }
            }
        }
    }
}

void Scene::voxWalk(SceneNode *root, VoxelVolume *voxels)
{
    // Do pointContainment test for every shape
    // Traverse the tree and call voxSetOp on each opNode
    if(ShapeNode* shpNode = dynamic_cast<ShapeNode*>(root)){
        // If we have a shapeNode, then voxelise the shape and store the result in voxels

        int xdim, ydim, zdim;
        voxels->getDim(xdim, ydim, zdim);

        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    bool val = (shpNode->shape->pointContainment(voxels->getVoxelPos(x, y, z)));
                    voxels->set(x, y, z, val);
                }
            }
        }
    }else if(OpNode* opNode = dynamic_cast<OpNode*>(root)){
        // If we have an opNode, we need to make recursive voxWalk calls to the left and right subtrees.
        // We then evaluate the op on the left and right voxelVolumes and the result
        // VoxelVolume of the operation is stored in the "voxels" voxelVolume passed in as an arg.

        cgp::Point corner;
        cgp::Vector diag;
        vox.getFrame(corner, diag);

        int xdim, ydim, zdim;
        vox.getDim(xdim, ydim, zdim);
        VoxelVolume* voxR = new VoxelVolume(xdim, ydim, zdim, corner, diag);

        voxWalk(opNode->left, voxels);
        voxWalk(opNode->right, voxR);

        voxSetOp(opNode->op, voxels, voxR);

        delete voxR;
    }
}

void Scene::voxelise(float voxlen)
{
    int xdim, ydim, zdim;

    // calculate voxel volume dimensions based on voxlen
    xdim = ceil(voldiag.i / voxlen)+2; // needs a 1 voxel border to ensure a closed mesh if shapes reach write up to the border
    ydim = ceil(voldiag.j / voxlen)+2;
    zdim = ceil(voldiag.k / voxlen)+2;
    voxsidelen = voxlen;
    voxactive = true;

    cgp::Vector voxdiag = cgp::Vector((float) xdim * voxlen, (float) ydim * voxlen, (float) zdim * voxlen);
    cgp::Point voxorigin = cgp::Point(-0.5f*voxdiag.i, -0.5f*voxdiag.j, -0.5f*voxdiag.k);
    vox.setDim(xdim, ydim, zdim);
    vox.setFrame(voxorigin, voxdiag);

    cerr << "Voxel volume dimensions = " << xdim << " x " << ydim << " x " << zdim << endl;

    // actual recursive depth-first walk of csg tree
    if(csgroot != NULL)
        voxWalk(csgroot, &vox);
    else
        cerr << "CSG tree empty." << endl;
}

void Scene::testSetOpScene(SetOp op){
    // A Sphere in the centre of the world with radius 4
    ShapeNode * sph = new ShapeNode();
    sph->shape = new Sphere(cgp::Point(0.0f, 0.0f, 0.0f), 4.0f);

    // A cylinder going from (-7, -7, 0) to (7, 7, 0) with a radius of 2
    ShapeNode * cyl1 = new ShapeNode();
    cyl1->shape = new Cylinder(cgp::Point(-7.0f, -7.0f, 0.0f), cgp::Point(7.0f, 7.0f, 0.0f), 2.0f);

    // Perform the requested operation on the two nodes
    OpNode * operation = new OpNode();
    operation->op = op;
    operation->left = sph;
    operation->right = cyl1;

    // Set the root of the csg tree to the requested operation
    csgroot = operation;
}

void Scene::testVoxeliseScene(int shapes, SetOp op1, SetOp op2){
    // A sphere in the centre of the world, with radius 4
    ShapeNode * sph = new ShapeNode();
    sph->shape = new Sphere(cgp::Point(0.0f, 0.0f, 0.0f), 4.0f);
    if(shapes == 1){
        csgroot = sph;
        return;
    }

    // A cylinder from (-7, 0, 0) to (7, 0, 0), with radius 2
    ShapeNode * cyl1 = new ShapeNode();
    cyl1->shape = new Cylinder(cgp::Point(-7.0f, 0.0f, 0.0f), cgp::Point(7.0f, 0.0f, 0.0f), 2.0f);

    // Operation between first 2 shapes
    OpNode * operation1 = new OpNode();
    operation1->op = op1;
    operation1->left = sph;
    operation1->right = cyl1;
    if(shapes == 2){
        csgroot = operation1;
        return;
    }

    // A cylinder from (0, -7, 0) to (0, 7, 0), with radius 2
    ShapeNode * cyl2 = new ShapeNode();
    cyl2->shape = new Cylinder(cgp::Point(0.0f, -7.0f, 0.0f), cgp::Point(0.0f, 7.0f, 0.0f), 2.0f);

    // Operation between result of first operation and the third shape (cyl2)
    OpNode * operation2 = new OpNode();
    operation2->op = op2;
    operation2->left = operation1;
    operation2->right = cyl2;

    csgroot = operation2;
}

void Scene::sampleScene()
{
    ShapeNode * sph = new ShapeNode();
    sph->shape = new Sphere(cgp::Point(0.0f, 0.0f, 0.0f), 4.0f);

    ShapeNode * cyl1 = new ShapeNode();
    cyl1->shape = new Cylinder(cgp::Point(-7.0f, -7.0f, 0.0f), cgp::Point(7.0f, 7.0f, 0.0f), 2.0f);

    ShapeNode * cyl2 = new ShapeNode();
    cyl2->shape = new Cylinder(cgp::Point(0.0f, -7.0f, 0.0f), cgp::Point(0.0f, 7.0f, 0.0f), 2.5f);

    OpNode * combine = new OpNode();
    combine->op = SetOp::UNION;
    combine->left = sph;
    combine->right = cyl1;

    OpNode * diff = new OpNode();
    diff->op = SetOp::DIFFERENCE;
    diff->left = combine;
    diff->right = cyl2;

    csgroot = diff;
}

void Scene::expensiveScene()
{
    ShapeNode * sph = new ShapeNode();
    sph->shape = new Sphere(cgp::Point(1.0f, -2.0f, -2.0f), 3.0f);

    ShapeNode * cyl = new ShapeNode();
    cyl->shape = new Cylinder(cgp::Point(-7.0f, -7.0f, 0.0f), cgp::Point(7.0f, 7.0f, 0.0f), 2.0f);

    ShapeNode * mesh = new ShapeNode();
    Mesh * bunny = new Mesh();
    bunny->readSTL("../meshes/bunny.stl");
    bunny->boxFit(10.0f);
    mesh->shape = bunny;

    OpNode * combine = new OpNode();
    combine->op = SetOp::UNION;
    combine->left = mesh;
    combine->right = cyl;

    OpNode * diff = new OpNode();
    diff->op = SetOp::DIFFERENCE;
    diff->left = combine;
    diff->right = mesh;

    csgroot = diff;
}
