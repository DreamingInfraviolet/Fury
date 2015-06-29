#include "renderwindow.h"
#include "scene.h"

void RenderWindow::draw(Scene* scene)
{
	scene->draw();
}