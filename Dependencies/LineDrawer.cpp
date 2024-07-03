
// Author: Laochra Murray

#include "LineDrawer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#undef GLM_ENABLE_EXPERIMENTAL

#include "view/shader.h"

#include "Debug.h"

LineDrawer::~LineDrawer() noexcept
{
	if (initialised)
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &positionBuffer);
		glDeleteBuffers(1, &colourBuffer);
	}
}

void LineDrawer::Draw(glm::mat4 pvMatrix) noexcept
{
	if (!initialised) Initialise();

	// Bind Vertex Array Object
	glBindVertexArray(vao);

	// Upload Positions and Colours
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * colours.size(), colours.data(), GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	// Bind Program and View Projection Matrix
	
	glUseProgram(linesProgram);
	glUniformMatrix4fv(glGetUniformLocation(linesProgram, "PVMatrix"), 1, GL_FALSE, (float*)&pvMatrix);

	// Draw the Lines!
	glDrawArrays(GL_LINES, 0, (int)positions.size());


	// Unbind Buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	positions.clear();
	colours.clear();
}

void LineDrawer::SetColour(glm::vec4 colour) noexcept
{
	currentColour = colour;
}

void LineDrawer::Add(glm::vec3 start, glm::vec3 end) noexcept
{
	Add(start, end, currentColour);
}
void LineDrawer::Add(glm::vec3 start, glm::vec3 end, glm::vec4 colour) noexcept
{
	positions.push_back(start);
	colours.push_back(colour);

	positions.push_back(end);
	colours.push_back(colour);
}

void LineDrawer::AddXYZLines() noexcept
{
	Add(glm::vec3(0), glm::vec3(1, 0, 0), glm::vec4(1, 0, 0, 1));
	Add(glm::vec3(0), glm::vec3(0, 1, 0), glm::vec4(0, 1, 0, 1));
	Add(glm::vec3(0), glm::vec3(0, 0, 1), glm::vec4(0, 0, 1, 1));
}

void LineDrawer::AddGrid(glm::vec3 centre, float size) noexcept
{
	AddGrid(centre, size, currentColour);
}
void LineDrawer::AddGrid(glm::vec3 centre, float size, glm::vec4 colour) noexcept
{
	glm::vec3 a, b;
	for (int x = (int)-size; x <= (int)size; x++)
	{
		a = centre + glm::vec3(x, 0, -size);
		b = centre + glm::vec3(x, 0, size);
		Add(a, b, colour);
	}
	for (int z = (int)-size; z <= (int)size; z++)
	{
		a = centre + glm::vec3(-size, 0, z);
		b = centre + glm::vec3(size, 0, z);
		Add(a, b, colour);
	}
}

void LineDrawer::AddCuboid(glm::vec3 centre, glm::vec3 size) noexcept
{
	AddCuboid(centre, size, currentColour);
}

void LineDrawer::AddCuboid(glm::vec3 centre, glm::vec3 size, glm::vec4 colour) noexcept
{
	glm::vec3 halfSize = size * 0.5f;
	
	#define PointAAA glm::vec3(centre.x - halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z)
	#define PointAAB glm::vec3(centre.x - halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z)
	#define PointABA glm::vec3(centre.x - halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z)
	#define PointABB glm::vec3(centre.x - halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z)
	#define PointBAA glm::vec3(centre.x + halfSize.x, centre.y - halfSize.y, centre.z - halfSize.z)
	#define PointBAB glm::vec3(centre.x + halfSize.x, centre.y - halfSize.y, centre.z + halfSize.z)
	#define PointBBA glm::vec3(centre.x + halfSize.x, centre.y + halfSize.y, centre.z - halfSize.z)
	#define PointBBB glm::vec3(centre.x + halfSize.x, centre.y + halfSize.y, centre.z + halfSize.z)
	
	Add(PointAAB, PointABB, colour);
	Add(PointABB, PointBBB, colour);
	Add(PointBBB, PointBAB, colour);
	Add(PointBAB, PointAAB, colour);

	Add(PointAAA, PointABA, colour);
	Add(PointABA, PointBBA, colour);
	Add(PointBBA, PointBAA, colour);
	Add(PointBAA, PointAAA, colour);

	Add(PointAAB, PointAAA, colour);
	Add(PointABB, PointABA, colour);
	Add(PointBBB, PointBBA, colour);
	Add(PointBAB, PointBAA, colour);

	#undef PointAAA
	#undef PointAAB
	#undef PointABA
	#undef PointABB
	#undef PointBAA
	#undef PointBAB
	#undef PointBBA
	#undef PointBBB
}

void LineDrawer::AddSphere(glm::vec3 centre, float radius, int ringSides) noexcept
{
	AddSphere(centre, radius, ringSides, currentColour);
}

