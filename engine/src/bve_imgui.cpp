#include "bve_imgui.h"

#include "bve_device.h"
#include "bve_window.h"

// libs
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

// std
#include <stdexcept>

#include "components/components.h"

namespace bve
{
	// ok this just initializes imgui using the provided integration files. So in our case we need to
	// initialize the vulkan and glfw imgui implementations, since that's what our engine is built
	// using.
	BveImgui::BveImgui(
		BveWindow& window, BveDevice& device, VkRenderPass renderPass, uint32_t imageCount, EntityManager& entityManager)
		: bveDevice_{device}, entityManager_{entityManager}
	{
		// set up a descriptor pool stored on this instance_, see header for more comments on this.
		VkDescriptorPoolSize pool_sizes[] = {
			{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
		pool_info.pPoolSizes = pool_sizes;
		if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &descriptorPool_) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up descriptor pool for imgui");
		}

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		// ImGui::StyleColorsClassic();

		// Setup Platform/Renderer backends
		// Initialize imgui for vulkan
		ImGui_ImplGlfw_InitForVulkan(window.getGLFWWindow(), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = device.getInstance();
		init_info.PhysicalDevice = device.getPhysicalDevice();
		init_info.Device = device.device();
		init_info.QueueFamily = device.getGraphicsQueueFamily();
		init_info.Queue = device.graphicsQueue();

		// pipeline cache is a potential future optimization, ignoring for now
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descriptorPool_;
		// todo, I should probably get around to integrating a memory allocator library such as Vulkan
		// memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
		// in a ton of locations.
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.MinImageCount = 2;
		init_info.ImageCount = imageCount;
		init_info.CheckVkResultFn = check_vk_result;
		ImGui_ImplVulkan_Init(&init_info, renderPass);

		// upload fonts, this is done by recording and submitting a one time use command buffer
		// which can be done easily bye using some existing helper functions on the bve device object
		auto commandBuffer = device.beginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.endSingleTimeCommands(commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	BveImgui::~BveImgui()
	{
		vkDestroyDescriptorPool(bveDevice_.device(), descriptorPool_, nullptr);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void BveImgui::newFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	// this tells imgui that we're done setting up the current frame,
	// then gets the draw data from imgui and uses it to record to the provided
	// command buffer the necessary draw commands
	void BveImgui::render(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImDrawData* drawdata = ImGui::GetDrawData();
		ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
	}

	void BveImgui::run()
	{
		// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can
		// browse its code to learn more about Dear ImGui!).
		if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create oursebves. We use a Begin/End pair to created a named
		// window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

			ImGui::Text(
				"This is some useful text."); // Display some text (you can use a format strings too)
			ImGui::Checkbox(
				"Demo Window",
				&show_demo_window); // Edit bools storing our window open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color",
			                  (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true
				// when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text(
				"Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
			ImGui::End();
		}

		// 3. Show another simple window.
		if (show_another_window) {
			ImGui::Begin(
				"Another Window",
				&show_another_window); // Pass a pointer to our bool variable (the window will have a
			// closing button that will clear the bool when clicked)
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me")) show_another_window = false;
			ImGui::End();
		}

		ImGui::Begin("Select");
		if (ImGui::BeginListBox("Entities")) {
			for (const auto& [entity, name] : entityManager_.getEntities()) {
				bool wasSelected = false;
				if (entityManager_.hasComponent<SelectedTag>(entity)) {
					wasSelected = true;
				}

				bool isSelected = wasSelected;

				// Selectable returns true if the item is clicked
				// Pass the address of the selected flag to make the widget toggleable
				if (ImGui::Selectable(name.c_str(), &isSelected)) {
					if (isSelected) {
						if (!wasSelected) {
							entityManager_.addComponent<SelectedTag>(entity);
						}
					} else {
						if (wasSelected) {
							entityManager_.removeComponent<SelectedTag>(entity);
						}
					}
				}
			}
			ImGui::EndListBox();
		}
		ImGui::End();
	}
} // namespace bve