#############################
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := touchcontrols

LOCAL_CFLAGS := -Werror -DANDROID_NDK


ifeq ($(TOUCH_CONTROLS_NO_SDL),1)
LOCAL_CFLAGS += -DTOUCH_CONTROLS_NO_SDL
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)  $(SDL_INCLUDE_PATHS)

LOCAL_SRC_FILES:= \
 	TouchControls.cpp \
 	ControlSuper.cpp \
 	RectF.cpp \
 	Button.cpp \
 	ButtonExt.cpp \
    ButtonGrid.cpp \
    QuadSlide.cpp \
    DPadSelect.cpp \
 	OpenGLUtils.cpp \
 	GLRect.cpp \
 	TouchJoy.cpp \
 	Mouse.cpp \
 	PolarSelect.cpp \
 	MultitouchMouse.cpp \
 	WheelSelect.cpp \
 	TouchControlsContainer.cpp \
 	GLLines.cpp \
 	JNITouchControlsUtils.cpp \
 	AssetFileAccess.cpp \
 	AssetFileCache.cpp \
 	android_fopen.cpp \
 	TextDraw.cpp \
 	TapDetect.cpp \
 	UI_Controls.cpp \
 	UI_Window.cpp \
 	UI_Slider.cpp \
 	UI_TextBox.cpp \
 	UI_Button.cpp \
 	UI_Switch.cpp \
 	UI_Keyboard.cpp \
    UI_TouchDefaultSettings.cpp \
    UI_ButtonListWindow.cpp \
    UI_DropDown.cpp \
    UI_ColorPicker.cpp


 	
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog  -landroid
LOCAL_STATIC_LIBRARIES := sigc libzip libpng tinyxml 

include $(BUILD_SHARED_LIBRARY)


ifeq ($(BUILD_GLES2_TC),1)

TC_LOCAL_SRC_FILES := $(LOCAL_SRC_FILES)
#Now also build gles2 version

include $(CLEAR_VARS)

LOCAL_MODULE := touchcontrols_gles2

LOCAL_CFLAGS := -Werror -DANDROID_NDK -DUSE_GLES2

LOCAL_C_INCLUDES := $(LOCAL_PATH)  $(SDL_INCLUDE_PATHS)

LOCAL_SRC_FILES:= $(TC_LOCAL_SRC_FILES)

LOCAL_LDLIBS := -lGLESv2 -ldl -llog -landroid
LOCAL_STATIC_LIBRARIES := sigc libzip libpng tinyxml 

#include $(BUILD_SHARED_LIBRARY) //DONT BUILD

endif
