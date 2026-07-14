# Vulkan Engine Written in C

- - - 

My primarily Graphics (but also for Games) Engine that is currently in development. It's being created because everytime
I want to do something interesting in C/C++, I find myself needing a graphics renderer of some kind. So I decided to
write my own in Vulkan.

### Features:

- Platform Layer - Windows and Linux
- Handwritten Data Structures
    - DSA Types: Array, Dynamic Arrays, Linear/Stack/Pool/Free-List Allocators, Linked List, Binary Tree, Stack, Ring
      Queue. Strings, String Builders, C_Strings
    - All structures know how to seralized and deserialize themselves.
    - C doesn't provide nice things like std::vector<>, so you have to write them yourself, however, it's also a greate
      opportunity to learn how these things are created in the first place. It also means there are no hidden/unknown
      allocations happening like might happen with a std::vector<> resize.
- Memory System
- Unit Tests and Custom Loggers/Asserts.
- File and network serialization code.
- Resource System: Meshes, Sprites, UI Sprites
- Immediate Mode UI
- Runtime Reflection System and Reflection Registry
- GLTF File Loading (using cgltf)
- Renderer
    - Bling Phong Shading
    - Mesh/Skeletal Mesh Rendering
    - Material System and Material Batching
    - UI Rendering
    - Sprite Rendering
    - Pipeline Cache
- Editor
    - Engine stats (ms timing per frame)
    - Object Transforms editing
    - Texture Viewer

### Current Todo's:

- General
    - Extend Animation API
    - Game Engine Specific Format
    - Level File Format
- Multithreading
    - multi-thread the resource system
    - multi-thread the renderer
- Renderer
    - PBR
    - Shadows
    - Instancing
    - Compute Shaders
    - Culling
- Particle System
- Work on the Linux Platform Layer
- Editor
    - Level Editor
    - Asset Editor
- Game
  - File Format for Abilties
  - Integrate Asset Handles for game to renderer/resource manager communication
  - Expand enemy AI functionality
  - Game specific UI System (Current UI system is simple and only meant for the editor)

### Extra Info:

- It's called Madness Engine cause the first game I plan to make with it will be called Pulse of Madness. It will be a
  3d turn based game.
- Why not use C++ - I have nothing against C++ but I found that it was making me a worse programmer because I was
  spending more time on language features, and less time on architecture and problem-solving. Using C gives me a much
  more forced and constrained environment. It forces me to use only the bare essentials of what c++ would give you and
  nothing else. In hindsight now, I also feel that C has made me understand memory management and low-levels concepts
  much better, since they are far more explicit than in C++. However, there are features I wish c had such as templates,
  operator overloading for math types and default values for variables, enum classes, better casting and type safety.  


