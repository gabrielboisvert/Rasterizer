#include "../include/App.h"

constexpr auto SCREEN_WIDTH = 800;
constexpr auto SCREEN_HEIGHT = 600;

int main(int argc, char* argv[])
{
	App app(SCREEN_WIDTH, SCREEN_HEIGHT);
	app.run();
	return 0;
}