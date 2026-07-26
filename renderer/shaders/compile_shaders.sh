#!/usr/bin/env bash
# scans all directories and subdirectories and creates .spv files for each frag and vert file
# runs in the directory it is located in, not where it was called from

cd "$(dirname "$0")" || exit 1

# /s = recursive, /m *.vert = match type, /c = run for each file
# -g = generate debug info
# -V = create SPIR-V binary

find . -type f -name "*.vert" -exec bash -c '
    file="$1"
    fname="$(basename "${file%.*}")"
    "$VULKAN_SDK/bin/glslangValidator" "$file" -gVS -V -o "${fname}.vert.spv"
' _ {} \;

find . -type f -name "*.frag" -exec bash -c '
    file="$1"
    fname="$(basename "${file%.*}")"
    "$VULKAN_SDK/bin/glslangValidator" "$file" -gVS -V -o "${fname}.frag.spv"
' _ {} \;

read -n 1 -s -r -p "Press any key to continue..."
echo