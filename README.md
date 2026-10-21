# Vulkan Engine Written in C

- - - 

My primarily Graphics (but also for Games) Engine that is currently in development. It's being created because everytime
I want to do something interesting in C/C++, I find myself needing a graphics renderer of some kind. So I decided to
write my own in Vulkan.

### Features:

- Platform Layer - Windows and Linux (No plans for MacOS)
- Handwritten Data Structures
    - DSA Types: Array, Dynamic Arrays, Linear/Stack/Pool/Free-List Allocators, Linked List, Binary Tree, Stack, Ring
      Queue. Strings, String Builders, C_Strings
    - All structures know how to seralized and deserialize themselves.
    - C doesn't provide nice things like std::vector<>, so you have to write them yourself, however, it's also a greate
      opportunity to learn how these things are created in the first place. It also means there are no hidden/unknown
      allocations happening like might happen with a std::vector<> resize.
- Memory System (w/Custom Allocators)
- Unit Tests
- Custom Logging and Asserts.
- Asset System (and Engine specific Formats): Meshes, Skinned Meshes, Sprites, UI Sprites, Particles, Materials, Textures
- Custom Immediate Mode UI For Gameplay
- Runtime Reflection System
- GLTF File Loading (using cgltf)
- Tracy Profiler
- ImGui For the Editor
- Renderer
    - GPU Driven Rendering (Indirect Draw, Instancing, SSBO's, vertex pulling)
    - Bling Phong Shading
    - Asyc Transfer Queue Mesh and Texture Data Upload
    - Mesh/Skeletal Mesh Rendering
    - Material System
    - Particle Rendering
    - UI Rendering
    - Sprite Rendering
    - Pipeline Caching
- Editor
    - Engine stats (ms timing per frame)
    - Asset Editor (Meshes, Particles, Mateiral, Texture)
    - Scene View and Editor

### Current Todo's:

- General
    - Extend Animation API
    - Level File Format
- Multithreading
    - multi-thread and make asset loading async
    - multi-thread the renderer
    - multi-thread the particle simulation (or move to compute shaders)
- Renderer
    - PBR
    - Shadows
    - Instancing
    - Compute Shaders
      - skinning
      - potentially particle system
    - Culling
- Work on the Linux Platform Layer
- Editor
    - Scene Editor
      - 3d widget for transformations
      - Add and Delete Objects
      - Per Object Material Overrides
      - Scene Serialization
- Game
  - File Format for Abilties (I plan to have a lot of unique abilities probably 500+)
  - Expand enemy AI functionality
  - Expand UI System to Handle Custom Mateirals

### Extra Info:

- It's called Madness Engine cause the first game I plan to make with it will be called Pulse of Madness. It will be a
  3d turn based game.
- Why not use C++ - I have nothing against C++ but I found that it was making me a worse programmer because I was
  spending more time on language features, and less time on architecture and problem-solving. Using C gives me a much
  more forced and constrained environment. It forces me to use only the bare essentials of what c++ would give you and
  nothing else. In hindsight now, I also feel that C has made me understand memory management and low-levels concepts
  much better, since they are far more explicit than in C++. However, there are features I wish c had such as templates,
  operator overloading for math types and default values for variables, enum classes, better casting and type safety.  


