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

	const std::unordered_map<unsigned int, TransformComponent*> GetTransforms();
	const std::unordered_map<unsigned int, RenderComponent*> GetRenders();
	const std::unordered_map<unsigned int, LightComponent*> GetLights();

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
	unsigned int MakeCamera(vec3 position, vec3 eulers);

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
	unsigned int MakeEmptyTransform(vec3 position, vec3 eulers = {0,0,0});

	const vec3 GetPosition(unsigned int entity);
	const vec3 GetLocalPosition(unsigned int entity);
	void SetLocalPosition(unsigned int entity, vec3 position);

	const vec3 GetEulers(unsigned int entity, bool radians = true);
	const vec3 GetLocalEulers(unsigned int entity, bool radians = true);
	void SetLocalEulers(unsigned int entity, vec3 eulers, bool radians = true);

	const vec3 GetScale(unsigned int entity);
	void SetScale(unsigned int entity, vec3 scale);

	const mat4 GetLocalTransform(unsigned int entity);
	const mat4 GetGlobalTransform(unsigned entity);

#pragma endregion

#pragma region Light Components
	/// <summary>
	/// Creates an ambient light entity
	/// </summary>
	/// <param name="colour">colour for the light</param>
	/// <param name="intensity"> intensity of the colour </param>
	/// <returns> entity id </returns>
	unsigned int MakeAmbientLightEntity(vec3 colour, float intensity = 1);

	/// <summary>
	/// Adds an ambient light to an entity
	/// </summary>
	/// <param name="entity">entity to add component</param>
	/// <param name="colour">colour for the light</param>
	/// <param name="intensity"> intensity of the colour </param>
	/// <returns> entity id </returns>
	unsigned int AddAmbientLightComponent(unsigned int entity, vec3 colour, float intensity = 1);
	unsigned int MakeDirectionalLightEntity(vec3 direction, vec3 colour, float intensity = 1);
	unsigned int AddDirectionalLightComponent(unsigned int entity, vec3 direction, vec3 colour, float intensity = 1);
	unsigned int MakePointLightEntity(vec3 pos, float range, vec3 colour, float intensity = 1, bool debug = true);
	unsigned int AddPointLightComponent(unsigned int entity, float range, vec3 colour, float intensity = 1);
	unsigned int MakeSpotLightEntity(vec3 pos, vec3 dir, float range, float cutOff, float outerCutOff, vec3 colour, float intensity = 1, bool debug = true);
	unsigned int AddSpotLightComponent(unsigned int entity, vec3 dir, float range, float cutOff, float outerCutOff, vec3 colour, float intensity = 1);

	void CalculateLinearQuadConstants(unsigned int entity);

	const LightType GetLightType(unsigned int entity);
	void SetLightType(unsigned int entity, LightType lightType);

	const vec3 GetColour(unsigned int entity);
	void SetColour(unsigned int entity, vec3 colour, float intensity);

	const vec3 GetDirection(unsigned int entity);
	void SetDirection(unsigned int entity, vec3 direction);

	const float GetRange(unsigned int entity);
	void SetRange(unsigned int entity, float range);
	const float GetLinearAttenuation(unsigned int entity);
	const float GetQuadraticAttenuation(unsigned int entity);

	const float GetCutOff(unsigned int entity);
	void SetCutOff(unsigned int entity, float cutOff);

	const float GetOuterCutOff(unsigned int entity);
	void SetOuterCutOff(unsigned int entity, float outerCutOff);

#pragma endregion 
	/// <summary>
	/// Add a new render component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns> new render component</returns>
	RenderComponent* AddRenderComponent(unsigned int _entity);

	/// <summary>
	/// Add a provided render component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="rend"> render component to add to entity</param>
	/// <returns> render component</returns>
	RenderComponent* AddRenderComponent(unsigned int _entity, RenderComponent* rend);

	/// <summary>
	/// Adds a new physics component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns>physics component</returns>
	PhysicsComponent* AddPhysicsComponent(unsigned int _entity);

	/// <summary>
	/// Adds a new light component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <returns>light component</returns>
	LightComponent* AddLightComponent(unsigned int _entity);

	/// <summary>
	/// Adds a new light component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="light"> light component to add to entity</param>
	/// <returns>light component</returns>
	LightComponent* AddLightComponent(unsigned int _entity, LightComponent* light);
private:
	/// <summary>
	/// Starting entity count value (must be above 0)
	/// </summary>
	unsigned int entityCount = 1;

	/// <summary>
	/// Light Components in scene
	/// </summary>
	std::unordered_map<unsigned int, LightComponent*> lightComponents;

	/// <summary>
	/// Physics Components in scene
	/// </summary>
	std::unordered_map<unsigned int, PhysicsComponent*> physicsComponents;

	/// <summary>
	/// Render Components in scene
	/// </summary>
	std::unordered_map<unsigned int, RenderComponent*> renderComponents;

	/// <summary>
	/// Transform Components in scene
	/// </summary>
	std::unordered_map<unsigned int, TransformComponent*> transformComponents;

	/// <summary>
	/// Asset Factory for asset making
	/// </summary>
	AssetFactory& assFact;
};

