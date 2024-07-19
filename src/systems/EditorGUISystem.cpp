#include "EditorGUISystem.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

#include <sstream>

#include "InputSystem.h"

EditorGUISystem::EditorGUISystem(GLFWwindow* window) noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;			// Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;		// Enable Viewports

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	//ImGui::GetIO().IniFilename = ;	// Set the filepath for imgui's config file

	// Setting Up Styling
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		ImGui::GetIO().Fonts->AddFontFromFileTTF("Assets\\fonts\\ComicMono.ttf", 16);

		style->ItemInnerSpacing = { 3.0f, 3.0f };
		style->ItemSpacing = { 3.0f, 3.0f };

		style->TabRounding = 4.0f;
		style->TabBarBorderSize = 2.0f;

		style->FrameRounding = 4.0f;
		style->FramePadding = { 4.0f, 3.5f };

		style->PopupRounding = 4.0f;

		style->WindowRounding = 4.0f;
		style->WindowPadding = { 10.0f, 10.0f };

		style->SelectableRounding = 5.0f;

		style->ScrollbarRounding = 4.0f;
		style->ScrollbarSize = 16.0f;

		// Text
		style->Colors[ImGuiCol_Text]						= ImVec4(0.000, 0.000, 0.000, 1.0);
		style->Colors[ImGuiCol_TextDisabled]			= ImVec4(0.500, 0.500, 0.500, 1.0);
		style->Colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.960, 0.670, 0.725, 1.0);

		// Backgrounds
		style->Colors[ImGuiCol_WindowBg]					= ImVec4(1.000, 1.000, 1.000, 1.0);
		style->Colors[ImGuiCol_ChildBg]					= ImVec4(1.000, 1.000, 1.000, 1.0);
		style->Colors[ImGuiCol_PopupBg]					= ImVec4(1.000, 1.000, 1.000, 1.0);
		style->Colors[ImGuiCol_TitleBg]					= ImVec4(1.000, 1.000, 1.000, 1.0);
		style->Colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(1.000, 1.000, 1.000, 1.0);
		style->Colors[ImGuiCol_ResizeGrip]				= ImVec4(1.000, 1.000, 1.000, 1.0);
		style->Colors[ImGuiCol_TitleBgActive]			= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_MenuBarBg]				= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_ScrollbarBg]				= ImVec4(0.960, 0.670, 0.725, 1.0);

		// Input Fields
		style->Colors[ImGuiCol_FrameBg]					= ImVec4(0.626, 0.897, 1.000, 1.0);
		style->Colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.586, 0.848, 0.947, 1.0);
		style->Colors[ImGuiCol_FrameBgActive]			= ImVec4(0.558, 0.808, 0.903, 1.0);
		style->Colors[ImGuiCol_CheckMark]				= ImVec4(0.356, 0.811, 0.984, 1.0);
		style->Colors[ImGuiCol_DragDropTarget]			= ImVec4(0.356, 0.811, 0.984, 1.0);

		// Buttons & Resize
		style->Colors[ImGuiCol_Button]					= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_ButtonHovered]			= ImVec4(0.911, 0.644, 0.694, 1.0);
		style->Colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.911, 0.644, 0.694, 1.0);
		style->Colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.911, 0.644, 0.694, 1.0);
		style->Colors[ImGuiCol_ButtonActive]			= ImVec4(0.871, 0.612, 0.661, 1.0);
		style->Colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.871, 0.612, 0.661, 1.0);

		// Foldable Headers
		style->Colors[ImGuiCol_Header]					= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_TableHeaderBg]			= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_HeaderHovered]			= ImVec4(0.911, 0.644, 0.694, 1.0);
		style->Colors[ImGuiCol_HeaderActive]			= ImVec4(0.871, 0.612, 0.661, 1.0);

		// Tabs
		style->Colors[ImGuiCol_Tab]						= ImVec4(0.356, 0.811, 0.984, 1.0);
		style->Colors[ImGuiCol_TabHovered]				= ImVec4(0.347, 0.785, 0.951, 1.0);
		style->Colors[ImGuiCol_TabActive]				= ImVec4(0.239, 0.741, 0.931, 1.0);
		style->Colors[ImGuiCol_TabUnfocused]			= ImVec4(0.404, 0.807, 0.959, 1.0);
		style->Colors[ImGuiCol_TabUnfocusedActive]	= ImVec4(0.336, 0.759, 0.919, 1.0);

		// Scrollbar Handles
		style->Colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.356, 0.811, 0.984, 1.0);
		style->Colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.347, 0.785, 0.951, 1.0);
		style->Colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.239, 0.741, 0.931, 1.0);


		// Borders & Bars
		style->Colors[ImGuiCol_Separator]				= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_Border]					= ImVec4(0.356, 0.811, 0.984, 1.0);

		// Tables
		style->Colors[ImGuiCol_TableBorderStrong]		= ImVec4(0.356, 0.811, 0.984, 1.0);
		style->Colors[ImGuiCol_TableBorderLight]		= ImVec4(0.356, 0.811, 0.984, 0.25);
		style->Colors[ImGuiCol_TableRowBg]				= ImVec4(1.000, 1.000, 1.000, 0.25);
		style->Colors[ImGuiCol_TableRowBgAlt]			= ImVec4(0.960, 0.670, 0.725, 0.1);

		// Docking
		style->Colors[ImGuiCol_DockingPreview]			= ImVec4(0.960, 0.670, 0.725, 1.0);
		style->Colors[ImGuiCol_DockingEmptyBg]			= ImVec4(0.309, 0.612, 0.726, 1.0);
	}
}

