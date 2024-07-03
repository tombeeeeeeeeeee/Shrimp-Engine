#include <config.h>
#include "controller/app.h"

#include "Debug.h"

int main() {
	debug = new Debug(); // Debug is created first so you can log errors with application setup - Laochra

	//TO DO, create an APP for building, and an APP for running the game
	App* app = new App();

	//App setup functions
	app->SetUpOpengl();
	app->MakeFactories();
	app->MakeSystems();

	app->Run();

	delete app;
	delete debug;
	return 0;
}