versionName=$(basename "$(find $HOME/vulkan -maxdepth 1 -type d ! -path /home/amenosora/vulkan | head -n 1)")
/$HOME/vulkan/$versionName/x86_64/bin/glslc ./Shaders/vertexShader.vert -o ./Shaders/output/vert.spv
/$HOME/vulkan/$versionName/x86_64/bin/glslc ./Shaders/fragmentShader.frag -o ./Shaders/output/frag.spv