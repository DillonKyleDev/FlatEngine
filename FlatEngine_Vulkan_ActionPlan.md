# FlatEngine Vulkan — Comprehensive Improvement Action Plan

This document covers every significant structural, correctness, and performance issue found across all reviewed source files. Each item explains **what Vulkan concept is involved**, **where the problem is**, **what the current code does**, and **what it should do instead**.

---

## Table of Contents

1. [Critical Bugs](#1-critical-bugs)
2. [Memory and Resource Management](#2-memory-and-resource-management)
3. [Structural / Architectural Issues](#3-structural--architectural-issues)
4. [Performance Issues](#4-performance-issues)
5. [Code Duplication](#5-code-duplication)
6. [Threading](#6-threading)
7. [Minor Correctness Issues](#7-minor-correctness-issues)
8. [Future Features Worth Planning For](#8-future-features-worth-planning-for)

---

## 1. Critical Bugs

These are issues that either cause incorrect GPU behavior silently, will crash under certain conditions, or produce wrong rendering results.

---

### 1.1 — Double Assignment of `srcStageMask` in ImGui RenderPass

**File:** `VulkanManager.cpp` → `CreateImGuiRendePassResources()`

**Vulkan concept:** Subpass dependencies tell the GPU when it's safe to move from one render pass stage to another. `srcStageMask` says "wait until this pipeline stage in the previous pass is done." If you assign it twice, only the second value takes effect. The first one is silently discarded — C++ has no warning for this.

**Current code:**
```cpp
dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
```

The first line is immediately overwritten. This also appears in the commented-out `SetDefaultRenderPassConfig()` and `CreateSceneRenderPassResources()` in `RenderPass.cpp`, so it was a copy-paste pattern.

**Fix:**
```cpp
dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
                         | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
```

---

### 1.2 — `BeginRenderPass` Stores a Dangling Pointer in a Member

**File:** `RenderPass.cpp` → `BeginRenderPass()`

**Vulkan concept:** `VkRenderPassBeginInfo.pClearValues` must point to valid memory at the time `vkCmdBeginRenderPass` is called. After the call it is not used again — but only because `vkCmdBeginRenderPass` is called in the same function.

**The problem:** `m_renderPassInfo` is a class member. `clearValues` is a local `std::vector` allocated on the stack. After `BeginRenderPass()` returns, `m_renderPassInfo.pClearValues` points to freed memory. If anything ever reads `m_renderPassInfo` again after the function returns (e.g. future refactoring, serialization, debugging), it will read garbage or crash.

**Current code:**
```cpp
void RenderPass::BeginRenderPass(uint32_t imageIndex)
{
    std::vector<VkClearValue> clearValues;
    // ... fills clearValues ...
    m_renderPassInfo.pClearValues = clearValues.data(); // pointer to local stack data
    vkCmdBeginRenderPass(..., &m_renderPassInfo, ...);
    // clearValues is destroyed here — m_renderPassInfo.pClearValues is now dangling
}
```

**Fix:** Either make `clearValues` a member variable, or better, keep it purely local and pass a local `VkRenderPassBeginInfo` to `vkCmdBeginRenderPass` instead of using `m_renderPassInfo`:
```cpp
void RenderPass::BeginRenderPass(uint32_t imageIndex)
{
    std::vector<VkClearValue> clearValues;
    // ... fill clearValues ...

    VkRenderPassBeginInfo renderPassInfo = m_renderPassInfo; // copy base settings
    renderPassInfo.framebuffer = m_framebuffers[VM_currentFrame];
    renderPassInfo.renderArea.extent = m_winSystem->GetExtent();
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data(); // safe: local, used immediately

    vkCmdBeginRenderPass(m_commandBuffers[VM_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}
```

---

### 1.3 — Framebuffer Attachment Order Does Not Match RenderPass Attachment Order

**File:** `RenderPass.cpp` → `CreateFrameBuffers()`

**Vulkan concept:** A `VkFramebuffer` holds a list of `VkImageView` attachments. The order of these attachments **must exactly match** the order of the `VkAttachmentDescription` entries in the `VkRenderPass`. Mismatch = undefined behavior, which often manifests as black screens, incorrect depth testing, or GPU crashes.

In `CreateRenderToTextureRenderPassResources()` (VulkanManager.cpp), the render pass attachments are added in this order:
1. Color attachment (index 0)
2. Depth attachment (index 1)
3. Resolve attachment (index 2)

But in `CreateFrameBuffers()`, the attachments are assembled as:
```cpp
if (m_b_msaaEnabled)      attachments.push_back(m_colorImageView);  // MSAA color
if (m_b_depthBuffersEnabled) attachments.push_back(m_depthTexture); // depth
attachments.push_back(m_imageViews[i]);                              // resolve/final
```

This happens to work when MSAA and depth are both enabled, but if you ever disable one or change the order in the render pass setup, this will silently break. The attachment building should be driven by named constants or a struct that keeps the RenderPass definition and Framebuffer creation in sync.

**Fix (long term):** Define the attachment order in one place — ideally as an enum or array that both `CreateRenderPass` and `CreateFrameBuffers` reference:
```cpp
enum AttachmentIndex { COLOR = 0, DEPTH = 1, RESOLVE = 2 };
```
Then use those indices explicitly when building both the render pass and framebuffer to make the relationship visible and safe.

---

### 1.4 — `CreateTextureImage` Takes `VkDeviceMemory` by Value, Not Reference

**File:** `WinSys.cpp` → `CreateTextureImage()` and `WinSys.h`

**Vulkan concept:** `VkDeviceMemory` is a handle (essentially a 64-bit integer). `vkAllocateMemory` fills it via a pointer. If you pass `textureImageMemory` by value, the allocation happens on a local copy that is destroyed when the function returns — the caller's handle is never updated, so the memory is allocated but unreachable, and will be leaked.

**Current signature in WinSys.h:**
```cpp
VkImage CreateTextureImage(std::string path, uint32_t mipLevels, VkDeviceMemory textureImageMemory, VkCommandPool& commandPool);
```

The parameter is passed by value. Inside `CreateImage()`, it passes a pointer to this local copy. The caller's `VkDeviceMemory` is never written.

**Fix:**
```cpp
VkImage CreateTextureImage(std::string path, uint32_t mipLevels, VkDeviceMemory& textureImageMemory, VkCommandPool& commandPool);
```
Add `&`. Both the header and implementation need this change. Verify that all callers (in `VulkanManager.cpp`) are also correctly storing the returned memory handle.

---

## 2. Memory and Resource Management

---

### 2.1 — Uniform Buffers Are Allocated Per-Mesh, Per-Frame Using `vkAllocateMemory` Directly

**File:** `Mesh.cpp` → `CreateUniformBuffers()`

**Vulkan concept:** Vulkan has a hard limit on the number of simultaneous `vkAllocateMemory` calls (`maxMemoryAllocationCount`, often as low as 4096). Each mesh currently makes `VM_MAX_FRAMES_IN_FLIGHT * 2` allocations (scene view + game view). At 100 meshes, you're making ~400 allocations just for UBOs.

The Vulkan spec and the tutorial you cited both warn about this (your code even has the comment: *"you're not supposed to actually call vkAllocateMemory for every individual buffer"*).

**Fix:** Use a single large `VkBuffer` with sub-allocations for all UBOs, or use a library like [VMA (Vulkan Memory Allocator)](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator). VMA is the industry standard solution here. It handles pooling, alignment, and defragmentation. Integration is a header-only include plus replacing `vkAllocateMemory` / `vkCreateBuffer` calls with `vmaCreateBuffer`.

---

### 2.2 — Vertex Deduplication Is Commented Out for Triangle Meshes

**File:** `Model.cpp` → `LoadModel()`

**Vulkan concept:** Vertex buffers sent to the GPU should not contain duplicate vertices. When you have a mesh where multiple triangles share a corner, the same position/normal/UV data gets uploaded multiple times. The index buffer exists specifically to solve this — it lets the GPU reuse vertex data across multiple triangles by referencing an index into the vertex array.

**Current code (triangle path):**
```cpp
// The deduplication block using `uniqueVertices` is commented out.
// Instead:
m_indices.push_back(static_cast<uint32_t>(m_vertices.size()));
m_vertices.push_back(vertex);
```

This pushes every single vertex, even duplicates. On a sphere or organic mesh this can 3-5x your vertex buffer size and hurt GPU cache performance.

**Fix:** Uncomment the `uniqueVertices` deduplication block that is already written and correct:
```cpp
if (uniqueVertices.count(vertex) == 0)
{
    uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
    m_vertices.push_back(vertex);
}
m_indices.push_back(uniqueVertices[vertex]);
```
Note: this requires `Vertex` to have `operator==` and a hash function defined in `Structs.h` — they presumably are since the code was working before being commented out.

---

### 2.3 — Descriptor Pool Growth Strategy Has No Upper Bound

**File:** `Allocator.cpp` → `CheckPoolAvailability()`

**Vulkan concept:** Descriptor pools hold descriptor sets (which bind textures, buffers, etc. to shaders). When a pool is full, you create another. The current code does this correctly. However, there is no cap on how many pools can be created. If there's a bug elsewhere that causes repeated allocation without corresponding frees (e.g. during rapid scene reloads), the pool count will grow unbounded.

**Fix:** Add a warning log and optional hard cap:
```cpp
if (m_descriptorPools.size() > 32)
{
    FlatEngine::LogError("Allocator: Descriptor pool count exceeds 32 — possible leak.");
}
```

---

### 2.4 — `SetFreed` Tracking Is Based on Pool Capacity, Not Actual Set Count

**File:** `Allocator.cpp` → `SetFreed()`

**Vulkan concept:** When a descriptor set is freed, the pool it came from needs to know. When all sets in a pool are freed, the pool itself can be destroyed.

**The problem:** `SetFreed` queues pool deletion when `m_setsFreedByPool[freedFrom] == m_sizePerPool`. But `m_sizePerPool` is the pool's _capacity_, not the number of sets actually allocated from it. If a pool was only partially used (e.g. 30 sets allocated from a 100-set pool), `SetFreed` will never trigger — the pool leaks.

**Fix:** Track the actual allocation count per pool separately from the capacity:
```cpp
std::vector<uint32_t> m_setsAllocatedByPool; // add this member
// In CheckPoolAvailability, when allocating:
m_setsAllocatedByPool[m_currentPoolIndex]++;
// In SetFreed:
if (m_setsFreedByPool[freedFrom] == m_setsAllocatedByPool[freedFrom])
{
    // safe to queue deletion
}
```

---

### 2.5 — `WinSys::CreateBuffer` Calls `vkAllocateMemory` Individually for Every Buffer

**File:** `WinSys.cpp` → `CreateBuffer()`

Same issue as 2.1 but for general buffers (vertex buffers, index buffers, staging buffers). Every `CreateBuffer` call is its own `vkAllocateMemory`. This is the correct approach for learning, but hits the allocation count limit in production. VMA (mentioned above) solves this too.

---

## 3. Structural / Architectural Issues

These don't cause crashes but make the code harder to maintain and extend.

---

### 3.1 — Pipeline State Is Duplicated Between `GraphicsPipeline` and `Material`

**File:** `GraphicsPipeline.h/.cpp`, `Material.h/.cpp`

**The problem:** Both `GraphicsPipeline` and `Material` store their own copies of `m_rasterizer`, `m_inputAssembly`, and `m_colorBlendAttachment`. Material setters (`SetRasterizerCreateInfos`, etc.) call through to `GraphicsPipeline` setters, but the structs are stored and initialized in both classes. This means state must be kept in sync across two owners.

For example, `Material::SetDefaultValues()` initializes all three structs, then calls:
```cpp
m_graphicsPipeline.SetInputAssemblyInfos(m_inputAssembly);
m_graphicsPipeline.SetRasterizerCreateInfos(m_rasterizer);
m_graphicsPipeline.SetColorBlendAttachmentCreateInfos(m_colorBlendAttachment);
```

If you update `m_rasterizer` on the Material but forget to call the setter, the pipeline uses stale data.

**Fix:** Remove the pipeline state structs from `Material` entirely. Let `Material` hold its own configuration as plain data, and only pass them into `GraphicsPipeline::CreateGraphicsPipeline()` when building the pipeline:

```cpp
// In Material.h — just store the config, don't sync to GraphicsPipeline:
struct PipelineConfig {
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
};
PipelineConfig m_pipelineConfig;

// In GraphicsPipeline::CreateGraphicsPipeline(), accept a config:
void CreateGraphicsPipeline(LogicalDevice&, WinSys&, RenderPass&, VkDescriptorSetLayout&, const PipelineConfig& config);
```

`GraphicsPipeline` then only stores the live `VkPipeline` and `VkPipelineLayout` handles — not the config data used to build them.

---

### 3.2 — `"imgui"` String Check Is a Special Case Inside `Material::Init()`

**File:** `Material.cpp` → `Init()`

**The problem:**
```cpp
if (m_name != "imgui")
{
    m_allocator.Init(...);
    m_graphicsPipeline.CreatePushConstantRanges();
}
```

This is fragile. The material name "imgui" has become load-bearing. If you rename the material file, this breaks silently. It's also a hidden contract that new developers won't know about.

**Fix:** Replace the string check with an explicit flag:
```cpp
// In Material.h:
bool m_b_usePushConstants = true;
bool m_b_useAllocator = true;

// In Material::Init():
if (m_b_useAllocator)
    m_allocator.Init(...);
if (m_b_usePushConstants)
    m_graphicsPipeline.CreatePushConstantRanges();
```

Set `m_b_usePushConstants = false` and `m_b_useAllocator = false` when constructing the ImGui material in `VulkanManager`.

---

### 3.3 — Three-Level Nested Map for Mesh Grouping Is Fragile and Duplicated

**File:** `VulkanManager.h/.cpp`

**The problem:** 
```cpp
std::map<std::string, std::map<std::string, std::map<long, Mesh*>>> m_sceneViewMaterialMeshes;
std::map<std::string, std::map<std::string, std::map<long, Mesh*>>> m_gameViewMaterialMeshes;
```

This is a `material name → model path → mesh ID → Mesh*` lookup. `AddSceneViewMaterialMesh`, `RemoveSceneViewMaterialMesh`, `AddGameViewMaterialMesh`, and `RemoveGameViewMaterialMesh` are all nearly identical.

**Fix:** Introduce a `MeshRegistry` class that owns one such map and exposes `Add`, `Remove`, and iteration. Then `VulkanManager` holds two instances:
```cpp
MeshRegistry m_sceneViewMeshes;
MeshRegistry m_gameViewMeshes;
```

This eliminates ~100 lines of duplicated nested-map manipulation code and gives you one place to fix bugs or change the data structure.

---

### 3.4 — `PipelineManager` Is Mostly Empty and Its Role Is Unclear

**File:** `PipelineManager.h/.cpp`, `ImGuiManager.h/.cpp`

`PipelineManager` is a base class with most of its body commented out. `ImGuiManager` extends it but also has most of its body commented out. The class hierarchy exists but doesn't do real work — `VulkanManager` directly manages render passes and ImGui init instead.

**Fix:** Either delete `PipelineManager` and `ImGuiManager` and move their remaining code directly into `VulkanManager`, or commit to the inheritance pattern and move `CreateImGuiResources`, `QuitImGui`, etc. back into `ImGuiManager`. Leaving half-implemented class hierarchies is the most disorienting thing for someone returning to code after time away.

---

### 3.5 — `VulkanManager` Is a God Object

**File:** `VulkanManager.h/.cpp`

`VulkanManager` handles: Vulkan instance creation, device selection, swapchain management, material loading/saving, model loading, mesh registration, ImGui lifetime, render pass orchestration, frame synchronization, and memory deletion queues.

This isn't a blocker but it's the root cause of many other issues. As a long-term goal, consider splitting it:
- `ResourceManager` — materials, models, textures
- `FrameOrchestrator` — `DrawFrame`, sync objects, command buffer submission
- `VulkanContext` — instance, device, swapchain (what it already has in `WinSys` + `PhysicalDevice` + `LogicalDevice`)

---

### 3.6 — `Helper::BeginSingleTimeCommands` and `EndSingleTimeCommands` Access `F_VulkanManager` Globally

**File:** `Helper.cpp`

```cpp
LogicalDevice& logicalDevice = F_VulkanManager->GetLogicalDevice();
```

These are static helper functions that reach out to the global `F_VulkanManager`. This makes them impossible to test in isolation and creates a hidden global dependency. Both functions already receive the `commandPool` as a parameter — they should also receive the `LogicalDevice`:

```cpp
static VkCommandBuffer BeginSingleTimeCommands(VkCommandPool& commandPool, LogicalDevice& logicalDevice);
static void EndSingleTimeCommands(VkCommandBuffer, VkCommandPool&, LogicalDevice& logicalDevice);
```

---

## 4. Performance Issues

---

### 4.1 — `DrawIndexed` Copies the Entire Index Vector Every Draw Call

**File:** `RenderPass.cpp` → `DrawIndexed()`

```cpp
void RenderPass::DrawIndexed(std::shared_ptr<Model> model)
{
    std::vector<uint32_t> indices = model->GetIndices(); // FULL COPY every call
    vkCmdDrawIndexed(..., static_cast<uint32_t>(indices.size()), ...);
}
```

`model->GetIndices()` returns by value, so the entire index array is heap-allocated and copied just to call `.size()` on it. In `DrawFrame`, this is called once per mesh per frame.

**Fix:** Either add an `GetIndexCount()` method to `Model`, or change `GetIndices()` to return `const std::vector<uint32_t>&`:
```cpp
// Model.h
uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_indices.size()); }

// RenderPass.cpp
vkCmdDrawIndexed(m_commandBuffers[VM_currentFrame], model->GetIndexCount(), 1, 0, 0, 0);
```

---

### 4.2 — `UpdateUniformBuffer` Recalculates the View and Projection Matrices Per Mesh Per Frame

**File:** `Mesh.cpp` → `UpdateUniformBuffer()`

The view matrix (camera position + look direction) and projection matrix (FOV, aspect ratio, near/far clip) are recalculated from scratch for every mesh. These values are the same for every mesh in a given viewport and frame.

**Fix:** Calculate view and projection once per frame per viewport in `VulkanManager::DrawFrame()` and store them in a small per-frame struct. Pass this struct (or a const reference to it) into `UpdateUniformBuffer`. This saves a `glm::lookAt` and `glm::perspective` call per mesh per frame.

---

### 4.3 — `std::list<VkCommandBuffer>` Is Used in `DrawFrame` for No Reason

**File:** `VulkanManager.cpp` → `DrawFrame()`

```cpp
std::list<VkCommandBuffer> commandBuffers = std::list<VkCommandBuffer>();
// ... push_back to list ...
std::vector<VkCommandBuffer> buffers = std::vector<VkCommandBuffer>();
for (VkCommandBuffer& buffer : commandBuffers)
    buffers.push_back(buffer);
// submit buffers
```

A `std::list` is then converted to a `std::vector` just to pass to `vkQueueSubmit`. `std::list` has worse cache performance than `std::vector`, and conversion adds an extra loop.

**Fix:** Use `std::vector<VkCommandBuffer>` directly throughout:
```cpp
std::vector<VkCommandBuffer> commandBuffers;
// push_back directly
// submit directly — no conversion needed
```

---

### 4.4 — New SDL2 Extension Array Is `new[]`-Allocated and Never Freed

**File:** `VulkanManager.cpp` → `CreateVulkanInstance()`

```cpp
sdlExtensionNames = new const char* [sdlExtensionCount];
// ... used ...
// never deleted
```

And:
```cpp
const char** extensionNames = new const char* [totalExtensionCount];
// ... used ...
// never deleted
```

These are small and only happen once, but it's a leak and a bad pattern.

**Fix:** Use `std::vector<const char*>` instead of raw `new[]`:
```cpp
std::vector<const char*> sdlExtensionNames(sdlExtensionCount);
SDL_Vulkan_GetInstanceExtensions(m_winSystem.GetWindow(), &sdlExtensionCount, sdlExtensionNames.data());

auto validationExtensions = VM_validationLayers.GetRequiredExtensions();
std::vector<const char*> allExtensions(sdlExtensionNames);
allExtensions.insert(allExtensions.end(), validationExtensions.begin(), validationExtensions.end());

createInfo.enabledExtensionCount = static_cast<uint32_t>(allExtensions.size());
createInfo.ppEnabledExtensionNames = allExtensions.data();
```

---

### 4.5 — `ThreadPool::JoinThreads()` Sets `m_b_stop` Without Notifying Threads First

**File:** `ThreadPool.cpp` → `JoinThreads()`

```cpp
void ThreadPool::JoinThreads()
{
    m_b_stop = true;
    for (auto& thread : m_threads)
    {
        if (thread.joinable())
            thread.join();
    }
    InitThreads();
}
```

Setting `m_b_stop = true` without calling `m_conditionVariable.notify_all()` means threads blocked on `m_conditionVariable.wait()` won't wake up to check the stop flag. The `join()` calls will hang forever. The destructor handles this correctly — `JoinThreads` should too:

```cpp
void ThreadPool::JoinThreads()
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_b_stop = true;
    }
    m_conditionVariable.notify_all(); // wake all waiting threads
    for (auto& thread : m_threads)
    {
        if (thread.joinable())
            thread.join();
    }
    m_b_stop = false; // reset for InitThreads
    m_threads.clear();
    InitThreads();
}
```

---

## 5. Code Duplication

---

### 5.1 — `CreateRenderToTextureRenderPassResources` and `CreatePostProcessingRenderPassResources` Are Almost Identical

**File:** `VulkanManager.cpp`

Both functions create a 3-attachment render pass (color, depth, resolve) with identical configurations. The only differences are which member variables (`m_postProcessingRenderPass` vs the parameter) and whether a subpass dependency is added.

**Fix:** `CreateRenderToTextureRenderPassResources` already accepts a `RenderPass&` and `Texture&` as parameters — this is the right pattern. `CreatePostProcessingRenderPassResources` should call it (or a shared internal helper) rather than duplicating ~80 lines:
```cpp
void VulkanManager::CreatePostProcessingRenderPassResources()
{
    CreateRenderToTextureRenderPassResources(m_postProcessingRenderPass, m_postProcessingTexture, m_postProcessingCommandPool);
    // Add any post-processing-specific subpass dependencies here
}
```

---

### 5.2 — Material Operations Are Always Mirrored Across Scene View and Game View

**File:** `VulkanManager.cpp` — `AddTextureToMaterial`, `RemoveTextureFromMaterial`, `AddUBOVec4ToMaterial`, `RemoveUBOVec4FromMaterial`, `SetMaterialVertexPath`, `SetMaterialFragmentPath`

Every one of these functions does:
```cpp
if (m_sceneViewMaterials.count(name)) m_sceneViewMaterials.at(name)->DoSomething();
if (m_gameViewMaterials.count(name))  m_gameViewMaterials.at(name)->DoSomething();
```

This pattern repeats 10+ times. A lambda helper eliminates all of it:

```cpp
void VulkanManager::ForBothViewMaterials(const std::string& name, std::function<void(Material&)> fn)
{
    if (m_sceneViewMaterials.count(name)) fn(*m_sceneViewMaterials.at(name));
    if (m_gameViewMaterials.count(name))  fn(*m_gameViewMaterials.at(name));
}

// Usage:
void VulkanManager::SetMaterialVertexPath(std::string materialName, std::string vertexPath)
{
    ForBothViewMaterials(materialName, [&](Material& m){ m.SetVertexPath(vertexPath); });
}
```

---

### 5.3 — `LoadEngineMaterials` Calls `AddSceneViewMaterial` and `AddGameViewMaterial` Redundantly for Every Engine Material

**File:** `VulkanManager.cpp` → `LoadEngineMaterials()`

```cpp
AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_empty.mat", &m_renderToTextureSceneViewRenderPass));
AddGameViewMaterial(LoadMaterial("../engine/materials/fl_empty.mat", &m_renderToTextureGameViewRenderPass));
// ... repeated 7 more times
```

A helper that loads both at once:
```cpp
void VulkanManager::LoadMaterialForBothViews(const std::string& path)
{
    AddSceneViewMaterial(LoadMaterial(path, &m_renderToTextureSceneViewRenderPass));
    AddGameViewMaterial(LoadMaterial(path, &m_renderToTextureGameViewRenderPass));
}
```

Then `LoadEngineMaterials` becomes:
```cpp
LoadMaterialForBothViews("../engine/materials/fl_empty.mat");
LoadMaterialForBothViews("../engine/materials/fl_unlit.mat");
// ...
```

---

### 5.4 — `Allocator::SetDefaultDescriptorSetLayoutConfig` and `SetDefaultDescriptorPoolConfig` Both Iterate `m_texturePipelineData`

**File:** `Allocator.cpp`

Both functions iterate the texture pipeline data map to configure descriptor bindings and pool sizes. The iteration logic is nearly identical. This is fine for now, but if the descriptor structure changes you'll need to update both.

---

## 6. Threading

---

### 6.1 — ThreadPool Exists But Is Not Used in the Render Path

**File:** `ThreadPool.h/.cpp`, `VulkanManager.cpp`

You have a well-structured thread pool implementation, and the commented-out threading code in `DrawFrame` shows you were planning to use it. The comment in `DrawFrame` correctly identifies why it was removed: the same `VkDescriptorPool` was being accessed from multiple threads, which Vulkan prohibits without external synchronization.

**The path forward when you return to this:** The fix is not to synchronize access to the shared pool — it's to give each thread its own allocator (and therefore its own descriptor pool). Since you already have `m_sceneViewCommandPool` and `m_gameViewCommandPool` as separate objects, the same pattern applies to descriptor allocation:

- Give the scene view render path its own `Allocator` instance
- Give the game view render path its own `Allocator` instance
- Record scene view and game view command buffers on separate threads
- Join both threads before the ImGui pass (which is single-threaded anyway, as ImGui is not thread-safe)

The ImGui descriptor allocation that currently happens after the join (in `DrawFrame`) must stay on the main thread — that is correct.

---

### 6.2 — `volatile bool m_b_stop` Should Be `std::atomic<bool>`

**File:** `ThreadPool.h`

```cpp
volatile bool m_b_stop = false;
```

`volatile` prevents compiler optimization of the variable but does not provide the memory ordering guarantees needed for cross-thread communication. In C++11 and later, `std::atomic<bool>` is the correct tool:

```cpp
std::atomic<bool> m_b_stop{false};
```

This ensures the write from one thread is immediately visible to all other threads without a data race.

---

## 7. Minor Correctness Issues

---

### 7.1 — `Mesh::UpdateUniformBuffer` Has a Dead Branch for `b_forceZUp`

**File:** `Mesh.cpp` → `UpdateUniformBuffer()`

```cpp
if (b_forceZUp)
{
    up = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
}
else
{
    up = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); // same value
}
```

Both branches produce the same result. The `else` branch was presumably meant to use a Y-up vector: `glm::vec4(0.0f, 1.0f, 0.0f, 0.0f)`. This should be corrected once the intended behavior is confirmed.

---

### 7.2 — `GetModel` in `VulkanManager` Can Load a Model Twice

**File:** `VulkanManager.cpp` → `GetModel()`

```cpp
std::shared_ptr<Model> VulkanManager::GetModel(std::string modelPath)
{
    if (m_models.count(modelPath))
    {
        return m_models.at(modelPath);
    }
    else
    {
        std::shared_ptr<Model> loadedModel = LoadModel(modelPath);
        m_models.emplace(modelPath, loadedModel);
        return loadedModel;
    }
}
```

`LoadModel()` also calls `m_models.emplace(modelPath, newModel)` at the end. So if `GetModel` is called for a model that isn't cached, it calls `LoadModel`, which inserts into `m_models`, and then `GetModel` tries to insert again. The second `emplace` will silently fail (since the key now exists), but it's a hidden double-insert attempt.

**Fix:** Remove the `m_models.emplace` from `LoadModel`, and let `GetModel` own the caching:
```cpp
std::shared_ptr<Model> VulkanManager::LoadModel(std::string modelPath)
{
    std::shared_ptr<Model> newModel = std::make_shared<Model>();
    newModel->Init(...);
    newModel->LoadModel(modelPath);
    return newModel; // don't insert into m_models here
}

std::shared_ptr<Model> VulkanManager::GetModel(std::string modelPath)
{
    auto it = m_models.find(modelPath);
    if (it != m_models.end()) return it->second;

    auto model = LoadModel(modelPath);
    m_models.emplace(modelPath, model);
    return model;
}
```

---

### 7.3 — `RemoveUBOVec4` Index Boundary Check Is Off by One

**File:** `Material.cpp` → `RemoveUBOVec4()`

```cpp
if (index != -1 && m_uboVec4Names.size() >= index)
{
    m_uboVec4Names.erase(index);
}
```

`size() >= index` allows `index == size()`, which is out of bounds for a 0-indexed map. Should be `size() > (size_t)index` or equivalently `m_uboVec4Names.count((uint32_t)index)` to check the key exists before erasing.

---

### 7.4 — `Allocator::AllocateDescriptorSets` Reduces Pool Availability Twice (Once Per Frame in Flight)

**File:** `Allocator.cpp` → `AllocateDescriptorSets()`

```cpp
for (int i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
{
    // ... configure descriptor writes ...
    vkUpdateDescriptorSets(...);
    m_allocationsRemainingByPool[m_currentPoolIndex] -= 1; // decremented per frame
}
```

The pool availability counter decrements once per frame in flight per call. `vkAllocateDescriptorSets` is called once with `descriptorSetCount = VM_MAX_FRAMES_IN_FLIGHT` — so it allocates `VM_MAX_FRAMES_IN_FLIGHT` sets in one call. The decrement inside the loop correctly accounts for this if `VM_MAX_FRAMES_IN_FLIGHT == 2` (2 decrements for 2 sets), but the intent is not obvious. If `MAX_FRAMES_IN_FLIGHT` changes, the accounting changes with it. A clearer approach:

```cpp
// After the allocation:
m_allocationsRemainingByPool[m_currentPoolIndex] -= VM_MAX_FRAMES_IN_FLIGHT;
// Outside the per-frame loop — reflect the actual number of sets allocated
```

---

### 7.5 — `WinSys::CreateTextureImage` Ignores the `mipLevels` Parameter It Receives

**File:** `WinSys.cpp` → `CreateTextureImage()`

```cpp
VkImage WinSys::CreateTextureImage(std::string path, uint32_t mipLevels, ...)
{
    // ...
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(...))) + 1; // overwrites parameter
```

The `mipLevels` parameter is passed in, then immediately overwritten by recalculation from the image size. The caller has no way to control the mip level count. This is fine if auto-calculation is always desired, but the parameter should be removed from the signature to avoid the false impression that it controls anything:

```cpp
VkImage WinSys::CreateTextureImage(std::string path, VkDeviceMemory& textureImageMemory, VkCommandPool& commandPool);
// mipLevels calculated internally, returned via out-param or struct if caller needs it
```

---

## 8. Future Features Worth Planning For

These are not bugs but architectural decisions to make now that will save significant rework later.

---

### 8.1 — Plan for a Pipeline Cache

`LogicalDevice` already has `m_graphicsPipelineCache` as a member, but it's never initialized or used — `VK_NULL_HANDLE` is passed to `vkCreateGraphicsPipelines` everywhere.

A pipeline cache (`VkPipelineCache`) allows the driver to reuse compiled pipeline state across runs. Serialize it to disk on shutdown, load it on startup. Pipeline compilation in `GraphicsPipeline::CreateGraphicsPipeline()` becomes dramatically faster on subsequent launches (especially relevant for hot reload).

**Where to add it:** Pass `m_logicalDevice.GetGraphicsPipelineCache()` as the second argument to `vkCreateGraphicsPipelines` in `GraphicsPipeline.cpp`. Initialize the cache in `VulkanManager::InitVulkan()` after device creation:
```cpp
VkPipelineCacheCreateInfo cacheInfo{};
cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
VkPipelineCache cache;
vkCreatePipelineCache(m_logicalDevice.GetDevice(), &cacheInfo, nullptr, &cache);
m_logicalDevice.SetGraphicsPipelineCache(cache);
```

---

### 8.2 — The `CustomUBO` `vec4s[32]` Array Needs a Defined Contract

**File:** `Mesh.cpp` → `UpdateUniformBuffer()` comment: *"FIX ME: 32 is the size of the m_uboVec4s array"*

This magic number 32 must match the shader definition exactly. If someone changes the shader array size, the CPU-side struct silently mismatches and the GPU reads garbage.

**Fix:** Define the count as a shared constant in `Structs.h`:
```cpp
constexpr uint32_t MAX_UBO_VEC4S = 32;

struct CustomUBO {
    BaseUBO baseUBO;
    glm::vec4 vec4s[MAX_UBO_VEC4S];
};
```

And in your GLSL shader:
```glsl
#define MAX_UBO_VEC4S 32
layout(binding = 0) uniform CustomUBO {
    // ...
    vec4 vec4s[MAX_UBO_VEC4S];
};
```

A shader compilation step that validates this at build time would be ideal.

---

### 8.3 — The Post-Processing Pass Is Stubbed Out but the Infrastructure Exists

The `m_postProcessingRenderPass`, `m_postProcessingTexture`, and `m_postProcessingCommandPool` are all initialized. The draw call in `DrawFrame` is commented out. When you return to implement post-processing:

- The scene view texture needs to be sampled as an input to the post-processing pass
- Its final layout in `CreateRenderToTextureRenderPassResources` is `VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL` — you'll need `VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL` if the post-processing pass samples it (this is what the `colorAttachmentResolve.finalLayout` on the resolve attachment already does — make sure you're reading from the resolved texture, not the MSAA color buffer)

---

### 8.4 — Transparent Object Rendering Is Not Handled

`GraphicsPipeline.cpp` has this comment:
```cpp
m_depthStencil.depthWriteEnable = VK_TRUE; // Set false for transparent meshes, then render those transparent objects last, back to front in drawindexed
```

The note is correct. Transparent rendering requires:
1. A separate render pass or subpass, after all opaque geometry
2. Depth writes disabled (`depthWriteEnable = VK_FALSE`) for the transparent pipeline
3. Meshes sorted back-to-front by distance from the camera before drawing

The current `m_sceneViewMaterialMeshes` grouping by material makes back-to-front sorting harder since you'd need to break material batching for transparent objects. A common approach is a dedicated `m_transparentMeshes` list that is sorted per-frame separately from the opaque material batches.

---

*End of action plan. Items in Section 1 should be addressed first as they affect correctness. Section 2 items are important before shipping. Sections 3–5 can be addressed incrementally as you touch each system.*
