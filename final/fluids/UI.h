#ifndef _H_UI_H_
#define _H_UI_H_

#include <GL/glui.h>

class UI
{
public:
	void initialise(int mainWindow);

private:
	GLUI *glui;
	GLUI_Panel *options_panel;
};

#endif
