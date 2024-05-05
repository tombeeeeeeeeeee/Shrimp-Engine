#pragma once
#include "../config.h"
#include "../components/ComponentInclude.h"
#include "../factories/AssetFactory.h"
/// <summary>
/// Managers all componetns and entities in a scene
/// </summary>
class SceneManager
{
public:

	SceneManager(AssetFactory& _assFact);
	~SceneManager();
	SceneManager(SceneManager& sm) :assFact(sm.assFact) {}
	SceneManager& operator=(SceneManager const& other);

	std::unordered_map<unsigned int, TransformComponent>* GetTransforms();
	std::unordered_map<unsigned int, RenderComponent>* GetRenders();
	std::unordered_map<unsigned int, LightComponent>* GetLights();

	/// <summary>
	/// Makes entity and adds to the entity count.
	/// </summary>
	/// <returns> unsigned int of entity</returns>
	unsigned int MakeEntity() { return entityCount++; }

	/// <summary>
	/// Creates an entity with a transform and camera component
	/// </summary>
	/// <param name="position"> transform position</param>
	/// <param name="eulers"> camera position </param>
	/// <returns> unsigned int of entity</returns>
	unsigned int MakeCamera(glm::vec3 position, glm::vec3 eulers);

#pragma region Transform Components

	/// <summary>
	/// Makes a transform component with a position at 0,0,0
	/// </summary>
	/// <returns>unsigned int transform component entity id</returns>
	unsigned int MakeEmptyTransform();

	/// <summary>
	/// Makes a transform component
	/// </summary>
	/// <param name="position">position of component</param>
	/// <param name="eulers">euler of component</param>
	/// <returns>unsigned int transform component entity id</returns>
	unsigned int MakeEmptyTransform(glm::vec3 position, glm::vec3 eulers = {0,0,0});

	const glm::vec3 GetPosition(unsigned int entity);
	const glm::vec3 GetLocalPosition(unsigned int entity);
	void SetLocalPosition(unsigned int entity, glm::vec3 position);

	const glm::vec3 GetEulers(unsigned int entity, bool radians = true);
	const glm::vec3 GetLocalEulers(unsigned int entity, bool radians = true);
	void SetLocalEulers(unsigned int entity, glm::vec3 eulers, bool radians = true);

	const glm::vec3 GetScale(unsigned int entity);
	void SetScale(unsigned int entity, glm::vec3 scale);

	const glm::mat4 GetLocalTransform(unsigned int entity);
	const glm::mat4 GetGlobalTransform(unsigned entity);

#pragma endregion

#pragma region Light Components
	/// <summary>
	/// Creates an ambient light entity
	/// </summary>
	/// <param name="colour">colour for the light</param>
	/// <param name="intensity"> intensity of the colour </param>
	/// <returns> entity id </returns>
	unsigned int MakeAmbientLightEntity(glm::vec3 colour, float intensity = 1);

	/// <summary>
	/// Adds an ambient light to an entity
	/// </summary>
	/// <param name="entity">entity to add component</param>
	/// <param name="colour">colour for the light</param>
	/// <param name="intensity"> intensity of the colour </param>
	/// <returns> entity id </returns>
	unsigned int AddAmbientLightComponent(unsigned int entity, glm::vec3 colour, float intensity = 1);
	unsigned int MakeDirectionalLightEntity(glm::vec3 direction, glm::vec3 colour, float intensity = 1);
	unsigned int AddDirectionalLightComponent(unsigned int entity, glm::vec3 direction, glm::vec3 colour, float intensity = 1);
	unsigned int MakePointLightEntity(glm::vec3 pos, float range, glm::vec3 colour, float intensity = 1, bool debug = true);
	unsigned int AddPointLightComponent(unsigned int entity, float range, glm::vec3 colour, float intensity = 1);
	unsigned int MakeSpotLightEntity(glm::vec3 pos, glm::vec3 dir, float range, float cutOff, float outerCutOff, glm::vec3 colour, float intensity = 1, bool debug = true);
	unsigned int AddSpotLightComponent(unsigned int entity, glm::vec3 dir, float range, float cutOff, float outerCutOff, glm::vec3 colour, float intensity = 1);

