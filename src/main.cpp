#include <config.h>
#include "controller/app.h"


int main() {

	//TO DO, create an APP for building, and an APP for running the game
	App* app = new App();

	//App setup functions
	app->SetUpOpengl();
	app->MakeFactories();
	app->MakeSystems();

	app->Run();

	delete app;
	return 0;
}