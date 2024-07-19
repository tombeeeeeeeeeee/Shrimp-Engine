#pragma once

#include "config.h"
#include "../factories/SceneManager.h"

class EditorGUISystem
{
public:
	static const bool enabled = true;

	EditorGUISystem(GLFWwindow* window) noexcept;
	~EditorGUISystem() noexcept;

	/// <summary>
	/// Draws The GUI For Shrimp Engine's Editor
	/// </summary>
	/// <param name="window">The Main Window for the Application</param>
	void Update(GLFWwindow* window, SceneManager* scene, unsigned int sceneView) noexcept;
	static void SetResizeFunctionCallback(GLFWwindowsizefun resizeFunctionInit);

private:
	unsigned int selectedID = 0;
	static GLFWwindowsizefun resizeFunction;

	// Clear ImGui Data for New Frame
	void ReadyImGui() const noexcept;
	// Actually Render the ImGui Data
	void Render(GLFWwindow* window) const noexcept;

	void DrawTransformHierarchy(SceneManager* scene);
	void DrawTransformHierarchyElement(unsigned int entityID, TransformComponent& transform, SceneManager* scene) noexcept;

	void DrawInspector(SceneManager* scene);

	void DrawSceneView(unsigned int renderTexture);
};
