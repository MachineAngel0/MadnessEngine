@echo off

:: scans all directories and subdirectories and creates .spirv file for each the frag and vert files

:: runs in the directory it is located in, not where it was called from
cd /d %~dp0

forfiles /s /m *.vert /c "cmd /c %VULKAN_SDK%/Bin/glslangValidator.exe @path -gVS -V -o @fname.vert.spv"
forfiles /s /m *.frag /c "cmd /c %VULKAN_SDK%/Bin/glslangValidator.exe @path -gVS -V -o @fname.frag.spv"

:: /s means all folders and sub folders
::  /m *.frag match the type
::  /c run for each file.
::  cmd /c execute command
:: - 0 name of output file
:: -gVS vulkan debug info // -gVS generate nonsemantic shader debug information with source
::  -V[ver] create SPIR-V binary

pause
