#ifndef _MAIN_H_
#define _MAIN_H_
class Visualisation;
class Simulation;
class Shader;

typedef unsigned int GLuint;

extern Visualisation vis;
extern Simulation sim;
extern Shader shader;

extern float view_left;
extern float view_right;
extern float view_bottom;
extern float view_top;

extern unsigned int w_width;
extern unsigned int w_height;

extern float view_near;
extern float view_far;

extern const float eye[];
extern float lightp[3];
#endif
