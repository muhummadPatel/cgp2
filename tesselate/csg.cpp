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

    // TO DO HERE, traverse csg tree pushing leaf nodes (shapes) to leaves vector
    // note: this displays all the constituent shapes in the tree but doesn't apply any set operations to them
    // so it is purely a pre-visualization
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

    // TO DO HERE, code to walk csg tree and deallocate nodes
    // will require dynamic casting of SceneNode pointers
    // TODO Why does it look like this never gets called?
    cerr << "!!!***><deleting><***!!!" << endl;
    if(csgroot != NULL){
        std::vector<SceneNode*> to_delete;
        to_delete.push_back(csgroot);

        while(!to_delete.empty()){
            SceneNode* curr = to_delete[to_delete.size()-1];
            to_delete.erase(to_delete.begin() + (to_delete.size()-1));

            if(OpNode* op = dynamic_cast<OpNode*>(curr)){
                to_delete.push_back(op->right);
                to_delete.push_back(op->left);
            }

            delete curr;
        }
    }
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
    // stub, needs completing

    int xdim, ydim, zdim;
    leftarg->getDim(xdim, ydim, zdim);

    // Perform the op on the given VoxelVolumes and "return" the result in VoxelVolumes
    if(op == SetOp::UNION){
        cerr << "UNION" << endl;
        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    if(leftarg->get(x, y, z) || rightarg->get(x, y, z)){
                        leftarg->set(x, y, z, true);
                    }else{
                        leftarg->set(x, y, z, false);
                    }
                }
            }
        }
    }else if(op == SetOp::INTERSECTION){
        cerr << "INTERSECTION" << endl;
        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    if(leftarg->get(x, y, z) && rightarg->get(x, y, z)){
                        leftarg->set(x, y, z, true);
                    }else{
                        leftarg->set(x, y, z, false);
                    }
                }
            }
        }
    }else if(op == SetOp::DIFFERENCE){
        cerr << "DIFFERENCE" << endl;
        cerr << "UNION" << endl;
        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    if(leftarg->get(x, y, z) && (!rightarg->get(x, y, z))){
                        leftarg->set(x, y, z, true);
                    }else{
                        leftarg->set(x, y, z, false);
                    }
                }
            }
        }
    }
}

void Scene::voxWalk(SceneNode *root, VoxelVolume *voxels)
{
    // stub, needs completing
    // will require dynamic casting of SceneNode pointers

    // Do pointContainment test for every shape
    // Traverse the tree and call voxSetOp on each opNode from the bottom up
    cerr << "Walking" << endl;
    if(ShapeNode* shpNode = dynamic_cast<ShapeNode*>(root)){
        int xdim, ydim, zdim;
        voxels->getDim(xdim, ydim, zdim);

        for(int x = 0; x < xdim; x++){
            for(int y = 0; y < ydim; y++){
                for(int z = 0; z < zdim; z++){
                    if(shpNode->shape->pointContainment(voxels->getVoxelPos(x, y, z))){
                        voxels->set(x, y, z, true);
                    }else{
                        voxels->set(x, y, z, false);
                    }
                }
            }
        }

        cerr << "ShapeNode" << endl;
    }else if(OpNode* opNode = dynamic_cast<OpNode*>(root)){
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
