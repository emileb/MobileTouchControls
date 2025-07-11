#include "UI_Window.h"
#include "TouchControlsConfig.h"


using namespace touchcontrols;

#define PADDING 0.02

UI_Window::UI_Window(std::string tag, RectF pos, std::string title, std::string image_filename) : ControlSuper(TC_TYPE_UI_WINDOW, tag, pos)
{
    image = image_filename;
    glTex = 0;

    backButton = new Button("close", touchcontrols::RectF(pos.left + 0.5, pos.top, pos.left + 2 + 0.5, pos.top + 2), "ui_close_button", UI_WINDOW_BUTTON_BACK);
    titleText = new UI_TextBox("text", touchcontrols::RectF(pos.left, pos.top, pos.right, pos.top + 2), "font_dual", 1, UI_TEXT_CENTRE, title, 0.09,
                               0x73f8ffff);

    backButton->signal_button.connect(signal);

    updateSize();
}

void UI_Window::updateSize()
{
    glRect.resize(controlPos.right - controlPos.left + PADDING, controlPos.bottom - controlPos.top + PADDING);
    backButton->updateSize();
    titleText->updateSize();
}

void UI_Window::scaleSize(float x, float y)
{
    ControlSuper::scaleSize(x, y);
    backButton->scaleSize(x, y);
    titleText->scaleSize(x, y);
}

void UI_Window::positionTranslate(float x, float y)
{
    ControlSuper::positionTranslate(x, y);
    backButton->positionTranslate(x, y);
    titleText->positionTranslate(x, y);
}

bool UI_Window::processPointer(int action, int pid, float x, float y)
{
    return backButton->processPointer(action, pid, x, y);
}

void UI_Window::resetOutput()
{

}

float UI_Window::getScrollOffsetY()
{
    return 0.1;
}

bool UI_Window::initGL()
{
    int x, y;
    glTex = loadTextureFromPNG(image, x, y);

    backButton->initGL();
    titleText->initGL();
    return false;
}

void UI_Window::setScissor(void)
{
    // Scissor window, leaving
    /*
    glScissor ( controlPos.left * GLScaleWidth,
                (controlPos.top) * -GLScaleHeight,
                controlPos.width() * GLScaleWidth,
                ( controlPos.height() - (2.f/ScaleY)) *  -GLScaleHeight );
      */
    int x, y, w, h;
    x = controlPos.left * GLScaleWidth;
    y = (1 - controlPos.bottom) * -GLScaleHeight;
    w = controlPos.width() * GLScaleWidth;
    h = (controlPos.height() - (2.f / ScaleY)) * -GLScaleHeight;
    //LOGTOUCH("%d   %d   %d   %d",x,y,w,h);

    gl_scissor(x, y, w, h);

    gl_enable(GL_SCISSOR_TEST);
}

bool UI_Window::drawGL(bool forEditor)
{
    gl_drawRect(glTex, controlPos.left - (PADDING / 2), controlPos.top, glRect);

    backButton->drawGL(forEditor);
    titleText->drawGL(forEditor);

    setScissor();

    return false;
}

void UI_Window::saveXML(TiXmlDocument &doc)
{

}

void UI_Window::loadXML(TiXmlDocument &doc)
{

}
