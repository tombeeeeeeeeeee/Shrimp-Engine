#pragma once

#include "config.h"

class EditorGUISystem
{
public:
	EditorGUISystem(GLFWwindow* window) noexcept;
	~EditorGUISystem() noexcept;

	/// <summary>
	/// Draws The GUI For Shrimp Engine's Editor
	/// </summary>
	/// <param name="window">The Main Window for the Application</param>
	void Update(GLFWwindow* window) noexcept;

private:
	// Clear ImGui Data for New Frame
	void ReadyImGui() const noexcept;
	// Actually Draws the ImGui Data
	void Draw(GLFWwindow* window) const noexcept;
};
