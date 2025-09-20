#!/usr/bin/env python3

import os
import subprocess
import shutil
import sys
from pathlib import Path

def get_version_name(vulkan_path):
    """Get the version name of Vulkan by finding the first directory in the vulkan directory."""
    vulkan_dir = Path(vulkan_path)
    dirs = [d for d in vulkan_dir.iterdir() if d.is_dir()]
    dirs.sort()  # Assumes the version is sorted alphabetically
    return dirs[0].name if dirs else None

def compile_shader(project_root, shader_name, version_name):
    """Compile a single shader using Vulkan's glslc."""
    shader_base = shader_name.split('.')[0]  # Remove extension
    print(f"🎯 Compiling shader stages for {shader_base}...")
    
    glslc_path = Path(f"{project_root}/Vendor/vulkan/{version_name}/x86_64/bin/glslc")
    vertex_shader = Path(project_root + f"/Assets/Shaders/{shader_base}.vert")
    fragment_shader = Path(project_root + f"/Assets/Shaders/{shader_base}.frag")
    vertex_output = Path(project_root + f"/Assets/Shaders/bin/{shader_base}.vert.spv")
    fragment_output = Path(project_root + f"/Assets/Shaders/bin/{shader_base}.frag.spv")
    
    subprocess.run([str(glslc_path), "-O", str(vertex_shader), "-o", str(vertex_output)])
    subprocess.run([str(glslc_path), "-O", str(fragment_shader), "-o", str(fragment_output)])

def compile_shaders(project_root, version_name):
    """Compile all shaders in the project root."""
    bin_dir = Path(project_root + "/Assets/Shaders/bin")
    bin_dir.mkdir(parents=True, exist_ok=True)

    print("🛠️ Starting shader compilation...")
    shader_files = list(Path(project_root + "/Assets/Shaders").glob("*.frag")) + list(Path(project_root + "/Assets/Shaders").glob("*.vert"))
    for shader_file in shader_files:
        if shader_file.is_file():
            print(f"🔹 Compiling {shader_file.name}")
            compile_shader(project_root, shader_file.name, version_name)
    print("✅ Done compiling shaders!")

def copy_assets(project_root):
    """Copy assets to the build and Sandbox directories."""
    print("📋 Copying assets...")
    assets_dst = Path(project_root + "/build/bin/Assets")

    if assets_dst.exists() and assets_dst.is_dir():
        shutil.rmtree(assets_dst)
    

    shutil.copytree(Path(project_root + "/Assets"), assets_dst)
    # shutil.copytree(Path(project_root + "/Assets"), sandbox)
    print("🎉 Done copying assets")

def main():
    if len(sys.argv) < 2:
        print("⚠️ Usage: python setup.py /path/to/project_root")
        sys.exit(1)

    project_root = sys.argv[1]
    vulkan_path = project_root + "/Vendor" + "/vulkan"
    version_name = get_version_name(vulkan_path)

    if not version_name:
        print("⚠️ Vulkan version not found.")
        sys.exit(1)

    print("🚀 Compiling shaders")

    # List shader files
    shader_files = list(Path(project_root + "/Assets/Shaders").glob("*.frag")) + list(Path(project_root + "/Assets/Shaders").glob("*.vert"))
    if not shader_files:
        print("No shader files found.")
    else:
        for file in shader_files:
            print(f"📂 Found shader file: {file.name}")
    
    compile_shaders(project_root, version_name)
    copy_assets(project_root)

if __name__ == "__main__":
    main()
