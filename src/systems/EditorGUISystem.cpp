#include "EditorGUISystem.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

EditorGUISystem::EditorGUISystem(GLFWwindow* window) noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Viewports

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//ImGui::GetIO().IniFilename = ;	// Set the filepath for imgui's config file
}

EditorGUISystem::~EditorGUISystem() noexcept
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorGUISystem::Update(GLFWwindow* window) noexcept
{
	ReadyImGui();


	// Specify All The ImGui Data
	ImGui::Begin("Hello Window!");
	ImGui::Text("Shrimp Engine's first imgui window");
	ImGui::End();


	Draw(window);
}

void EditorGUISystem::ReadyImGui() const noexcept
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void EditorGUISystem::Draw(GLFWwindow* window) const noexcept
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(window);
	}

	glfwSwapBuffers(window);
}
