#pragma once

// Author: Laochra Murray

#include <config.h>
#include "GeneralMacros.h"

#include <vector>
using std::vector;

typedef unsigned int uint;

// Only use after glad and glfw have been initialised.
class LineDrawer
{
public:
	LineDrawer() noexcept = default;
	~LineDrawer() noexcept;
	uncopyable(LineDrawer)
	immovable(LineDrawer)

	// Draw Lines.
	void Draw(glm::mat4 pvMatrix) noexcept;

	/// <param name="colour:">The colour to be used for lines when none is specified.</param>
	void SetColour(glm::vec4 colour) noexcept;


	/// <summary>Uses current default colour. See overload to specify a colour.</summary>
	void Add(glm::vec3 start, glm::vec3 end) noexcept;
	void Add(glm::vec3 start, glm::vec3 end, glm::vec4 colour) noexcept;
	
	/// <summary>Adds 3 lines displaying XYZ directions coloured RGB.</summary>
	void AddXYZLines() noexcept;
	
	/// <summary>Adds a grid with XYZ coloured RGB. <para>Uses current default colour for grid. See overload to specify a colour.</para></summary> <param name="size">= How many units to span from the specified centre.</param>
	void AddGrid(glm::vec3 centre, float size) noexcept;
	/// <summary>Adds a grid with XYZ coloured RGB.</summary> <param name="size">= How many units to span from the specified centre.</param>
	void AddGrid(glm::vec3 centre, float size, glm::vec4 colour) noexcept;
	
	/// <summary> Adds a cuboid at the specified centre. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="size">= XYZ size lengths of the cuboid</param>
	void AddCuboid(glm::vec3 centre, glm::vec3 size) noexcept;
	/// <summary> Adds a cuboid at the specified centre.</summary> <param name="size">= XYZ size lengths of the cuboid</param>
	void AddCuboid(glm::vec3 centre, glm::vec3 size, glm::vec4 colour) noexcept;

	/// <summary> Adds a sphere at the specified centre with the specified range. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="ringSides">Amount of sides for each ring of the sphere.</param>
	void AddSphere(glm::vec3 centre, float radius, int ringSides) noexcept;
	/// <summary> Adds a sphere at the specified centre with the specified range.</summary> <param name="ringSides">Amount of sides for each ring of the sphere.</param>
	void AddSphere(glm::vec3 centre, float radius, int ringSides, glm::vec4 colour) noexcept;

	/// <summary> Adds a cone originating from the specified point. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="direction">= The direction from the point to the base.</param> <param name="range">= The distance from the point to the base.</param> <param name="baseRadius">= The distance from the centre of the base to each of its points.</param> <param name="baseSides">= The amount of sides the base has.</param>
	void AddCone(glm::vec3 point, glm::vec3 direction, float range, float baseRadius, int baseSides) noexcept;
	/// <summary> Adds a cone originating from the specified point.</summary> <param name="direction">= The direction from the point to the base.</param> <param name="range">= The distance from the point to the base.</param> <param name="baseRadius">= The distance from the centre of the base to each of its points.</param> <param name="baseSides">= The amount of sides the base has.</param>
	void AddCone(glm::vec3 point, glm::vec3 direction, float range, float baseRadius, int baseSides, glm::vec4 colour) noexcept;

	/// <summary> Adds a conical frustum originating from the inner range and extending to the outer range, using point as the origin of the angle. <para>Uses current default colour. See overloads to specify a colour.</para></summary> <param name="direction">= The direction from the point to the outer base.</param> <param name="innerRange">= The distance from the point to the inner base.</param> <param name="outerRange">= The distance from the point to the outer base.</param> <param name="angle">= The angle in radians from the direction to the outside of the conical frustum.</param> <param name="baseSides">= The amount of sides the bases have.</param>
	void AddConicalFrustum(glm::vec3 point, glm::vec3 direction, float innerRange, float outerRange, float angle, int baseSides) noexcept;
	/// <summary> Adds a conical frustum originating from the inner range and extending to the outer range, using point as the origin of the angle. </summary> <param name="direction">= The direction from the point to the outer base.</param> <param name="innerRange">= The distance from the point to the inner base.</param> <param name="outerRange">= The distance from the point to the outer base.</param> <param name="angle">= The angle in radians from the direction to the outside of the conical frustum.</param> <param name="baseSides">= The amount of sides the bases have.</param>
	void AddConicalFrustum(glm::vec3 point, glm::vec3 direction, float innerRange, float outerRange, float angle, int baseSides, glm::vec4 colour) noexcept;

protected:
	vector<glm::vec3> positions;
	vector<glm::vec4> colours;
	
	glm::vec4 currentColour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	uint vao;
	uint positionBuffer;
	uint colourBuffer;

	unsigned int linesProgram;

	bool initialised = false;

	void Initialise() noexcept;
};