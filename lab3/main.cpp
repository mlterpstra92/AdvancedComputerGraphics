#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

struct Surfel
{
    float pos[3];
    float color[3]; // Changed from 4 to save space
    float uvec[3];
    float vvec[3];
};

int numpoints;
Surfel *pts;

void read_points(const char *fname)
{
    FILE *f = fopen(fname, "rb");
    assert(f);
    fread(&numpoints, sizeof(int), 1, f);
    printf("Reading %i points from %s ...\n", numpoints, fname);
    pts = new Surfel[numpoints];
    fread(pts, sizeof(Surfel)*numpoints, 1, f);    
    fclose(f);
}

int main(int argc, char *argv[])
{
    assert(argc>=2 && argv[1]);
    
    read_points(argv[1]);
    
    return 0;
}