EditorGUISystem::~EditorGUISystem() noexcept
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorGUISystem::Update(GLFWwindow* window, SceneManager* scene, unsigned int sceneView) noexcept
{
	ReadyImGui();


	// Specify All The ImGui Data
	DrawTransformHierarchy(scene);
	DrawInspector(scene);
	DrawSceneView(sceneView);

	Render(window);
}

GLFWwindowsizefun EditorGUISystem::resizeFunction = nullptr;
void EditorGUISystem::SetResizeFunctionCallback(GLFWwindowsizefun resizeFunctionInit)
{
	resizeFunction = resizeFunctionInit;
}

void EditorGUISystem::ReadyImGui() const noexcept
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void EditorGUISystem::Render(GLFWwindow* window) const noexcept
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(window);
	}
}

void EditorGUISystem::DrawTransformHierarchy(SceneManager* scene)
{
	ImGui::Begin("Transform Hierarchy");
	for (std::pair<const unsigned int, TransformComponent>& transformPair : *scene->GetTransforms())
	{
		if (transformPair.second.parent != 0) return;

		unsigned int entityID = transformPair.first;
		TransformComponent& transform = transformPair.second;

		DrawTransformHierarchyElement(entityID, transform, scene);
	}
	ImGui::End();
}
void EditorGUISystem::DrawTransformHierarchyElement(unsigned int entityID, TransformComponent& transform, SceneManager* scene) noexcept
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
			if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
			{
				selectedID = entityID;
			}
		}
	}

	for (unsigned int i = 0; i < transform.children.size(); i++)
	{
		unsigned int childID = transform.children[i];
		ImGui::Indent();
		DrawTransformHierarchyElement(childID, (*scene->GetTransforms())[childID], scene);
		ImGui::Unindent();
	}
}

