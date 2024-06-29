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

void EditorGUISystem::DrawTransformLabel(unsigned int entityID, TransformComponent& transform, SceneManager * scene) noexcept
{
	const unsigned int& id = entityID;
	std::string label = "Entity " + std::to_string(entityID);

	bool isSelected = entityID == selectedID;
	bool isLeaf = transform.children.size() == 0;

	int flags = 0;
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
	flags |= ImGuiTreeNodeFlags_OpenOnArrow;
	flags |= ImGuiTreeNodeFlags_OpenOnDoubleClick;
	flags |= ImGuiTreeNodeFlags_FramePadding;
	flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
	flags |= ImGuiTreeNodeFlags_Selected * isSelected;
	flags |= ImGuiTreeNodeFlags_Leaf * isLeaf;

	if (ImGui::TreeNodeEx(label.c_str(), flags))
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
			{
				selectedID = entityID;
			}
		}
		ImGui::TreePop();
	}
	else
	{
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped))
		{
			if(ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
			{
				selectedID = entityID;
			}
		}
	}

	for (unsigned int i = 0; i < transform.children.size(); i++)
	{
		unsigned int childID = transform.children[i];
		ImGui::Indent();
		DrawTransformLabel(childID, (*scene->GetTransforms())[childID], scene);
		ImGui::Unindent();
	}
}

void EditorGUISystem::Update(GLFWwindow* window, SceneManager* scene) noexcept
{
	ReadyImGui();


	// Specify All The ImGui Data
	ImGui::Begin("Transform Hierarchy");
	for (std::pair<const unsigned int, TransformComponent>& transformPair : *scene->GetTransforms())
	{
		if (transformPair.second.parent != 0) return;

		unsigned int entityID = transformPair.first;
		TransformComponent& transform = transformPair.second;

		DrawTransformLabel(entityID, transform, scene);
	}
	ImGui::End();

	ImGui::Begin("Inspector");
	if (selectedID != 0)
	{
		ImGui::Text(("EntityID: " + std::to_string(selectedID)).c_str());
		
		ImGui::Spacing();

		auto* transforms = scene->GetTransforms();
		if (transforms->find(selectedID) != transforms->end())
		{
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				TransformComponent& transform = (*transforms)[selectedID];

				ImGui::DragFloat3("Position", (float*)&transform.position, 0.25f);
				ImGui::BeginDisabled();
				ImGui::DragFloat4("Rotation", (float*)&transform.rotation);
				ImGui::EndDisabled();
				ImGui::DragFloat3("Scale", (float*)&transform.scale, 0.25f);
			}
		}
		
		auto* lights = scene->GetLights();
		if (lights->find(selectedID) != lights->end())
		{
			if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
			{
				LightComponent& light = (*lights)[selectedID];

				ImGui::ColorEdit3("Colour", (float*)&light.colour);
				ImGui::Text("Some other editable fields go here");
			}
		}


	}
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
