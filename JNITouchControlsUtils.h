

#include "TouchControlsContainer.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <jni.h>

//void setUIJNIEnv( JNIEnv* env);

void setTCJNIEnv( JavaVM* jvm);


void showCustomCommands();
void showTouchSettings();
void showEditButtons();

void toggleKeyboard();
void showKeyboard(int val);

void openConsoleBox( const char * title );
void addTextConsoleBox( const char * text );
void closeConsoleBox();
bool getConsoleBoxCanceled();

void ChangeDroidMusic(int action,int param1,int param2);

#ifdef __cplusplus
void setControlsContainer(touchcontrols::TouchControlsContainer * cc);
#endif

JNIEnv * getEnv();

#ifdef __cplusplus
}
#endif
