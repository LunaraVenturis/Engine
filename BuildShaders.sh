versionName=$(basename "$(find $HOME/vulkan -maxdepth 1 -type d ! -path $HOME/vulkan | head -n 1)")
/$HOME/vulkan/$versionName/x86_64/bin/glslc ./Assets/Shaders/vertexShader.vert -o ./Assets/Shaders/output/vert.spv
/$HOME/vulkan/$versionName/x86_64/bin/glslc ./Assets/Shaders/fragmentShader.frag -o ./Assets/Shaders/output/frag.spv