/* Some convenience code for using GLSL shaders in your OpenGL program.
*/

#include "Shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>

/* Choose profiles, set optimal options */
void Shader::chooseCgProfiles()
{
    vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
    cgGLSetOptimalOptions(vertexProfile);
    fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentProfile);
    printf("vertex profile:   %s\n", cgGetProfileString(vertexProfile));
    printf("fragment profile: %s\n", cgGetProfileString(fragmentProfile));

}

/* Load Cg program from disk */
CGprogram Shader::loadCgProgram(CGprofile profile, const char *filename)
{
    CGprogram program;
    assert(cgIsContext(context));

    fprintf(stderr, "Cg program %s creating.\n", filename);
    program = cgCreateProgramFromFile(context, CG_SOURCE, filename, profile, NULL, NULL);
    
    if(!cgIsProgramCompiled(program)) 
    {
        printf("%s\n",cgGetLastListing(context));
        exit(1);
    }

    fprintf(stderr, "Cg program %s loading.\n", filename);
    cgGLLoadProgram(program);
    
    return program;
}

void Shader::loadCgPrograms()
{
    /* Load all Cg programs that are used with loadCgProgram */
    depthVertexProgram = loadCgProgram(vertexProfile, "depthVertexProgram.cg");
    depthFragmentProgram = loadCgProgram(fragmentProfile, "depthFragmentProgram.cg");
    normalFragmentProgram = loadCgProgram(fragmentProfile, "normalFragmentProgram.cg");
    smoothFragmentProgram = loadCgProgram(fragmentProfile, "smoothFragmentProgram.cg");
    textureProgram = loadCgProgram(fragmentProfile, "textureProgram.cg");
}

CGparameter Shader::getNamedParameter( CGprogram program, const char * name )
{
    CGparameter result = cgGetNamedParameter(program, name);
    if (!result)
    {
      printf("Parameter %s was not defined in the shader\n", name);
    }
    
    return result;
}

void Shader::init()
{
    cgSetErrorCallback(handleCgError);
    context = cgCreateContext();
    chooseCgProfiles();
    loadCgPrograms();
}