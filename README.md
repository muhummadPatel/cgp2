##Assignment 2: Converting Constructive Solid Geometry Trees to a Voxel Representation

**Author** : Muhummad Yunus Patel  
**Student#** : PTLMUH006  
**Date** : 06-March-2016

**NOTE:** Should you require any clarification, please feel free to contact me
 at muhummad.patel@gmail.com. Thank you. :)

###Description:
This practical implements code to convert constructive solid geometry (csg) trees
to a voxel representation of the result. The tree consists of shapeNodes and opNodes.
The tree is evaluated by walking through and voxelising any shapes encountered and
performing any operations found. At the end of this traversal, there will be a single
VoxelVolume left representing the result of applying all the operations on the shapes in the
csg tree.

In order to support this voxelising of the csg tree, the VoxelVolume class was also implemented.
The VoxelVolume voxgrid was implemented as a flattened bit-packed int array. All operations
were then done using bitwise operators. Voxel operations implemented include get, set,
fill, clear, and setDim(to allocate the required memory).

The added code was thoroughly unit tested in the test_scene.h and test_scene.cpp files.
The tests check that all bit operations and set operations of voxelvolumes are correctly evaluated.
(i.e. UNION, INTERSECTION, and DIFFERENCE, should all return the correct VoxelVolume)
Furthermore, tests were included to check that more interesting csg trees were correctly traversed
and evaluated.

###Compiling and Running the Solution:
* See instructions in InstallReadme.txt for instructions on setting up and compiling the project.
* To run the the Tesselate program:
    * First set up and build the project (see previous point).
    * Navigate to the build directory
    * Run ./tesselate/tessviewer
    * Tick the 'show scene' checkbox in the left panel
    * Click the 'Voxelise' button in the left panel
    * After a bit, you should see a voxel representation of the sample scene in the viewing area
* The unit tests will be automatically run when you build the project. (see InstallReadme.txt for instructions on setting up and building the project)
