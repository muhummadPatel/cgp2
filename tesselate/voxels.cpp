//
// Voxels
//

#include "voxels.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <limits>

using namespace std;

VoxelVolume::VoxelVolume()
{
    xdim = ydim = zdim = 0;
    voxgrid = NULL;
    setFrame(cgp::Point(0.0f, 0.0f, 0.0f), cgp::Vector(0.0f, 0.0f, 0.0f));
}

VoxelVolume::VoxelVolume(int xsize, int ysize, int zsize, cgp::Point corner, cgp::Vector diag)
{
    voxgrid = NULL;
    setDim(xsize, ysize, zsize);
    setFrame(corner, diag);
}

VoxelVolume::~VoxelVolume()
{
    clear();
}

void VoxelVolume::clear()
{
    if(voxgrid != NULL)
    {
        delete [] voxgrid;
        voxgrid = NULL;
    }
}

void VoxelVolume::fill(bool setval)
{
    for(int i = 0; i < grid_len; i++){
        if(setval){
            voxgrid[i] = ~0u;
        }else{
            voxgrid[i] = 0u;
        }
    }
}

void VoxelVolume::calcCellDiag()
{
    if(xdim > 0 && ydim > 0 && zdim > 0)
        cell = cgp::Vector(diagonal.i / (float) xdim, diagonal.j / (float) ydim, diagonal.k / (float) zdim);
    else
        cell = cgp::Vector(0.0f, 0.0f, 0.0f);
}

void VoxelVolume::getDim(int &dimx, int &dimy, int &dimz)
{
    dimx = xdim; dimy = ydim; dimz = zdim;
}

void VoxelVolume::setDim(int &dimx, int &dimy, int &dimz)
{
    // Clear the voxgrid and update it's dimensions
    clear();
    xdim = dimx; ydim = dimy; zdim = dimz;

    // allocate the required memory and fill it with 0's
    int total_bits = xdim * ydim * zdim;
    int bits_per_int = sizeof(int) * 8;
    grid_len = (int)(((1.0 * total_bits) / bits_per_int) + 0.5);

    voxgrid = new int[grid_len];

    fill(false);

    calcCellDiag();
}

void VoxelVolume::getFrame(cgp::Point &corner, cgp::Vector &diag)
{
    corner = origin;
    diag = diagonal;
}

void VoxelVolume::setFrame(cgp::Point corner, cgp::Vector diag)
{
    origin = corner;
    diagonal = diag;
    calcCellDiag();
}

bool VoxelVolume::set(int x, int y, int z, bool setval)
{
    // Figure out which bit in which int we need to set
    int bit_to_set = (xdim * ydim * z) + (zdim * y) + x;
    int voxel_index = bit_to_set / (sizeof(int) * 8.0);
    int offset = bit_to_set % (sizeof(int) * 8);

    int* v = &voxgrid[voxel_index];
    if(setval){
        *v |= (1u << offset); // Turn the bit on
    }else{
        *v &= ~(1u << offset); // Switch the bit off
    }

    return true;
}

bool VoxelVolume::get(int x, int y, int z)
{
    // Which bit in which int do we need to get
    int bit_to_set = (xdim * ydim * z) + (ydim * y) + x;
    int voxel_index = bit_to_set / (sizeof(int) * 8.0);
    int offset = bit_to_set % (sizeof(int) * 8);

    int v = voxgrid[voxel_index];
    bool bit = (v >> offset) & 1u; // Extract the requested bit from the int
    return bit;
}

cgp::Point VoxelVolume::getVoxelPos(int x, int y, int z)
{
    cgp::Point pnt;
    cgp::Vector halfcell;
    float px, py, pz;

    px = (float) x / (float) xdim;
    py = (float) y / (float) ydim;
    pz = (float) z / (float) zdim;

    pnt = cgp::Point(origin.x + px * diagonal.i + 0.5f * cell.i, origin.y + py * diagonal.j + 0.5f * cell.j, origin.z + pz * diagonal.k + 0.5f * cell.k); // convert from voxel space to world coordinates
    return pnt;
}
