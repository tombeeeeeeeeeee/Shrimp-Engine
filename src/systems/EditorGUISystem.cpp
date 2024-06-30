#include "EditorGUISystem.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

#include <sstream>

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

void EditorGUISystem::Update(GLFWwindow* window, SceneManager* scene) noexcept
{
	ReadyImGui();


	// Specify All The ImGui Data
	DrawTransformHierarchy(scene);
	DrawInspector(scene);


	Render(window);
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

			ImGui::Text(StringBuilder("Material:", render.material).CStr());
			ImGui::Text(StringBuilder("Mesh:", render.mesh).CStr());

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
			LightComponent& light = lightComponents->operator[](selectedID);

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
