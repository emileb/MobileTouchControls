#include "UI_TouchDefaultSettings.h"
#include "UI_ButtonListWindow.h"

namespace touchcontrols
{

#define BUTTON_CLOSE 1
#define BUTTON_RESET 2
#define BUTTON_EDIT_BUTTONS 3

#define SLIDER_ALPHA 10
#define SLIDER_LOOK  11
#define SLIDER_MOVE  12
#define SLIDER_TURN  13

#define SWITCH_INVERT_LOOK   20
#define SWITCH_JOYSTICKS     22
#define SWITCH_JOYSTICK_MODE 23
#define SWITCH_HIDE_INV      24
#define SWITCH_HIDE_NBRS     25
#define SWITCH_WEAP_WHEEL    26
#define SWITCH_FIXED_MOVE    27

#define DROPDOWN_DBL_TAP_LEFT 30
#define DROPDOWN_DBL_TAP_RIGHT 31

static TouchControlsContainer *container = NULL;
static UI_Controls *rootControls = NULL;
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

    root->SetAttribute( "invert_look", settings.invertLook );
    root->SetAttribute( "show_sticks", settings.showJoysticks );
    root->SetAttribute( "joystick_mode", settings.joystickLookMode );
    root->SetAttribute( "auto_hide_inventory", settings.autoHideInventory );
   	root->SetAttribute( "auto_hide_numbers", settings.autoHideNumbers );
    root->SetAttribute( "weapon_wheel_enabled", settings.weaponWheelEnabled );
    root->SetAttribute( "fixed_move_stick", settings.fixedMoveStick );

    root->SetDoubleAttribute ( "alpha", settings.alpha );
    root->SetDoubleAttribute ( "look_sens", settings.lookSensitivity );
    root->SetDoubleAttribute ( "turn_sens", settings.turnSensitivity );
    root->SetDoubleAttribute ( "move_sens", settings.moveSensitivity );

    root->SetAttribute( "dbl_tap_left", settings.dblTapLeft );
    root->SetAttribute( "dbl_tap_right", settings.dblTapRight );

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

    root->QueryBoolAttribute ( "invert_look", &settings.invertLook );
    root->QueryBoolAttribute ( "show_sticks", &settings.showJoysticks );
    root->QueryBoolAttribute ( "joystick_mode", &settings.joystickLookMode );
    root->QueryBoolAttribute ( "auto_hide_inventory", &settings.autoHideInventory );
  	root->QueryBoolAttribute ( "auto_hide_numbers", &settings.autoHideNumbers );
  	root->QueryBoolAttribute ( "weapon_wheel_enabled", &settings.weaponWheelEnabled );
	root->QueryBoolAttribute ( "fixed_move_stick", &settings.fixedMoveStick );

    root->QueryFloatAttribute ( "alpha",  &settings.alpha );
    root->QueryFloatAttribute ( "look_sens",  &settings.lookSensitivity );
    root->QueryFloatAttribute ( "turn_sens",  &settings.turnSensitivity );
    root->QueryFloatAttribute ( "move_sens",  &settings.moveSensitivity );

    root->QueryUnsignedAttribute( "dbl_tap_left",  &settings.dblTapLeft );
    root->QueryUnsignedAttribute( "dbl_tap_right",  &settings.dblTapRight );

    delete doc;
}

static void resetDefaults()
{
    settings.alpha = 0.5;
    settings.invertLook = false;
    settings.lookSensitivity = 1;
    settings.turnSensitivity = 1;
    settings.moveSensitivity = 1;
    settings.showJoysticks = true;
    settings.joystickLookMode = false;
    settings.autoHideInventory = true;
    settings.autoHideNumbers = true;
    settings.weaponWheelEnabled = true;
    settings.fixedMoveStick = false;

    settings.dblTapLeft = 0;
    settings.dblTapRight = 0;
}

