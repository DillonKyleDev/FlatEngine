#include "Material.h"
#include "VulkanManager.h"
#include "FlatEngine.h"
#include "Vector3.h"
#include "Transform.h"
#include "Camera.h"
#include "Helper.h"

#include <stdexcept>

#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	Material::Material(std::string name, std::string vertexPath, std::string fragmentPath)
	{
		m_name = name;
		m_graphicsPipeline = GraphicsPipeline(vertexPath, fragmentPath);
		m_renderPass = RenderPass();		
		SetDefaultValues();
	}

	Material::Material()
	{
		m_name = "";
		m_graphicsPipeline = GraphicsPipeline();
		SetDefaultValues();
	}

	Material::~Material()
	{
	}

	void Material::SetDefaultValues()
	{
		m_textureCount = 0;
		m_allocator = Allocator();
		// handles
		m_winSystem = nullptr;
		m_logicalDevice = nullptr;
		m_b_initialized = false;

		m_renderTexture = Texture();
		m_imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		m_mipLevels = 1;

		// Set default RenderPass configuration
		// TODO
		
		// Default Graphics Pipeline configuration (Filled in with saved values when LoadMaterial() is called
		m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_inputAssembly.primitiveRestartEnable = VK_FALSE;

		m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_rasterizer.depthClampEnable = VK_FALSE;
		m_rasterizer.rasterizerDiscardEnable = VK_FALSE;
		m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		m_rasterizer.lineWidth = 1.0f;
		m_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		m_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		m_rasterizer.depthBiasEnable = VK_FALSE;
		m_rasterizer.depthBiasConstantFactor = 0.0f;
		m_rasterizer.depthBiasClamp = 0.0f;
		m_rasterizer.depthBiasSlopeFactor = 0.0f;

		m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT | 0;
		m_colorBlendAttachment.blendEnable = VK_TRUE;
		m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		
		m_graphicsPipeline.SetInputAssemblyInfos(m_inputAssembly);
		m_graphicsPipeline.SetRasterizerCreateInfos(m_rasterizer);
		m_graphicsPipeline.SetColorBlendAttachmentCreateInfos(m_colorBlendAttachment);
	}

	std::string Material::GetData()
	{
		json inputAssemblyData = {
			{ "topology", (int)m_inputAssembly.topology },
			{ "_primitiveRestartEnable", (bool)m_inputAssembly.primitiveRestartEnable }
		};

		json rasterizerData = {
			{ "polygonMode", (int)m_rasterizer.polygonMode },
			{ "cullMode", (int)m_rasterizer.cullMode },
			{ "lineWidth", m_rasterizer.lineWidth }
		};

		json colorBlendAttachmentData = {
			{ "alphaBlendOp", (int)m_colorBlendAttachment.alphaBlendOp }
		};

		json jsonData = {
			{ "name", m_name },
			{ "vertexShaderPath", m_graphicsPipeline.GetVertexPath() },
			{ "fragmentShaderPath", m_graphicsPipeline.GetFragmentPath() },
			{ "textureCount", m_textureCount },
			{ "rasterizerData", rasterizerData },
			{ "inputAssemblyData", inputAssemblyData },
			{ "colorBlendAttachmentData", colorBlendAttachmentData }
		};

		std::string data = jsonData.dump(4);
		// Return dumped json object with required data for saving
		return data;
	}

	void Material::Init()
	{
		if (m_graphicsPipeline.GetVertexPath() != "" && m_graphicsPipeline.GetFragmentPath() != "")
		{
			CreateImageResources();
			CreateRenderPassResources();
			if (m_name == "imgui")
			{
				CreateImGuiResources();
			}
			else
			{
				m_allocator.Init(AllocatorType::DescriptorPool, m_textureCount, *m_logicalDevice);
				m_graphicsPipeline.CreatePushConstantRanges();
			}			
			m_graphicsPipeline.CreateGraphicsPipeline(*m_logicalDevice, *m_winSystem, m_renderPass, m_allocator.GetDescriptorSetLayout());
			m_b_initialized = true;
		}
	}

	bool Material::Initialized()
	{
		return m_b_initialized;
	}

	void Material::Cleanup()
	{
		if (m_name == "imgui")
		{
			QuitImGui();
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplSDL2_Shutdown();
		}

		m_renderPass.Cleanup(*m_logicalDevice);
		m_graphicsPipeline.Cleanup(*m_logicalDevice);
	}

	void Material::QuitImGui()
	{
		vkDestroyCommandPool(m_logicalDevice->GetDevice(), *m_commandPool, nullptr);

		VkResult err = vkDeviceWaitIdle(m_logicalDevice->GetDevice());
		VulkanManager::check_vk_result(err);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void Material::SetHandles(VkInstance* instance, WinSys* winSystem, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkCommandPool* commandPool)
	{		
		m_instance = instance;
		m_winSystem = winSystem;
		m_physicalDevice = physicalDevice;
		m_logicalDevice = logicalDevice;
		m_commandPool = commandPool;
		m_renderPass.SetHandles(instance, winSystem, physicalDevice, logicalDevice);
	}

	void Material::RecreateGraphicsPipeline()
	{
		m_graphicsPipeline.Cleanup(*m_logicalDevice);

		if (m_graphicsPipeline.GetVertexPath() != "" && m_graphicsPipeline.GetFragmentPath() != "")
		{
			m_graphicsPipeline.CreateGraphicsPipeline(*m_logicalDevice, *m_winSystem, m_renderPass, m_allocator.GetDescriptorSetLayout());
		}
	}

	void Material::GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo)
	{
		bindings.resize(1);
		bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		bindings[0].descriptorCount = 1;
		bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		bindings[0].binding = 0;
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();
	}

	void Material::GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo)
	{
		poolSizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = 1000;
		poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();
	}

	void Material::CreateImGuiResources()
	{
		// https://frguthmann.github.io/posts/vulkan_imgui/       

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;// | ImGuiConfigFlags_ViewportsEnable;
		//io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_RendererHasViewports;

		VulkanManager::CreateCommandPool(*m_commandPool, *m_logicalDevice, m_logicalDevice->GetGraphicsIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		// Set up Descriptor Material Allocator
		std::vector<VkDescriptorSetLayoutBinding> bindings{};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		GetImGuiDescriptorSetLayoutInfo(bindings, layoutInfo);
		std::vector<VkDescriptorPoolSize> poolSizes{};
		VkDescriptorPoolCreateInfo poolInfo{};
		GetImGuiDescriptorPoolInfo(poolSizes, poolInfo);
		
		m_allocator.ConfigureDescriptorSetLayout(bindings, layoutInfo);
		m_allocator.ConfigureDescriptorPools(poolSizes, poolInfo);
		m_allocator.Init(AllocatorType::DescriptorPool, m_textureCount, *m_logicalDevice);

		ImGui_ImplSDL2_InitForVulkan(m_winSystem->GetWindow());

		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = *m_instance;
		init_info.PhysicalDevice = m_physicalDevice->GetDevice();
		init_info.Device = m_logicalDevice->GetDevice();
		init_info.QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(m_physicalDevice->GetDevice());
		init_info.Queue = m_logicalDevice->GetGraphicsQueue();
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = CreateDescriptorPool();
		init_info.RenderPass = m_renderPass.GetRenderPass();
		init_info.Subpass = 0;
		init_info.MinImageCount = VM_MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = VM_MAX_FRAMES_IN_FLIGHT;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = nullptr;
		init_info.CheckVkResultFn = VulkanManager::check_vk_result;

		if (!ImGui_ImplVulkan_Init(&init_info))
		{
			FlatEngine::LogError("ImGui backends setup failed!");
		}
	}

	void Material::SetName(std::string name)
	{
		m_name = name;
	}

	std::string Material::GetName()
	{
		return m_name;
	}

	void Material::SetPath(std::string path)
	{
		m_path = path;
	}

	std::string Material::GetPath()
	{
		return m_path;
	}

	void Material::SetVertexPath(std::string path)
	{
		m_graphicsPipeline.SetVertexPath(path);
	}

	void Material::SetFragmentPath(std::string path)
	{
		m_graphicsPipeline.SetFragmentPath(path);
	}

	std::string Material::GetVertexPath()
	{
		return m_graphicsPipeline.GetVertexPath();
	}

	std::string Material::GetFragmentPath()
	{
		return m_graphicsPipeline.GetFragmentPath();
	}

	VkPipeline& Material::GetGraphicsPipeline()
	{
		return m_graphicsPipeline.GetGraphicsPipeline();
	}

	VkPipelineLayout& Material::GetPipelineLayout()
	{
		return m_graphicsPipeline.GetPipelineLayout();
	}

	VkDescriptorPool Material::CreateDescriptorPool()
	{
		return m_allocator.CreateDescriptorPool();
	}

	void Material::CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures)
	{
		m_allocator.AllocateDescriptorSets(descriptorSets, model, textures);
	}

	Allocator& Material::GetAllocator()
	{
		return m_allocator;
	}

	void Material::SetTextureCount(uint32_t textureCount)
	{
		m_textureCount = textureCount;
	}

	uint32_t Material::GetTextureCount()
	{
		return m_textureCount;
	}

	void Material::OnWindowResized()
	{
		// Cleanup image resources??
		CreateImageResources();
		m_renderPass.RecreateFrameBuffers();
	}

	void Material::CreateImageResources()
	{
		if (m_name == "imgui")
		{
			m_renderTexture.GetImageViews() = m_winSystem->GetSwapChainImageViews();
			m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews()); // Give m_renderPass the new imageViews
			// m_renderPass.ConfigureFrameBufferImageViews(m_winSystem->GetSwapChainImageViews());
		}
		else 	// Configure images used to render to texture
		{
			std::vector<VkImage>& images = m_renderTexture.GetImages();
			std::vector<VkImageView>& imageViews = m_renderTexture.GetImageViews();
			std::vector<VkDeviceMemory>& imageMemory = m_renderTexture.GetImageMemory();
			VkSampler& sampler = m_renderTexture.GetSampler();
			images.resize(VM_MAX_FRAMES_IN_FLIGHT);
			imageViews.resize(VM_MAX_FRAMES_IN_FLIGHT);
			imageMemory.resize(VM_MAX_FRAMES_IN_FLIGHT);
			VkExtent2D extent = m_winSystem->GetExtent();

			for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
			{
				uint32_t mipLevels = 1;
				int texWidth = extent.width;
				int texHeight = extent.height;
				VkDeviceSize imageSize = texWidth * texHeight * 16;
				VkBuffer stagingBuffer{};
				VkDeviceMemory stagingBufferMemory{};
				m_winSystem->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
				m_winSystem->CreateImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, m_imageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, images[i], imageMemory[i]);

				VkCommandBuffer copyCmd = Helper::BeginSingleTimeCommands();
				m_winSystem->InsertImageMemoryBarrier(
					copyCmd,
					images[i],
					VK_ACCESS_TRANSFER_READ_BIT,
					VK_ACCESS_MEMORY_READ_BIT,
					VK_IMAGE_LAYOUT_UNDEFINED,
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
				Helper::EndSingleTimeCommands(copyCmd);

				m_winSystem->TransitionImageLayout(images[i], m_imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
				m_winSystem->CopyBufferToImage(stagingBuffer, images[i], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
				m_winSystem->TransitionImageLayout(images[i], m_imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

				vkDestroyBuffer(m_logicalDevice->GetDevice(), stagingBuffer, nullptr);
				vkFreeMemory(m_logicalDevice->GetDevice(), stagingBufferMemory, nullptr);

				m_winSystem->CreateImageView(imageViews[i], images[i], m_imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
				m_winSystem->CreateTextureSampler(sampler, mipLevels);
			}

			m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews()); // Give m_renderPass the VkImageViews to write to their VkImages (to be used later by ImGui material)
		}
	}

	VkPipelineInputAssemblyStateCreateInfo& Material::GetInputAssemblyCreateInfos()
	{
		return m_inputAssembly;
	}

	void Material::SetInputAssemblyCreateInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos)
	{
		m_inputAssembly = inputAssemblyInfos;
		m_graphicsPipeline.SetInputAssemblyInfos(m_inputAssembly);
	}

	VkPipelineRasterizationStateCreateInfo& Material::GetRasterizerCreateInfos()
	{
		return m_rasterizer;
	}

	void Material::SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos)
	{
		m_rasterizer = rasterizerInfos;
		m_graphicsPipeline.SetRasterizerCreateInfos(m_rasterizer);
	}

	void Material::SetColorBlendAttachmentCreateInfos(VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos)
	{
		m_colorBlendAttachment = colorBlendAttachmentInfos;
	}

	VkPipelineColorBlendAttachmentState& Material::GetColorBlendAttachmentCreateInfos()
	{
		return m_colorBlendAttachment;
	}


	void Material::CreateRenderPassResources()
	{
		if (m_name == "imgui")
		{
			// Configure ImGui Render Pass
			m_renderPass.EnableMsaa();
			VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // F_VulkanManager->GetMaxSamples();
			VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_renderPass.SetImageColorFormat(colorFormat);
			m_renderPass.SetMSAASampleCount(msaaSamples);

			VkAttachmentDescription colorAttachment = {};
			colorAttachment.format = m_winSystem->GetImageFormat();
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkAttachmentReference colorAttachmentRef = {};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			m_renderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

			VkAttachmentDescription colorAttachmentResolve{};
			colorAttachmentResolve.format = colorFormat;
			colorAttachmentResolve.samples = msaaSamples;
			colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			VkAttachmentReference colorAttachmentResolveRef{};
			colorAttachmentResolveRef.attachment = 1;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			m_renderPass.AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

			// Create Dependency
			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			m_renderPass.AddSubpassDependency(dependency);

			VkSubpassDescription subpass = {};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &colorAttachmentRef;
			subpass.pResolveAttachments = &colorAttachmentResolveRef;
			m_renderPass.AddSubpass(subpass);

			m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews());
			//m_renderPass.ConfigureFrameBufferImageViews(m_winSystem->GetSwapChainImageViews());
		}
		else
		{
			m_renderPass.EnableDepthBuffering();
			m_renderPass.EnableMsaa();
			VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // F_VulkanManager->GetMaxSamples();
			VkFormat colorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
			m_renderPass.SetImageColorFormat(colorFormat);
			m_renderPass.SetMSAASampleCount(msaaSamples);

			VkAttachmentDescription colorAttachment{};
			colorAttachment.format = colorFormat;
			colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			VkAttachmentReference colorAttachmentRef{};
			colorAttachmentRef.attachment = 0;
			colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			m_renderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

			VkAttachmentDescription depthAttachment{};
			depthAttachment.format = Helper::FindDepthFormat(m_physicalDevice->GetDevice());
			depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
			depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			VkAttachmentReference depthAttachmentRef{};
			depthAttachmentRef.attachment = 1;
			depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			m_renderPass.AddRenderPassAttachment(depthAttachment, depthAttachmentRef);

			VkAttachmentDescription colorAttachmentResolve{};
			colorAttachmentResolve.format = colorFormat;
			colorAttachmentResolve.samples = msaaSamples;
			colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			VkAttachmentReference colorAttachmentResolveRef{};
			colorAttachmentResolveRef.attachment = 2;
			colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			m_renderPass.AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

			// Create Dependency
			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			m_renderPass.AddSubpassDependency(dependency);

			VkSubpassDescription subpass{};
			subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass.colorAttachmentCount = 1;
			subpass.pColorAttachments = &m_renderPass.GetAttachmentRefs()[0];
			subpass.pDepthStencilAttachment = &m_renderPass.GetAttachmentRefs()[1];
			subpass.pResolveAttachments = &m_renderPass.GetAttachmentRefs()[2];
			m_renderPass.AddSubpass(subpass);

			m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews());
		}

		m_renderPass.Init(*m_commandPool);
	}


	void Material::HandleRenderPass(uint32_t imageIndex)
	{
		if (m_name == "imgui")
		{
			m_renderPass.BeginRenderPass(imageIndex);
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderPass.GetCommandBuffers()[VM_currentFrame]);
			m_renderPass.EndRenderPass();
		}
		else
		{			
			m_renderPass.BeginRenderPass(imageIndex);

			for (std::pair<long, Mesh> mesh : FlatEngine::GetMeshes())
			{
				if (mesh.second.Initialized() && mesh.second.GetMaterialName() == m_name)
				{
					mesh.second.GetModel().UpdateUniformBuffer(imageIndex, *m_winSystem, &mesh.second);
					RecordDefaultCommandBuffer(imageIndex, mesh.second);
					m_renderPass.DrawIndexed(mesh.second);
				}
			}

			m_renderPass.EndRenderPass();

			// Once VkImage has been written to from m_renderPass, that image can be used as a texture to render to using a different material and desired descriptorSets, so we'd need to create descriptor sets for it using that material's configuration
			for (std::map<std::string, std::vector<VkDescriptorSet>*>::iterator materialDescriptorPair = m_renderTextureMaterialDescriptorSets.begin(); materialDescriptorPair != m_renderTextureMaterialDescriptorSets.end(); materialDescriptorPair++)
			{
				std::string materialName = materialDescriptorPair->first;
				std::shared_ptr<Material> renderToMaterial = F_VulkanManager->GetMaterial(materialName);
				Model emptyModel = Model();
				std::vector<Texture> textures = std::vector<Texture>(1, m_renderTexture); // m_renderTexture was given to m_renderPass and if m_name != imgui, it was written to in m_renderPass.DrawIndexed()
				renderToMaterial->GetAllocator().AllocateDescriptorSets(*materialDescriptorPair->second, emptyModel, textures);
			}
		}
	}

	RenderPass& Material::GetRenderPass()
	{
		return m_renderPass;
	}

	void Material::AddMaterialDescriptorSetToRenderTo(std::string materialName, std::vector<VkDescriptorSet>* descriptorSetsToWriteTo)
	{
		std::pair<std::string, std::vector<VkDescriptorSet>*> materialDescriptorSetPair = { materialName, descriptorSetsToWriteTo };

		m_renderTextureMaterialDescriptorSets.emplace(materialDescriptorSetPair);
	}

	void Material::RecordDefaultCommandBuffer(uint32_t imageIndex, Mesh& mesh)
	{
		std::vector<VkCommandBuffer>& commandBuffers = m_renderPass.GetCommandBuffers();
		VkPipeline& graphicsPipeline = mesh.GetMaterial()->GetGraphicsPipeline();
		VkPipelineLayout& pipelineLayout = mesh.GetMaterial()->GetPipelineLayout();
		Transform* transform = mesh.GetParent()->GetTransform();
		Vector3 meshPosition = transform->GetPosition();
		glm::mat4 meshScale = transform->GetScaleMatrix();
		glm::mat4 meshRotation = transform->GetRotationMatrix();
		Camera* primaryCamera = FlatEngine::GetPrimaryCamera();
		Vector3 cameraPosition = primaryCamera->GetParent()->GetTransform()->GetPosition();
		Vector3 lookDir = primaryCamera->GetLookDirection();
		float nearClip = primaryCamera->GetNearClippingDistance();
		float farClip = primaryCamera->GetFarClippingDistance();
		float perspectiveAngle = primaryCamera->GetPerspectiveAngle();
		glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

		static auto startTime = std::chrono::high_resolution_clock::now();
		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
		glm::vec4 meshPos = glm::vec4(meshPosition.x, meshPosition.y, meshPosition.z, 0);
		glm::vec4 viewportCameraPos = glm::vec4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 0);
		glm::mat4 model = meshRotation * meshScale;
		glm::vec4 cameraLookDir = glm::vec4(lookDir.x, lookDir.y, lookDir.z, 0);
		glm::mat4 view = glm::lookAt(cameraPosition.GetGLMVec3(), glm::vec3(cameraPosition.x + cameraLookDir.x, cameraPosition.y + cameraLookDir.y, cameraPosition.z + cameraLookDir.z), up); // Look at camera direction not working right...
		float aspectRatio = (float)(m_winSystem->GetExtent().width / m_winSystem->GetExtent().height);
		glm::mat4 projection = glm::perspective(glm::radians(perspectiveAngle), aspectRatio, nearClip, farClip);
		projection[1][1] *= -1;

		PushConstants pushConstants;
		pushConstants.time = glm::vec4(time);
		pushConstants.meshPosition = meshPos;
		pushConstants.cameraPosition = viewportCameraPos;
		pushConstants.model = model;
		pushConstants.view = view;
		pushConstants.projection = projection;

		uint32_t pushOffset = 0;
		uint32_t pushSize = sizeof(PushConstants);

		vkCmdPushConstants(commandBuffers[VM_currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, pushOffset, pushSize, &pushConstants);

		vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}
}