#include "UI_Window.h"
#include "TouchControlsConfig.h"


using namespace touchcontrols;

UI_Window::UI_Window ( std::string tag, RectF pos, std::string image_filename ) : ControlSuper ( TC_TYPE_UI_WINDOW, tag, pos )
{
    image = image_filename;
    glTex = 0;

    backButton =  new Button ( "close", touchcontrols::RectF ( pos.left, pos.top, pos.left + 2,  pos.top + 2), "ui_back_arrow", UI_WINDOW_BUTTON_BACK );
    titleText = new UI_TextBox ( "text",touchcontrols::RectF ( pos.left, pos.top, pos.right,     pos.top + 2 ), "font_dual", 1, UI_TEXT_CENTRE, "Touch settings", 0.09 );

    backButton->signal_button.connect( signal );

    updateSize();
}

void UI_Window::updateSize()
{
    glRect.resize ( controlPos.right - controlPos.left, controlPos.bottom - controlPos.top );
}



bool UI_Window::processPointer ( int action, int pid, float x, float y )
{
    return backButton->processPointer( action, pid, x, y );
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
    glTex = loadTextureFromPNG ( image, x, y );

    backButton->initGL();
    titleText->initGL();
    return false;
}

void UI_Window::setScissor ( void )
{
    // Scissor window, leaving
    glScissor ( controlPos.left * GLScaleWidth, (controlPos.top) * -GLScaleHeight,
                controlPos.width() * GLScaleWidth, ( controlPos.height() - (2.f/ScaleY)) *  -GLScaleHeight );
    glEnable ( GL_SCISSOR_TEST );
}

bool UI_Window::drawGL ( bool forEditor )
{
    drawRect ( glTex, controlPos.left, controlPos.top, glRect );

    backButton->drawGL( forEditor );
    titleText->drawGL( forEditor );

    setScissor();

    return false;
}

void UI_Window::saveXML ( TiXmlDocument &doc )
{

}

void UI_Window::loadXML ( TiXmlDocument &doc )
{

}
