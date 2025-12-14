@echo off

:: spirv-dis shader_mesh.vert.spv -o ../debug_spriv/shader_mesh.vert.txt

forfiles /s /m *.spv /c "cmd /c %VULKAN_SDK%/Bin/spirv-dis.exe @path -o ../debug_spriv/@fname.spv.txt"

pause
