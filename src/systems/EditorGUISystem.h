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
	void Update(GLFWwindow* window, SceneManager* scene) noexcept;

private:
	unsigned int selectedID = 0;

	// Clear ImGui Data for New Frame
	void ReadyImGui() const noexcept;
	// Actually Draws the ImGui Data
	void Draw(GLFWwindow* window) const noexcept;

	void DrawTransformLabel(unsigned int entityID, TransformComponent& transform, SceneManager* scene) noexcept;
};
