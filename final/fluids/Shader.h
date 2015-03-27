#ifndef _H_GLSL_SHADERS_H_
#define _H_GLSL_SHADERS_H_

#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include "OpenGL.h"
#include <Cg/cg.h>
#include <Cg/cgGL.h>
#include <string>

class Shader
{
public:
	CGcontext	context;
	CGprofile	vertexProfile, fragmentProfile;

	CGprogram depthVertexProgram = NULL;
	CGprogram depthFragmentProgram = NULL;
	CGprogram smoothFragmentProgram = NULL;
	CGprogram normalFragmentProgram = NULL;
	CGprogram textureProgram = NULL;

	CGparameter getNamedParameter( CGprogram program, const char * name );
	void chooseCgProfiles();
	CGprogram loadCgProgram(CGprofile profile, const char *filename);
	void loadCgPrograms();
	static void handleCgError() 
	{
	    fprintf(stderr, "Cg error: %s\n", cgGetErrorString(cgGetError()));
	}
	void init();
};

#endif