void EditorGUISystem::DrawInspector(SceneManager* scene)
{
	ImGui::Begin("Inspector");
	if (selectedID != 0)
	{
		ImGui::Text(("EntityID: " + std::to_string(selectedID)).c_str());

		auto* transformComponents = scene->GetTransforms();
		bool hasTransform = transformComponents->find(selectedID) != transformComponents->end();
		auto* renderComponents = scene->GetRenders();
		bool hasRender = renderComponents->find(selectedID) != renderComponents->end();
		auto* physicsComponents = scene->GetPhysics();
		bool hasPhysics = physicsComponents->find(selectedID) != physicsComponents->end();
		auto* lightComponents = scene->GetLights();
		bool hasLight = lightComponents->find(selectedID) != lightComponents->end();
		//auto* cameraComponents = scene->GetCameras();
		//const bool hasCamera = cameraComponents->find(selectedID) != cameraComponents->end();

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		if (ImGui::BeginCombo("Modify Components", "", ImGuiComboFlags_NoPreview))
		{
			if (ImGui::Selectable("Render Component", hasRender))
			{
				if (!hasRender) scene->AddRenderComponent(selectedID);
				else scene->GetRenders()->erase(selectedID);
				hasRender = !hasRender;
			}
			ImGui::Spacing();
			if (ImGui::Selectable("Physics Component", hasPhysics))
			{
				if (!hasPhysics) scene->AddPhysicsComponent(selectedID);
				else scene->GetPhysics()->erase(selectedID);
				hasPhysics = !hasPhysics;
			}
			ImGui::Spacing();
			if (ImGui::Selectable("Light Component", hasLight))
			{
				if (!hasLight) scene->AddLightComponent(selectedID);
				else scene->GetLights()->erase(selectedID);
				hasLight = !hasLight;
			}
			ImGui::Spacing();

			ImGui::EndCombo();
		}

		ImGui::Spacing();

		if (hasTransform && ImGui::CollapsingHeader("Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
		{
			TransformComponent& transform = (*transformComponents)[selectedID];

			ImGui::DragFloat3("Position", (float*)&transform.position, 0.25f);

			glm::vec3 rotationEuler = glm::eulerAngles(transform.rotation);
			glm::vec3 rotationDegrees = glm::vec3(glm::degrees(rotationEuler.x), glm::degrees(rotationEuler.y), glm::degrees(rotationEuler.z));
			glm::vec3 originalDegrees = rotationDegrees;
			if (ImGui::DragFloat3("Rotation", (float*)&rotationDegrees))
			{
				rotationDegrees -= originalDegrees;
				rotationEuler = glm::vec3(glm::radians(rotationDegrees.x), glm::radians(rotationDegrees.y), glm::radians(rotationDegrees.z));

				glm::quat quatZ = glm::angleAxis(rotationEuler.z, glm::vec3(0, 0, 1));
				glm::quat quatY = glm::angleAxis(rotationEuler.y, glm::vec3(0, 1, 0));
				glm::quat quatX = glm::angleAxis(rotationEuler.x, glm::vec3(1, 0, 0));

				transform.rotation = glm::normalize(quatZ * quatY * quatX) * transform.rotation;
			}

			ImGui::DragFloat3("Scale", (float*)&transform.scale, 0.25f);

			ImGui::Spacing();
		}
		if (hasRender && ImGui::CollapsingHeader("Render Component", ImGuiTreeNodeFlags_DefaultOpen))
		{
			RenderComponent& render = (*renderComponents)[selectedID];

			ImGui::Text(StringBuilder("Material: ", render.material).CStr());
			ImGui::Text(StringBuilder("Mesh: ", render.mesh).CStr());

			ImGui::Spacing();
		}
		if (hasPhysics && ImGui::CollapsingHeader("Physics Component", ImGuiTreeNodeFlags_DefaultOpen))
		{
			PhysicsComponent& physics = (*physicsComponents)[selectedID];
			
			ImGui::Checkbox("Gravity", &physics.isGravitated);
			ImGui::Spacing();
			bool isKinematic = physics.invMass == 0;
			if (ImGui::Checkbox("Kinematic", &isKinematic))
			{
				if (isKinematic) physics.invMass = 0;
				else physics.invMass = 1;
			}
			ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
			if (!isKinematic)
			{
				ImGui::SameLine();
				float mass = 1 / physics.invMass;
				if (ImGui::DragFloat("Mass", &mass))
				{
					physics.invMass = 1 / mass;
				}
			}
			ImGui::Spacing();
			ImGui::DragFloat("Elasticity", &physics.angularDrag);
			ImGui::Spacing();
			ImGui::DragFloat("Drag", &physics.drag);
			ImGui::DragFloat("Angular Drag", &physics.angularDrag);
			ImGui::PopItemWidth();
			
			ImGui::Spacing();
		}
		if (hasLight && ImGui::CollapsingHeader("Light Component", ImGuiTreeNodeFlags_DefaultOpen))
		{
			LightComponent& light = (*lightComponents)[selectedID];
			std::string lightTypeName;
			bool hasDirection = false;
			bool hasRange = false;
			bool hasCutoff = false;
			switch (light.lightType)
			{
			case LightType::ambient:
			{
				lightTypeName = "Ambient";
				break;
			}
			case LightType::directional:
			{
				lightTypeName = "Directional";
				hasDirection = true;
				break;
			}
			case LightType::point:
			{
				lightTypeName = "Point";
				hasRange = true;
				break;
			}
			case LightType::spot:
			{
				lightTypeName = "Spotlight";
				hasDirection = true;
				hasRange = true;
				hasCutoff = true;
				break;
			}
			default:
			{
				lightTypeName = "Invalid";
				break;
			}
			}

			if (ImGui::BeginCombo("Light Type", lightTypeName.c_str()))
			{
				if (ImGui::Selectable("Directional", light.lightType == LightType::directional))
				{
					light.lightType = LightType::directional;
				}
				if (ImGui::Selectable("Point", light.lightType == LightType::point))
				{
					light.lightType = LightType::point;
				}
				if (ImGui::Selectable("Spotlight", light.lightType == LightType::spot))
				{
					if (!hasCutoff)
					{
						light.cutOff = glm::radians(0.0f);
						light.outerCutOff = glm::radians(25.0f);
					}
					light.lightType = LightType::spot;
				}
				ImGui::EndCombo();
			}
			
			ImGui::Spacing();
			
			ImGui::ColorEdit3("Colour", (float*)&light.colour);

			ImGui::Spacing();

			if (hasDirection)
			{
				ImGui::DragFloat3("Direction", (float*)&light.direction);
			}
			if (hasRange)
			{
				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				if (ImGui::DragFloat("Range", (float*)&light.range))
				{
					scene->CalculateLinearQuadConstants(selectedID);
				}
				ImGui::PopItemWidth();
			}
			if (hasCutoff)
			{
				float cutOffDegrees[2] = { glm::degrees(light.cutOff), glm::degrees(light.outerCutOff) };

				ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3);
				if (ImGui::DragFloat("##InnerCutOff", &cutOffDegrees[0], 0.1f, 0.0f, 89.9f, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (cutOffDegrees[0] > cutOffDegrees[1]) cutOffDegrees[1] = cutOffDegrees[0];
					light.cutOff = cutOffDegrees[0];
					light.outerCutOff = cutOffDegrees[1];
				}
				ImGui::SameLine();
				if (ImGui::DragFloat("##OuterCutOff", &cutOffDegrees[1], 0.1f, 0.1f, 89.9f, NULL, ImGuiSliderFlags_AlwaysClamp))
				{
					if (cutOffDegrees[1] < cutOffDegrees[0]) cutOffDegrees[0] = cutOffDegrees[1];
					light.cutOff = cutOffDegrees[0];
					light.outerCutOff = cutOffDegrees[1];
				}
				ImGui::PopItemWidth();
				ImGui::SameLine();
				ImGui::Text("Inner/Outer CutOff");
			}

			ImGui::Spacing();
		}


	}
	ImGui::End();
}

void EditorGUISystem::DrawSceneView(unsigned int renderTexture)
{
	GLFWwindow* window = glfwGetCurrentContext();

	ImVec2 oldPadding = ImGui::GetStyle().WindowPadding;
	ImGui::GetStyle().WindowPadding = ImVec2(0.0f, 0.0f);

	ImGui::Begin("Scene View", (bool*)0, ImGuiWindowFlags_NoCollapse);
	
	float titleBarHeight = ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;
	int newScreenWidth = (int)ImGui::GetWindowWidth();
	int newScreenHeight = (int)(ImGui::GetWindowHeight() - titleBarHeight);
	if (SCREEN_WIDTH != newScreenWidth || SCREEN_HEIGHT != newScreenHeight)
	{
		if (resizeFunction != nullptr) resizeFunction(window, newScreenWidth, newScreenHeight);
	}

	if (SCREEN_WIDTH <= 0 || SCREEN_HEIGHT <= 0)
	{
		ImGui::End();
		ImGui::GetStyle().WindowPadding = oldPadding;
		return;
	}

	ImGui::Image((ImTextureID)(uintptr_t)renderTexture, ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT), ImVec2(0, 1), ImVec2(1, 0));
	inputSystem->enabled = ImGui::IsItemHovered();
	
	ImGui::End();
	ImGui::GetStyle().WindowPadding = oldPadding;
}
