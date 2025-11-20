a voxel engine built for just for fun :)

roadmap:
- Procedural chunk generation
    [ ] Load/generate chunks as you move through the world. Unload invisible chunks.
    [ ] Store chunks efficiently in memory and serialize/deserize them to a save file
- Player interaction
    [ ] Place/remove blocks
    [ ] Place different block types (also things like flowers)
    [x] Highlight the current block the player's aiming at
- Phyiscs
    [x] Player movement, jumping (with gravity)
    [x] Collision detection with voxels
    [x] Offset's the camera's AABB box (since the player's head is higher than their body)
- Culling
    [x] Face culling: Don't show voxel faces that are occluded
    [ ] Frustum culling: Only draw chunks and voxels that are actually visible
    [ ] Reduce mesh vertices for chunks that are far away
- Lighting
    [ ] Ambient occlusion
    [ ] Voxel lighting
- Data structures
    [ ] Use octrees to store voxels and chunks
- Multithreading
    [ ] Multithreaded chunk generation
    [ ] Multithreaded chunk mesh generation
