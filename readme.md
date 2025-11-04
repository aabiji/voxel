Voxel engine

TODO:
Misc:
    - Basic UI for selecting blocks to place
    - Our own version of glm (math for vectors, matrices, projection, quaternions, etc)
- Physics
    - AABB Collision with blocks (don't clip them or fall through)
- Rendering
    - Nice lighting
        - Cascaded shadow maps
    - Nice water shading
    - Transparency sorting for nice water/glass rendering
    - Instanced rendering for things like flowers
    - Ambient occlusion
- Chunking system
    - Multithreaded
    - Terrain generation using Perlin Noise
    - Load/unload chunks as the player moves
    - Different biomes
    - Save and load worlds
    - Compress chunk data in memory or when saving??
- Blocks
    - Glass and water block types
    - Also support objects that aren't cubes (ex: flowers)
    - Read block textures from a texture atlas
    - Placing blocks
    - Removing blocks, with a nice removing animation
- Optimizations
    - Frustum culling
    - Occlusion culling
    - Level of detail

Priorities:
- Must be *fast*
- Built for learning purposes (really understand what's going on)
