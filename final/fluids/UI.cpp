#include "Visualisation.h"
#include <GL/glui.h>
#include "UI.h"
#include "main.h"

void updateSlidingWall(int num)
{
	sim.toggleSlidingWall();
}

void UI::initialise(int mainWindow)
{
	// create window
	glui = GLUI_Master.create_glui_subwindow(mainWindow, GLUI_SUBWINDOW_RIGHT);
	glui->set_main_gfx_window(mainWindow);

	// options
	options_panel = glui->add_panel("Options");
	options_panel->set_w(200);

	glui->add_checkbox_to_panel(options_panel, "Sliding wall", &sim.slidingWall, 0, updateSlidingWall);
	glui->add_checkbox_to_panel(options_panel, "Dynamic gravity", &sim.dynamicGravity);
	glui->add_checkbox_to_panel(options_panel, "Freeze Simulation", &sim.frozen);
	// exit
	glui->add_button("Quit", 0, exit);

	// sync variables
	GLUI_Master.sync_live_all();
}
