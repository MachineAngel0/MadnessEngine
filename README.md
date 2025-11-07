# Vulkan Engine Written in C

- - - 

My primarily Graphics (but also for Games) Engine that is currently in development. It's being created because everytime I want to do something interesting in C/C++, I find myself needing a graphics renderer of some kind. So I decided to write my own in Vulkan. 


### Features:
- Platform Layer - Windows and Linux
- Handwritten Data Structures
    - DSA Types: Array, Dynamic Arrays, Linear/Stack/Pool/Free-List Allocators, Linked List, Binary Tree, Stack, Ring
      Queue,
    - C doesn't provide nice things like std::vector<>, so you have to write them yourself, however, it's also a greate
      opportunity to learn how these things are created in the first place. It also means there are no hidden/unknown
      allocations happening like might happen with a std::vector<> resize.
    - Math Library: Vec2,3,4
- Unit Tests and Custom Loggers/Asserts


### Current Todo's:
- Finish setting up a more robust renderer
- Unit Tests for the Data Structure's Folder
- Hookup the Memory Logger
- Work on the Linux Platform Layer

### PLANNED:
- Simple Flex Style Immediate Mode UI
- Editor for easy development


### Fun Fact:
- It's called Madness Engine cause the first game I plan to make with it will be called Pulse of Madness. It will be a
  3d turn based game.
- Why not use C++ - I have nothing against C++ but I found that it was making me a worse programmer because I was spending more time on language features, and less time on architecture and problem-solving. Using C gives me a much more forced and constrained environment. It forces me to use only the things available and nothing else. In that sense I can't abuse any language features, which may not be a good fit for my project. 