static void buttonPress ( int state, int code )
{
    if ( state == 0 && code == UI_WINDOW_BUTTON_BACK )
    {
        rootControls->setEnabled ( false ); // controlsEnabled will get called automatically
        LOGTOUCH("Window close");
    }

    if ( code == BUTTON_RESET )
    {
        container->resetDefaults();
        resetDefaults();
    }
    else if (code == BUTTON_EDIT_BUTTONS )
    {
        showButtonListWindow( container );
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
    else if( uid == SLIDER_TURN )
    {
        settings.turnSensitivity = value * 2;
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
    else if( uid == SWITCH_JOYSTICKS )
    {
        settings.showJoysticks = value;
        signal_settingChange.emit( settings );
    }
    else if( uid == SWITCH_JOYSTICK_MODE )
    {
        settings.joystickLookMode = value;
    }
    else if( uid == SWITCH_HIDE_INV )
    {
        settings.autoHideInventory = value;
    }
	else if( uid == SWITCH_HIDE_NBRS )
	{
		settings.autoHideNumbers = value;
	}
	else if( uid == SWITCH_WEAP_WHEEL )
	{
		settings.weaponWheelEnabled = value;
	}
	else if( uid == SWITCH_FIXED_MOVE )
	{
		settings.fixedMoveStick = value;
	}
}

static void dropDownChange ( uint32_t uid, uint32_t value )
{
    if( uid == DROPDOWN_DBL_TAP_LEFT )
    {
        settings.dblTapLeft = value;
    }
    else if( uid == DROPDOWN_DBL_TAP_RIGHT )
    {
        settings.dblTapRight = value;
    }
}

// Gets called by a signal when the controls are enabled or disabled
static void controlsEnabled( bool enabled )
{
    if( !enabled )
    {
        saveSettings( settingsFilename );
        signal_settingChange.emit( settings );
    }
}


sigc::signal<void, tTouchSettings> *getSettingsSignal()
{
    return &signal_settingChange;
}

UI_Controls *createDefaultSettingsUI ( TouchControlsContainer *con, std::string settingsFile )
{
    settingsFilename = settingsFile;
    container = con;

    if ( rootControls == NULL )
    {
        //Defaults
        resetDefaults();

        // Then try and load file
        loadSettings( settingsFilename);

        signal_settingChange.emit( settings );

        rootControls = new UI_Controls( "ui_settings" );

        float textSize = 0.07f;
        uint32_t windownLeft = 3;
        uint32_t windowRight = 24;

        //rootControls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 2, 22, 4 ), "font_dual", 1, UI_TEXT_CENTRE, "Touch settings", 0.09 ) );
       // rootControls->addControl ( new Button ( "close", touchcontrols::RectF ( windownLeft, 2, windownLeft + 2, 4 ), "ui_back_arrow", BUTTON_CLOSE ) );
        float y = 1;;

        // Draws backwards so need background last
        UI_Window *window =  new UI_Window ( "bg_window", touchcontrols::RectF ( windownLeft, y, windowRight, 14 ),"Touch settings", "ui_background" );
        rootControls->addControl( window );
        window->signal.connect ( sigc::ptr_fun ( &buttonPress ) );

        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, y, 12, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Transparency:", textSize ) );
        UI_Slider *slider =   new UI_Slider ( "slider_alpha",  touchcontrols::RectF ( 13, y, windowRight - 1, y+2 ), SLIDER_ALPHA, "ui_slider_bg1", "ui_slider_handle" );
        slider->setValue(  settings.alpha );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        rootControls->addControl ( slider );

        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, y, 12, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Up/Down sensitivity:", textSize ) );
        slider =              new UI_Slider ( "slider_look",  touchcontrols::RectF ( 13, y, windowRight - 1, y+2 ), SLIDER_LOOK , "ui_slider_bg1", "ui_slider_handle");
        slider->setValue(  settings.lookSensitivity / 2 );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        rootControls->addControl ( slider );

        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, y, 12, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Turn sensitivity:", textSize ) );
        slider =              new UI_Slider ( "slider_turn",  touchcontrols::RectF ( 13, y, windowRight - 1, y+2 ), SLIDER_TURN , "ui_slider_bg1", "ui_slider_handle");
        slider->setValue(  settings.turnSensitivity / 2 );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        rootControls->addControl ( slider );

        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, y, 12, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Move sensitivity:", textSize ) );
        slider =              new UI_Slider ( "slider_move",    touchcontrols::RectF ( 13, y, windowRight - 1, y+2 ), SLIDER_MOVE, "ui_slider_bg1", "ui_slider_handle" );
        slider->setValue(  settings.moveSensitivity / 2 );
        slider->signal.connect ( sigc::ptr_fun ( &sliderChange ) );
        rootControls->addControl ( slider );


        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",          touchcontrols::RectF ( windownLeft, y, 9.5, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Invert look:", textSize ) );
        UI_Switch *swtch =      new UI_Switch ( "invert_switch", touchcontrols::RectF ( 10, y+0.2, 13, y+1.8 ), SWITCH_INVERT_LOOK, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.invertLook );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

        UI_Button *buttonEdit =   new UI_Button ( "edit_buttons",  touchcontrols::RectF ( 13, y, windowRight, y+2 ), BUTTON_EDIT_BUTTONS, "font_dual", 0, UI_TEXT_CENTRE, "Hide/Show buttons", textSize, "ui_button_bg" );
        buttonEdit->signal.connect ( sigc::ptr_fun ( &buttonPress ) );
        rootControls->addControl ( buttonEdit );

        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",   touchcontrols::RectF ( windownLeft, y, 9.5, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Fixed Move:", textSize ) );
        swtch =      new UI_Switch ( "fixed_move_stick",       touchcontrols::RectF ( 10, y+0.2, 13, y+1.8 ), SWITCH_FIXED_MOVE, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.fixedMoveStick );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

        rootControls->addControl ( new UI_TextBox ( "text",   touchcontrols::RectF ( 13, y, 21, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Joystick Look mode:", textSize ) );
        swtch =      new UI_Switch ( "joystick_look_switch",  touchcontrols::RectF ( 21, y+0.2, 24, y+1.8 ), SWITCH_JOYSTICK_MODE, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.joystickLookMode );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

        y += 2;

        UI_DropDown *dblTapLeft = new UI_DropDown( "dbl_tap_left",  touchcontrols::RectF ( windownLeft, y, 13 , y+2 ), DROPDOWN_DBL_TAP_LEFT,  "font_dual", 0, "Double tap left  :  ", "None:Use:Jump:Fire", textSize, "ui_dropdown_bg" );
        dblTapLeft->setSelected(settings.dblTapLeft);
        rootControls->addControl ( dblTapLeft );
        dblTapLeft->signal.connect(sigc::ptr_fun ( &dropDownChange) );

        UI_DropDown *dblTapRight = new UI_DropDown( "dbl_tap_right",  touchcontrols::RectF ( 13, y, windowRight , y+2 ), DROPDOWN_DBL_TAP_RIGHT,  "font_dual", 0, "Double tap right  :  ", "None:Use:Jump:Fire", textSize, "ui_dropdown_bg" );
        dblTapRight->setSelected(settings.dblTapRight);
        rootControls->addControl ( dblTapRight );
        dblTapRight->signal.connect(sigc::ptr_fun ( &dropDownChange) );


        y += 2;

        rootControls->addControl ( new UI_TextBox ( "text",   touchcontrols::RectF ( windownLeft, y, 9.5, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Auto-hide inven:", textSize ) );
        swtch =      new UI_Switch ( "auto_hide_inventory",       touchcontrols::RectF ( 10, y+0.2, 13, y+1.8 ), SWITCH_HIDE_INV, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.autoHideInventory );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

     	rootControls->addControl ( new UI_TextBox ( "text",   touchcontrols::RectF ( 13, y, 21, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Auto-hide numbers", textSize ) );
        swtch =      new UI_Switch ( "auto_hide_number",  touchcontrols::RectF ( 21, y+0.2, 24, y+1.8 ), SWITCH_HIDE_NBRS, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.autoHideNumbers );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

        y += 2;

   		rootControls->addControl ( new UI_TextBox ( "text",   touchcontrols::RectF ( windownLeft, y, 9.5, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Weapon wheel:", textSize ) );
        swtch =      new UI_Switch ( "weapon_wheel_enabled",       touchcontrols::RectF ( 10, y+0.2, 13, y+1.8 ), SWITCH_WEAP_WHEEL, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.weaponWheelEnabled );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

        rootControls->addControl ( new UI_TextBox ( "text",   touchcontrols::RectF ( 13, y, 21, y+2 ), "font_dual", 0, UI_TEXT_RIGHT, "Show joysticks:", textSize ) );
        swtch =      new UI_Switch ( "show_joy_switch",       touchcontrols::RectF ( 21, y+0.2, 24, y+1.8 ), SWITCH_JOYSTICKS, "ui_switch2_on", "ui_switch2_off" );
        swtch->setValue( settings.showJoysticks );
        swtch->signal.connect(sigc::ptr_fun ( &switchChange) );
        rootControls->addControl ( swtch );

        y += 2;

        UI_Button *buttonReset =   new UI_Button ( "reset_button",  touchcontrols::RectF ( 13 - 5, y, windowRight - 5, y+2 ), BUTTON_RESET, "font_dual", 0, UI_TEXT_CENTRE, "Reset", textSize, "ui_button_bg" );
        buttonReset->signal.connect ( sigc::ptr_fun ( &buttonPress ) );
        rootControls->addControl ( buttonReset );

        rootControls->signalEnable.connect( sigc::ptr_fun(&controlsEnabled) );
    }

    return rootControls;
}



}