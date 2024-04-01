#include <config.h>
#include "controller/app.h"


int main() {

	App* app = new App();

	app->SetUpOpengl();
	app->MakeSystems();
	app->MakeFactories();

	app->Run();

	delete app;
	return 0;
}