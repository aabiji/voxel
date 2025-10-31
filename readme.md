Voxel engine

Roadmap:
Misc:
    - Custom memory allocator to avoid fragmentation
    - Basic UI for selecting blocks to place
    - Our own version of glm (math for vectors, matrices, projection, quaternions, etc)
- Physics
    - 3D camera movement
    - 3D player movement
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
    - Greedy meshing
    - Face culling
    - Frustum culling
    - Occlusion culling
    - Level of detail
    - Optionally moving greedy meshing and terrain generation to compute shaders?

Priorities:
- Must be *fast*
- Written in zig to learn the language
- Built for learning purposes (really understand what's going on)
