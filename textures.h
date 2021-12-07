
#ifndef GRAFIKALAB6_TEKSTURA_TEXTURES_H
#define GRAFIKALAB6_TEKSTURA_TEXTURES_H

#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cstdio>

class textures {
public:
    static GLbyte *
    LoadTGAImage(const char *FileName, GLint *ImWidth, GLint *ImHeight, GLint *ImComponents, GLenum *ImFormat);
};


#endif //GRAFIKALAB6_TEKSTURA_TEXTURES_H
