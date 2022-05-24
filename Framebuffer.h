
#ifndef _Framebuffer_H_
#define _Framebuffer_H_


namespace touchcontrols
{

typedef struct
{
	int vidWidth;
	int vidHeight;
	int vidWidthReal;
	int vidHeightReal;

	bool isGles2;
	bool npotAvailable;
	bool filter;

	bool depthStencilAvailable;
} fbConfig;



void R_FrameBufferConfig(fbConfig config);
void R_FrameBufferInit(bool useGL4ES, bool isGles2);
void R_FrameBufferStart();
void R_FrameBufferEnd();


}

#endif