void LineDrawer::AddSphere(glm::vec3 centre, float radius, int ringSides, glm::vec4 colour) noexcept
{
	const float phi = 2.0f * glm::pi<float>() / ringSides;

	glm::vec3 previousPoint{}, firstPoint{};
	glm::mat4 newRotation{};
	glm::vec3 newDirection{}, newPoint{};

	glm::vec3 currentAxis = glm::vec3(1, 0, 0);

	for (int axisNum = 0; axisNum < 3; axisNum++)
	{
		for (int i = 0; i < ringSides; i++)
		{
			newRotation = glm::rotate((float)i * phi, glm::vec3(1, 0, 0));
			newRotation = glm::rotate(newRotation, glm::radians(90.0f), currentAxis);
			newDirection = (glm::vec3)glm::normalize(glm::inverse(newRotation)[2]);
			newPoint = centre + newDirection * radius;

			if (i > 0)
			{
				Add(previousPoint, newPoint, colour);
				if (i == ringSides - 1) { Add(newPoint, firstPoint, colour); }
			}
			else { firstPoint = newPoint; }
			previousPoint = newPoint;
		}

		currentAxis = glm::vec3(currentAxis.z, currentAxis.x, currentAxis.y);
	}
}

void LineDrawer::AddCone(glm::vec3 point, glm::vec3 direction, float range, float baseRadius, int baseSides) noexcept
{
	AddCone(point, direction, range, baseRadius, baseSides, currentColour);
}

void LineDrawer::AddCone(glm::vec3 point, glm::vec3 direction, float range, float baseRadius, int baseSides, glm::vec4 colour) noexcept
{
	const glm::vec3 baseCentre = point + (direction * range);

	const float phi = 2.0f * glm::pi<float>() / baseSides;
	const float theta = std::atan(baseRadius / range);

	//TODO: This isn't actually facing the correct way it turns out. Not sure if its this or the light thats set up wrong
	const glm::mat4 coneRotation = glm::inverse(glm::lookAt(point, baseCentre, { 0, 1, 0 }));

	glm::vec3 previousPoint{}, firstPoint{};
	glm::mat4 newRotation{};
	glm::vec3 newDirection{}, newPoint{};
	for (int i = 0; i < baseSides; i++)
	{
		newRotation = glm::rotate(coneRotation, phi * i, { 0, 0, 1 });
		newRotation = glm::rotate(newRotation, theta, { 1, 0, 0 });

		newDirection = glm::normalize(-newRotation[2]);
		newPoint = point + newDirection * sqrt(range * range + baseRadius * baseRadius);

		Add(point, newPoint, colour);

		if (i > 0)
		{
			Add(previousPoint, newPoint, colour);
			if (i == baseSides - 1)
			{
				Add(newPoint, firstPoint, colour);
			}
		}
		else
		{
			firstPoint = newPoint;
		}
		previousPoint = newPoint;
	}
}

void LineDrawer::AddConicalFrustum(glm::vec3 point, glm::vec3 direction, float innerRange, float outerRange, float angle, int baseSides) noexcept
{
	AddConicalFrustum(point, direction, innerRange, outerRange, angle, baseSides, currentColour);
}

void LineDrawer::AddConicalFrustum(glm::vec3 point, glm::vec3 direction, float innerRange, float outerRange, float angle, int baseSides, glm::vec4 colour) noexcept
{
	const float innerBaseRadius = innerRange * tan(angle);
	const float outerBaseRadius = outerRange * tan(angle);

	const float phi = 2.0f * glm::pi<float>() / baseSides;
	const float theta = angle;

	const glm::mat4 coneRotation = glm::lookAt(point, point + direction, { 0, 1, 0 });

	glm::vec3 previousInnerPoint{}, firstInnerPoint{};
	glm::vec3 previousOuterPoint{}, firstOuterPoint{};
	for (int i = 0; i < baseSides; i++)
	{
		glm::mat4 newRotation = glm::rotate(coneRotation, phi * i, { 0, 0, 1 });
		newRotation = glm::rotate(newRotation, theta, { 1, 0, 0 });

		glm::vec3 newDirection = glm::normalize(-newRotation[2]);
		glm::vec3 newInnerPoint = point + newDirection * sqrt(sqr(innerRange) + sqr(innerBaseRadius));
		glm::vec3 newOuterPoint = point + newDirection * sqrt(sqr(outerRange) + sqr(outerBaseRadius));

		Add(newInnerPoint, newOuterPoint, colour);

		if (i > 0)
		{
			Add(previousInnerPoint, newInnerPoint, colour);
			Add(previousOuterPoint, newOuterPoint, colour);
			if (i == baseSides - 1)
			{
				Add(newInnerPoint, firstInnerPoint, colour);
				Add(newOuterPoint, firstOuterPoint, colour);
			}
		}
		else
		{
			firstInnerPoint = newInnerPoint;
			firstOuterPoint = newOuterPoint;
		}
		previousInnerPoint = newInnerPoint;
		previousOuterPoint = newOuterPoint;
	}
}

void LineDrawer::Initialise() noexcept
{
	initialised = true;

	glLineWidth(3.0f);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &positionBuffer);
	glGenBuffers(1, &colourBuffer);

	linesProgram = MakeShader("src/shaders/lines.vert", "src/shaders/lines.frag");
}
