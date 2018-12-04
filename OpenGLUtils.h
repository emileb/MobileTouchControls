#include "GLRect.h"
#include "GLLines.h"
#include <string>
#include <vector>
#include "TouchControlsConfig.h"
extern "C" {
#include <libpng/png.h>
}


#ifndef _OpenGLUtils_H_
#define _OpenGLUtils_H_

#define TEXTURE_LOAD_ERROR -1

namespace touchcontrols
{

struct FontInfo
{
    float leftGap;
    float rightGap;
};

extern float GLScaleWidth ;
extern float GLScaleHeight ;

void gl_setGLESVersion( int v );
int  gl_getGLESVersion();

void gl_setGraphicsBasePath(std::string path);

float GLESscaleX(float X);
float GLESscaleY(float Y);

void gl_drawRect(GLuint texture, float x, float y, GLRect &rect);
void gl_drawRect(GLfloat r, GLfloat g, GLfloat b, GLfloat a, float x, float y, GLRect &rect);
void gl_drawLines(GLfloat x, GLfloat y, GLLines &lines);


GLuint loadTextureFromPNG(std::string filename, int &width, int &height, std::vector< FontInfo >* = NULL );

void png_zip_read(png_structp png_ptr, png_bytep data, png_size_t length);

void gl_setFixAspect( bool v );

bool gl_getFixAspect();

void clearGlTexCache();

void setTextureNumberStart( int start );

#ifdef USE_GLES2
extern "C"
{
    void glColor4f( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
    void glLoadIdentity();
    void glScalef( GLfloat x, GLfloat y, GLfloat z );
    void glTranslatef( GLfloat x, GLfloat y, GLfloat z );
}
#endif


}


#endif
