#!/bin/bash

versionName=$(basename "$(find "$HOME/vulkan" -maxdepth 1 -type d ! -path "$HOME/vulkan" | head -n 1)")

compile_shader() {
    local project_root="$1"
    local shader_name="$2"
    local shader_base="${shader_name%.*}"  # Remove extension

    echo "🎯 Compiling shader stages for $shader_base..."
    "$HOME/vulkan/$versionName/x86_64/bin/glslc" -O "$project_root/Assets/Shaders/$shader_base.vert" -o "$project_root/Assets/Shaders/bin/$shader_base.vert.spv"
    "$HOME/vulkan/$versionName/x86_64/bin/glslc" -O "$project_root/Assets/Shaders/$shader_base.frag" -o "$project_root/Assets/Shaders/bin/$shader_base.frag.spv"
}

compile_shaders() {
    local project_root="$1"
    mkdir -p "$project_root/Assets/Shaders/bin"

    echo "🛠️ Starting shader compilation..."
    for file in "$project_root/Assets/Shaders/"*.{frag,vert}; do
        if [ -f "$file" ]; then
            shader_file="$(basename "$file")"
            echo "🔹 Compiling $shader_file"
            compile_shader "$project_root" "$shader_file"
        fi
    done

    echo "✅ Done compiling shaders!"
}

echo "🚀 Compiling shaders"

if [ -z "$1" ]; then
    echo "⚠️ Usage: $0 /path/to/project_root"
    exit 1
fi

echo "📂 Files found:"
ls "$1/Assets/Shaders/"*.{frag,vert} 2>/dev/null || echo "No shader files found."

compile_shaders "$1"

echo "📋 Copying assets"
if [ -d "$1/build/bin/Assets" ]; then
rm -rf "$1/build/bin/Assets"
fi
if [ -d "$1/Sandbox/Assets" ]; then
rm -rf "$1/Sandbox/Assets"
fi
cp -r "$1/Assets" "$1/build/bin/"
cp -r "$1/Assets" "$1/Sandbox/"
echo "🎉 Done copying assets"
