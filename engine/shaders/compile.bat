glslc.exe imgui.vert -o compiledShaders/imguiVert.spv
glslc.exe imgui.frag -o compiledShaders/imguiFrag.spv
glslc.exe empty.vert -o compiledShaders/emptyVert.spv
glslc.exe empty.frag -o compiledShaders/emptyFrag.spv
glslc.exe grid.vert  -o compiledShaders/gridVert.spv
glslc.exe grid.frag  -o compiledShaders/gridFrag.spv
glslc.exe xAxis.vert -o compiledShaders/xAxisVert.spv
glslc.exe xAxis.frag -o compiledShaders/xAxisFrag.spv
glslc.exe yAxis.vert -o compiledShaders/yAxisVert.spv
glslc.exe yAxis.frag -o compiledShaders/yAxisFrag.spv
glslc.exe zAxis.vert -o compiledShaders/zAxisVert.spv
glslc.exe zAxis.frag -o compiledShaders/zAxisFrag.spv
glslc.exe unlit.vert -o compiledShaders/unlitVert.spv
glslc.exe unlit.frag -o compiledShaders/unlitFrag.spv
glslc.exe uv.vert -o compiledShaders/uvVert.spv
glslc.exe uv.frag -o compiledShaders/uvFrag.spv

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