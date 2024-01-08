#include <config.h>
#include "controller/app.h"

#include "components/ComponentInclude.h"


int main() {

	App* app = new App();

	unsigned int cubeEntity = app->MakeEntity();
	TransformComponent transform;
	transform.position = { 3.0f, 0.0f, 0.25f };
	transform.eulers = { 0.0f, 0.0f, 0.0f };
	app->transformComponents[cubeEntity] = transform;

	PhysicsComponent physics;
	physics.velocity = { 0.0f, 0.0f, 0.0f };
	physics.eulerVelocity = { 10.0f, 10.0f, 10.0f };
	app->physicsComponents[cubeEntity] = physics;

	RenderComponent render;
	render.mesh = app->MakeCubeMesh({ 0.25f, 0.25f, 0.25f });
	render.materials[0] = app->MakeTexture("img/meHappy.png");
	render.materials[1] = app->MakeTexture("img/vignette.jpg");

	app->renderComponents[cubeEntity] = render;

	unsigned int cameraEntity = app->MakeEntity();
	transform.position = { 0.0f, 0.0f, 1.0f };
	transform.eulers = { 0.0f, 0.0f, 0.0f };
	app->transformComponents[cameraEntity] = transform;

	CameraComponent* camera = new CameraComponent();
	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->SetUpOpengl();
	app->MakeSystems();

	app->Run();

	delete app;
	return 0;
}