@REM glslc.exe imgui.vert -o compiledShaders/imguiVert.spv
@REM glslc.exe imgui.frag -o compiledShaders/imguiFrag.spv
glslc.exe shader.vert -o compiledShaders/vert.spv
glslc.exe shader.frag -o compiledShaders/frag.spv
glslc.exe shader2.vert -o compiledShaders/vert2.spv
glslc.exe shader2.frag -o compiledShaders/frag2.spv

@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe imgui.vert -o ../Binaries/windows-x86_64/Release/Shaders/compiledShaders/imguiVert.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe imgui.frag -o ../Binaries/windows-x86_64/Release/Shaders/compiledShaders/imguiFrag.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader.vert -o ../Binaries/windows-x86_64/Release/Shaders/compiledShaders/vert.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader.frag -o ../Binaries/windows-x86_64/Release/Shaders/compiledShaders/frag.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader2.vert -o ../Binaries/windows-x86_64/Release/Shaders/compiledShaders/vert2.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader2.frag -o ../Binaries/windows-x86_64/Release/Shaders/compiledShaders/frag2.spv

@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe imgui.vert -o ../Binaries/windows-x86_64/Debug/Shaders/compiledShaders/imguiVert.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe imgui.frag -o ../Binaries/windows-x86_64/Debug/Shaders/compiledShaders/imguiFrag.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader.vert -o ../Binaries/windows-x86_64/Debug/Shaders/compiledShaders/vert.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader.frag -o ../Binaries/windows-x86_64/Debug/Shaders/compiledShaders/frag.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader2.vert -o ../Binaries/windows-x86_64/Debug/Shaders/compiledShaders/vert2.spv
@REM C:/VulkanSDK/1.3.283.0/Bin/glslc.exe shader2.frag -o ../Binaries/windows-x86_64/Debug/Shaders/compiledShaders/frag2.spv

pause