	/// <summary>
	/// Adds a new light component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns>light component</returns>
	LightComponent AddLightComponent(unsigned int _entity);

	/// <summary>
	/// Adds a new light component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="light"> light component to add to entity</param>
	/// <returns>light component</returns>
	LightComponent AddLightComponent(unsigned int _entity, LightComponent& light);

	void CalculateLinearQuadConstants(unsigned int entity);
	void CalculateLinearQuadConstants(float range, float& linear, float& quad);
	const LightType GetLightType(unsigned int entity);
	void SetLightType(unsigned int entity, LightType lightType);

	const glm::vec3 GetLightColour(unsigned int entity);
	void SetLightColour(unsigned int entity, glm::vec3 colour, float intensity);

	const glm::vec3 GetLightDirection(unsigned int entity);
	void SetLightDirection(unsigned int entity, glm::vec3 direction);

	const float GetLightRange(unsigned int entity);
	void SetLightRange(unsigned int entity, float range);
	const float GetLinearAttenuation(unsigned int entity);
	const float GetQuadraticAttenuation(unsigned int entity);

	const float GetLightCutOff(unsigned int entity);
	void SetLightCutOff(unsigned int entity, float cutOff);

	const float GetLightOuterCutOff(unsigned int entity);
	void SetLightOuterCutOff(unsigned int entity, float outerCutOff);

#pragma endregion 

#pragma region Render Components

	/// <summary>
	/// Add a new render component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns> new render component</returns>
	RenderComponent AddRenderComponent(unsigned int _entity);

	/// <summary>
	/// Add a provided render component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="rend"> render component to add to entity</param>
	/// <returns> render component</returns>
	RenderComponent AddRenderComponent(unsigned int _entity, RenderComponent& rend);

	const MaterialAsset* GetMaterial(unsigned int entity);
	void SetMaterial(unsigned int entity, MaterialAsset* mat);

	const MeshAsset* GetMesh(unsigned int entity);
	void SetMesh(unsigned int entity, MeshAsset* mesh);

#pragma endregion

	/// <summary>
	/// Adds a new physics component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns>physics component</returns>
	PhysicsComponent AddPhysicsComponent(unsigned int _entity);

#pragma region Hierarchy Systems

	/// <summary>
	/// Recursivly updates global transfrom from parent to child.
	/// </summary>
	/// <param name="entity"> Transform ID to update</param>
	/// <param name="transformComponents"> All transform components</param>
	void GlobalTransformUpdate(unsigned int entity, bool first = true);

	/// <summary>
	/// Changes a transforms parent, and changes the parents listed children.
	/// </summary>
	/// <param name="child"> Transform ID of Child</param>
	/// <param name="parent"> Transform ID of Parent</param>
	void SetParent(unsigned int child, unsigned int parent = 0);


	void HierarchyUpdate();
#pragma endregion

private:
	/// <summary>
	/// Starting entity count value (must be above 0)
	/// </summary>
	unsigned int entityCount = 1;

	/// <summary>
	/// Light Components in scene
	/// </summary>
	std::unordered_map<unsigned int, LightComponent> lightComponents;

	/// <summary>
	/// Physics Components in scene
	/// </summary>
	std::unordered_map<unsigned int, PhysicsComponent> physicsComponents;

	/// <summary>
	/// Render Components in scene
	/// </summary>
	std::unordered_map<unsigned int, RenderComponent> renderComponents;

	/// <summary>
	/// Transform Components in scene
	/// </summary>
	std::unordered_map<unsigned int, TransformComponent> transformComponents;

	std::unordered_map<unsigned int, bool> UpdatedGlobalTransform;
	std::vector<unsigned int> TransformsToUpdate;

	/// <summary>
	/// Asset Factory for asset making
	/// </summary>
	AssetFactory& assFact;
};

