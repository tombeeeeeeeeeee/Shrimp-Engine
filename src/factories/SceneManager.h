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
	std::unordered_map<unsigned int, PhysicsComponent>* GetPhysics();

	/// <summary>
	/// Makes entity and adds to the entity count.
	/// </summary>
	/// <returns> unsigned int of entity</returns>
	unsigned int MakeEntity() { return entityCount++; }

	/// <summary>
	/// Creates an entity with a transform and camera component 
	/// </summary>
	/// <param name="position"> transform position </param>
	/// <returns> unsigned int of entity</returns>
	unsigned int MakeCamera(glm::vec3 position);

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
	unsigned int MakeEmptyTransform(glm::vec3 position, glm::quat rotation = glm::identity<glm::quat>());

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

#pragma region Physics Components


	unsigned int MakePhysicsEntity();

	/// <summary>
	/// Adds a new physics component to the provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="invMass"> 1 over the Mass of the body</param>
	/// <returns>physics component</returns>
	PhysicsComponent AddPhysicsComponent(unsigned int _entity, float invMass = 0);

	/// <summary>
	/// Adds a new physics component to a provided entity id
	/// </summary>
	/// <param name="_entity"> id to add component to</param>
	/// <param name="momentOfInertia"> Scale components for the inverse moment of inertia</param>
	/// <param name="force"> current net force acting on the body</param>
	/// <param name="velocity"> current velocity of the body</param>
	/// <param name="netDepen"> current depen on the body</param>
	/// <param name="invBodyIT"> inverse ineretia tensor in local space</param>
	/// <param name="invWorldIT"> inverse world inertia tensor in world space</param>
	/// <param name="torque"> current torque acting on the body</param>
	/// <param name="angularVelocity"> current angular velocity acting on the body</param>
	/// <param name="angularMomentum"> current angular momentum acting on the body</param>
	/// <param name="dX"> maximum delta along the x axis from transform to vertex</param>
	/// <param name="dY"> maximum delta along the y axis from transform to vertex</param>
	/// <param name="dZ"> maximum delta along the z axis from transform to vertex</param>
	/// <param name="shapes"> shapes comprising the body</param>
	/// <param name="centreOfMass"> displacement from transform for the centre of mass</param>
	/// <param name="isGravitated"> whether the shape is impacted by gravity</param>
	/// <param name="elasiticCoef"> elasticity of the object (between 0 and 1 for realistic collisions)</param>
	/// <param name="drag"> linear drag impacting the body</param>
	/// <param name="angularDrag"> angular drag impacting the body</param>
	/// <returns> new physics component</returns>
	PhysicsComponent AddPhysicsComponent(unsigned int _entity,
		glm::vec3 momentOfInertia, glm::vec3 force,
		glm::vec3 velocity, glm::vec3 netDepen,
		glm::mat3 invBodyIT, glm::mat3 invWorldIT,
		glm::vec3 torque, glm::vec3 angularVelocity,
		glm::vec3 angularMomentum, float dX, float dY, float dZ,
		std::vector<Shape> shapes, float invMass, glm::vec3 centreOfMass, bool isGravitated,
		float elasiticCoef, float drag, float angularDrag);


	/// <summary>
	/// Gets the scale components of the moment of inertia of a body
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <returns>returns scale components of the moment of inertia or NAN if entity has no physics component</returns>
	const glm::vec3 GetMomentOfInertiaScale(unsigned int _enity);
	/// <summary>
	/// Sets the moment of inertia scale for a provided entity
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <param name="scale"> scale as a vector 3</param>
	void SetMomentOfInertiaScale(unsigned int _entity, glm::vec3 scale);
	/// <summary>
	/// Sets the moment of inertia scale for a provided entity
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <param name="x"> x component of scale vector</param>
	/// <param name="y"> y component of scale vector</param>
	/// <param name="z"> z component of scale vector</param>
	void SetMomentOfInertiaScale(unsigned int _entity, float x, float y, float z);

	/// <summary>
	/// Gets the current net force acting on a body
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <returns>returns current force acting or NAN if entity has no physics component</returns>
	const glm::vec3 GetForce(unsigned int _entity);
	/// <summary>
	/// Sets the force for a provided entity
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <param name="force"> force as a vector 3</param>
	void SetForce(unsigned int _entity, glm::vec3 force);
	/// <summary>
	/// Sets the force for a provided entity
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <param name="x"> x component of force vector</param>
	/// <param name="y"> y component of force vector</param>
	/// <param name="z"> z component of force vector</param>
	void SetForce(unsigned int _entity, float x, float y, float z);
	/// <summary>
	/// Adds force to the current force a body is experiencing
	/// </summary>
	/// <param name="_entity"> entity id</param>
	/// <param name="force"> force as a vector 3</param>
	void AddForce(unsigned int _entity, glm::vec3 force);

	/// <summary>
	/// Gets the current velocity of a body
	/// </summary>
	/// <param name="_entity">entity id of body</param>
	/// <returns>current veloctiy of body or NAN if entity does not have a physics component</returns>
	const glm::vec3 GetVelocity(unsigned int _entity);
	/// <summary>
	/// Sets the current velocity of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="velocity"> velocity as a vector3</param>
	void SetVelocity(unsigned int _entity, glm::vec3 velocity);
	/// <summary>
	/// Sets the current current velocity of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="x"> x component of veloctiy vector</param>
	/// <param name="y"> y component of veloctiy vector</param>
	/// <param name="z"> z component of veloctiy vector</param>
	void SetVelocity(unsigned int _entity, float x, float y, float z);
	/// <summary>
	/// Adds velocity to a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="velocity"> additional velocity</param>
	void AddVelocity(unsigned int _entity, glm::vec3 velocity);

	/// <summary>
	/// Get current momentum of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> returns current momentum of a body or NAN if the entity is static or doesnt have a physics component</returns>
	const glm::vec3 GetMomentum(unsigned int _entity);
	/// <summary>
	/// Sets current momentum of a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <param name="momentum"> momentum as a vector3</param>
	void SetMomentum(unsigned int _entity, glm::vec3 momentum);
	/// <summary>
	/// Sets current momentum of a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <param name="x"> x component of momentum vector</param>
	/// <param name="y"> y component of momentum vector</param>
	/// <param name="z"> z component of momentum vector</param>
	void SetMomentum(unsigned int _entity, float x, float y, float z);
	/// <summary>
	/// Adds momentum to a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <param name="momentum"> additional momentum</param>
	void AddMomentum(unsigned int _entity, glm::vec3 momentum);

	/// <summary>
	/// Gets the current net depenertation
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns>current net depenetration or NAN if the entity doesnt have a physics component</returns>
	const glm::vec3 GetNetDepenetration(unsigned int _entity);
	/// <summary>
	/// Set the net depenetration of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="depenetration"> depenetration of body</param>
	void SetNetDepenetration(unsigned int _entity, glm::vec3 depenetration);

	/// <summary>
	/// Get inverse inertia tensor in local space
	/// </summary>
	/// <param name="_entity"> entity id of a body </param>
	/// <returns> inverse local inertia tensor </returns>
	const glm::mat3 GetInverseBodyInertiaTensor(unsigned int _entity);
	/// <summary>
	/// Set local inverse inertia tensor in local space
	/// </summary>
	/// <param name="_entity">entity id of a body</param>
	/// <param name="invBodyTensor"> inverse local inertia tensor or NAN if entity doesnt have a physics component</param>
	void SetInverseBodyInertiaTensor(unsigned int _entity, glm::mat3 invBodyTensor);
	/// <summary>
	/// Updates the local inertia tensor.
	/// Called when a new shape is added to the body.
	/// </summary>
	/// <param name="_entity">entity id of body</param>
	void UpdateBodyInertiaTensor(unsigned int _entity);

	/// <summary>
	/// Gets inverse inertia tensor in world space.
	/// </summary>
	/// <param name="_entity"></param>
	/// <returns> inertia tensor in world space or NAN if entity doesnt have a physics component</returns>
	const glm::mat3 GetInverseWorldInertiaTensor(unsigned int _entity);
	/// <summary>
	/// Set inverse inertia tensor in world space.
	/// the invWorldIT is set every integration step of the Physics system and will rend this function useless if it is being called.
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="invWorldTensor"> inverse world inertia tensor</param>
	void SetInverseWorldInertiaTensor(unsigned int _entity, glm::mat3 invWorldTensor);

	/// <summary>
	/// Get the current torque acting on a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <returns> current torque acting on a body or NAN if the entity doesnt have a physics component</returns>
	const glm::vec3 GetTorque(unsigned int _entity);
	/// <summary>
	/// Sets the current torque acting on a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <param name="torque"> torque</param>
	void SetTorque(unsigned int _entity, glm::vec3 torque);
	/// <summary>
	/// Adds to the current torque acting on a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <param name="torque"> additional torque</param>
	void AddTorque(unsigned int _entity, glm::vec3 torque);

	/// <summary>
	/// Gets the angular velocity acting on a body
	/// </summary>
	/// <param name="_entity"> entity id of the body</param>
	/// <returns> angular veloctiy in revolutions per second or NAN if the entity doesnt have a physics component</returns>
	const glm::vec3 GetAngularVelocity(unsigned int _entity);
	/// <summary>
	/// Sets the angular velocity acting on a body
	/// </summary>
	/// <param name="_entity"> entity id of the body</param>
	/// <param name="angularVel"> angular veloctiy in revolutions per second</param>
	void SetAngularVelocity(unsigned int _entity, glm::vec3 angularVel);
	/// <summary>
	/// Adds to the currenbt angular velocity acting on a body
	/// </summary>
	/// <param name="_entity"> entity id of a body</param>
	/// <param name="angularVel"> additional angular velocity in revolutions per second</param>
	void AddAngularVelocity(unsigned int _entity, glm::vec3 angularVel);

	/// <summary>
	/// Get the angular momentum of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> current angular momentum or NAN if the entity doesnt have a physics component</returns>
	const glm::vec3 GetAngularMomentum(unsigned int _entity);
	/// <summary>
	/// Sets the current angular momentum of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="angularMom"> angular momentum in world space</param>
	void SetAngularMomentum(unsigned int _entity, glm::vec3 angularMom);
	/// <summary>
	/// Adds to the current angular momentum of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="angularMom"> additional angular momentum in world space</param>
	void AddAngularMomentum(unsigned int _entity, glm::vec3 angularMom);

	/// <summary>
	/// Gets the shapes that make up a physics body
	/// </summary>
	/// <param name="_entity"> entity id of the body</param>
	/// <returns> pointer to the vectro of shapes or nullptr if the entity doesnt have a physics component</returns>
	const std::vector<Shape>* GetPhysicsComponentShapes(unsigned int _entity);

	/// <summary>
	/// Sets the shapes that make up a physics body
	/// </summary>
	/// <param name="_entity"> entity id of the body</param>
	/// <param name="shapes"> pointer to shapes vector</param>
	void SetPhysicsComponentShapes(unsigned int _entity, std::vector<Shape>* shapes);

	/// <summary>
	/// Add shape to current physics body shape
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="shape"> shape</param>
	/// <param name="offset"> offset from transform position to shape</param>
	void AddPhysicsShape(unsigned int _entity, Shape shape, glm::vec3 offset = glm::zero<glm::vec3>());
	/// <summary>
	/// Add Sphere to net shape of body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="radius"> radius of sphere</param>
	/// <param name="offset"> offset from transform position to sphere</param>
	void AddPhysicsSphere(unsigned int _entity, float radius = 0.5f, glm::vec3 offset = glm::zero<glm::vec3>());
	//TODO
	void AddPhysicsShapeBox(unsigned int _entity, float x = 0.5f, float y = 0.5f, float z = 0.5f, glm::vec3 offset = glm::zero<glm::vec3>());
	//TODO
	void AddPhysicsShapeBox(unsigned int _entity, glm::vec3 bottomLeft, glm::vec3 topRight);

	/// <summary>
	/// Adds upright pill shape to net shape of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="radius"> radius and space half space between tips of pill</param>
	/// <param name="offset"> offset from transform position to pill</param>
	void AddPhysicsShapePill(unsigned int _entity, float radius = 0.5f, glm::vec3 offset = glm::zero<glm::vec3>());
	/// <summary>
	/// Add pill shape to net shape of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="bottom"> position of bottom of the pill</param>
	/// <param name="top"> position of top of pill</param>
	/// <param name="radius"> radius around these points</param>
	void AddPhysicsShapePill(unsigned int _entity, glm::vec3 bottom, glm::vec3 top, float radius);

	/// <summary>
	/// Get the inverse mass of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> inverse mass of a body or NAN if the entity has no physics component</returns>
	const float GetInverseMass(unsigned int _entity);
	/// <summary>
	/// Get the mass of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> mass of object or NAN if its inverse mass is 0 or if the entity has no physics component</returns>
	const float GetMass(unsigned int _entity);

	/// <summary>
	/// Sets the inverse mass of a body, set invMass to 0 if the body should remain static
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="invMass"> inverse mass, will be set to absolute value</param>
	void SetInverseMass(unsigned int _entity, float invMass);
	/// <summary>
	/// Sets the mass of a body, cannot be used to make an body static
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="mass"> mass will be set to absolute value</param>
	void SetMass(unsigned int _entity, float mass);

	/// <summary>
	/// Get Centre of Mass of an body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> centre of mass or NAN if the entity does not have a physics component</returns>
	const glm::vec3 GetCentreOfMass(unsigned int _entity);
	/// <summary>
	/// Set the centre of mass of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="CoM"> centre of mass in local space</param>
	void SetCentreOfMass(unsigned int _entity, glm::vec3 CoM);

	/// <summary>
	/// Get whether a body is affected by gravity
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> whether the body is affected by gravity or false if the entity does not have a physics component </returns>
	const bool GetIsGravitated(unsigned int _entity);
	/// <summary>
	/// Set whether a body is affected by gravity
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	void SetIsGravitated(unsigned int _entity, bool gravitated);

	/// <summary>
	/// Get elastic coefficent of body
	/// collisions use average elasticity of bodies in collision.
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> elasitic coef or NAN if the entity doesnt have a physics component</returns>
	const float GetElasticCoef(unsigned int _entity);
	/// <summary>
	/// Sets the elastic coefficient of a body
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <param name="elasticCoef"> elastic coef (between 0 and 1 for "realistic" collisions)</param>
	void SetElasticCoef(unsigned int _entity, float elasticCoef);

	/// <summary>
	/// Gets linear drag of body.
	/// During the integration step, the current velocity is multipiled by 1 - (0.0001f * drag)
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	/// <returns> drag or NAN if the entity dopesnt have a physics component</returns>
	const float GetPhysicsDrag(unsigned int _entity);
	/// <summary>
	/// Sets linear drag of body.
	/// During the integration step, the current velocity is multipiled by 1 - (0.0001f * drag)
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	void SetPhysicsDrag(unsigned int _entity, float drag);

	/// <summary>
	/// Get angular drag of body
	/// During the integration step, the current angularVelocity is multipiled by 1 - (0.01f * angularDrag)
	/// </summary>
	/// <param name="_entity">entity of body</param>
	/// <returns>angular drag or NAN if entity doesnt have a physics component</returns>
	const float GetPhysicsAngularDrag(unsigned int _entity);
	/// <summary>
	/// Set angular drag of body
	/// During the integration step, the current angularVelocity is multipiled by 1 - (0.01f * angularDrag)
	/// </summary>
	/// <param name="_entity"> entity id of body</param>
	void SetPhysicsAngularDrag(unsigned int _entity, float angularDrag);
#pragma endregion

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

