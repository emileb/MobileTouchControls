//
//  GLheader.h
//  touchcontrols
//
//  Created by Emile on 05/12/2015.
//  Copyright © 2015 Beloko games. All rights reserved.
//

#ifndef GLheader_h
#define GLheader_h


#ifdef __ANDROID__

#ifdef USE_GLES2
#include <GLES2/gl2.h>
#else
#include <GLES/gl.h>
#endif

#endif



#ifdef __IOS__

#include "OpenGLES/ES1/gl.h"

#endif

#endif /* GLheader_h */
