#include "UI_TouchDefaultSettings.h"

namespace touchcontrols
{

#define BUTTON_CLOSE 1
#define BUTTON_RESET 2
#define BUTTON_2 3

#define SLIDER_ALPHA 10
#define SLIDER_LOOK  11
#define SLIDER_MOVE  12


#define SWITCH_INVERT_LOOK 20
#define SWITCH_SHOWSAVE     21
#define SWITCH_JOYSTICKS    22

static TouchControlsContainer *container = NULL;
static TouchControls *controls = NULL;
static std::string settingsFilename;
static tTouchSettings settings;

static  sigc::signal<void, tTouchSettings> signal_settingChange;

static void saveSettings ( std::string filename )
{
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration ( "1.0", "", "" );
    doc.LinkEndChild ( decl );

    TiXmlElement * root = new TiXmlElement ( "settings" );
    doc.LinkEndChild ( root );

    root->SetAttribute ( "inverLook", settings.invertLook );
    root->SetDoubleAttribute ( "alpha", settings.alpha );

    LOGTOUCH("Saving settings to %s\n", filename.c_str() );
    doc.SaveFile ( filename );
}

static void loadSettings ( std::string filename )
{
    LOGTOUCH("Loading settings from %s\n", filename.c_str() );

    TiXmlDocument *doc = new TiXmlDocument ( filename.c_str() );

    if ( !doc->LoadFile() )
    {
        LOGTOUCH("Failed to open settings file\n");
        return;
    }

    TiXmlHandle hDoc ( doc );
    TiXmlElement* root = hDoc.FirstChild ( "settings" ).Element();

    root->QueryBoolAttribute ( "inverLook", &settings.invertLook );
    root->QueryFloatAttribute ( "alpha",  &settings.alpha );

    delete doc;
}

static void resetDefaults()
{
    settings.alpha = 0.5;
    settings.invertLook = false;
    settings.lookSensitivity = 1;
    settings.moveSensitivity = 1;
    settings.showLoadSave = false;
    settings.showJoysticks = true;
}

static void buttonPress ( int state, int code )
{
    if ( state == 0 && code == BUTTON_CLOSE )
    {
        saveSettings( settingsFilename);
        signal_settingChange.emit( settings );
        controls->setEnabled ( false );
    }

    if ( code == BUTTON_RESET )
    {
        container->resetDefaults();
        resetDefaults();
    }
}

static void sliderChange ( uint32_t uid, float value )
{
    if( uid == SLIDER_ALPHA )
    {
        settings.alpha = value;
    }
    else if( uid == SLIDER_LOOK )
    {
        settings.lookSensitivity = value * 2;
    }
    else if( uid == SLIDER_MOVE )
    {
        settings.moveSensitivity = value * 2;
    }
}

static void switchChange ( uint32_t uid, bool value )
{
    if( uid == SWITCH_INVERT_LOOK )
    {
        settings.invertLook = value;
    }
    else if( uid == SWITCH_SHOWSAVE )
    {
        settings.showLoadSave = value;
        signal_settingChange.emit( settings );
    }
    else if( uid == SWITCH_JOYSTICKS )
    {
        settings.showJoysticks = value;
        signal_settingChange.emit( settings );
    }
}



sigc::signal<void, tTouchSettings> *getSettingsSignal()
{
    return &signal_settingChange;
}

TouchControls *createDefaultSettingsUI ( TouchControlsContainer *con, std::string settingsFile )
{
    settingsFilename = settingsFile;
    container = con;

    if ( controls == NULL )
    {
        //Defaults
        resetDefaults();

        // Then try and laod file
        loadSettings( settingsFilename);

        signal_settingChange.emit( settings );

        controls = new TouchControls ( "ui_settings", false, false );

        float textSize = 0.07f;
        uint32_t windownLeft = 3;
        uint32_t windowRight = 24;

        controls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 2, 22, 4 ), "font_dual", 1, UI_TEXT_CENTRE, "Touch settings", 0.09 ) );
        controls->addControl ( new Button ( "close", touchcontrols::RectF ( windownLeft, 2, windownLeft + 2, 4 ), "ui_back_arrow", BUTTON_CLOSE ) );

        controls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 4, 12, 6 ), "font_dual", 0, UI_TEXT_RIGHT, "Transparency:", textSize ) );
        UI_Slider *slider =   new UI_Slider ( "slider_alpha",  touchcontrols::RectF ( 13, 4, windowRight - 1, 6 ), SLIDER_ALPHA, "ui_slider_bg1", "ui_slider_handle" );
        slider->setValue(  settings.alpha );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        controls->addControl ( slider );

        controls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 6, 12, 8 ), "font_dual", 0, UI_TEXT_RIGHT, "Look sensitivity:", textSize ) );
        slider =              new UI_Slider ( "slider_look",  touchcontrols::RectF ( 13, 6, windowRight - 1, 8 ), SLIDER_LOOK , "ui_slider_bg1", "ui_slider_handle");
        slider->setValue(  settings.lookSensitivity / 2 );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        controls->addControl ( slider );

        controls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 8, 12, 10 ), "font_dual", 0, UI_TEXT_RIGHT, "Move sensitivity:", textSize ) );
        slider =              new UI_Slider ( "slider_move",    touchcontrols::RectF ( 13, 8, windowRight - 1, 10 ), SLIDER_MOVE, "ui_slider_bg1", "ui_slider_handle" );
        slider->setValue(  settings.moveSensitivity / 2 );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        controls->addControl ( slider );

        controls->addControl ( new UI_TextBox ( "text",          touchcontrols::RectF ( windownLeft, 10, 9.5, 12 ), "font_dual", 0, UI_TEXT_RIGHT, "Invert look:", textSize ) );
        UI_Switch *swtch =      new UI_Switch ( "invert_switch", touchcontrols::RectF ( 10, 10.3, 13, 11.7 ), SWITCH_INVERT_LOOK, "ui_switch_on", "ui_switch_off" );
        swtch->setValue( settings.invertLook );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        controls->addControl ( swtch );

        controls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( 13, 10, windowRight - 3.5, 12 ), "font_dual", 0, UI_TEXT_RIGHT, "Save/Load btns:", textSize ) );
        swtch = new UI_Switch ( "showsave_switch",              touchcontrols::RectF ( windowRight - 3, 10.3, windowRight, 11.7 ),SWITCH_SHOWSAVE, "ui_switch_on", "ui_switch_off" ) ;
        swtch->setValue( settings.showLoadSave );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        controls->addControl ( swtch);

        controls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 12, 9.5, 14 ), "font_dual", 0, UI_TEXT_RIGHT, "Show joysticks:", textSize ) );
        swtch =      new UI_Switch ( "show_joy_switch",         touchcontrols::RectF ( 10, 12.3, 13, 13.7 ), SWITCH_JOYSTICKS, "ui_switch_on", "ui_switch_off" );
        swtch->setValue( settings.showJoysticks );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        controls->addControl ( swtch );

        UI_Button *button =   new UI_Button ( "reset_button",  touchcontrols::RectF ( 13, 12, windowRight, 14 ), BUTTON_RESET, "font_dual", 0, UI_TEXT_CENTRE, "Reset", textSize, "ui_button_bg" );
        button->signal.connect ( sigc::ptr_fun ( &buttonPress ) );
        controls->addControl ( button );




        // Draws backwards so need background last
        controls->addControl ( new UI_Window ( "bg_window", touchcontrols::RectF ( windownLeft, 2, windowRight, 14.2 ), "ui_background" ) );

        controls->setPassThroughTouch ( false );

        controls->signal_button.connect ( sigc::ptr_fun ( &buttonPress ) );
    }

    return controls;
}


}