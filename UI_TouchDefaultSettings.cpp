#include "UI_TouchDefaultSettings.h"
#include "UI_ButtonListWindow.h"

namespace touchcontrols
{

#define BUTTON_CLOSE 1
#define BUTTON_RESET 2
#define BUTTON_EDIT_BUTTONS 3
#define BUTTON_RESET_POS 4

#define SLIDER_ALPHA 10
#define SLIDER_LOOK  11
#define SLIDER_FWD   12
#define SLIDER_TURN  13
#define SLIDER_STRAFE  14

#define SLIDER_PRECISION  15
#define SLIDER_DEADZONE   16

#define SWITCH_DIGITAL_MOVE   19
#define SWITCH_INVERT_LOOK   20
#define SWITCH_JOYSTICKS     22
#define SWITCH_JOYSTICK_MODE 23
#define SWITCH_HIDE_INV      24
#define SWITCH_HIDE_NBRS     25
#define SWITCH_WEAP_WHEEL    26
#define SWITCH_FIXED_MOVE    27
#define SWITCH_PRECISION_SHOOT    28
#define SWITCH_SHOW_CUSTOM        29

#define DROPDOWN_DBL_TAP_LEFT 30
#define DROPDOWN_DBL_TAP_RIGHT 31

#define DEFAULT_COLOR 32

#define DROPDOWN_VOLUME_UP 33
#define DROPDOWN_VOLUME_DOWN 34


static TouchControlsContainer *container = NULL;
static UI_Controls *rootControls = NULL;
static std::string settingsFilename;
static tTouchSettings settings;

static  sigc::signal<void, tTouchSettings> signal_settingChange;

static void applyControlValues();

bool touchSettings_save(std::string filename)
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	doc.LinkEndChild(decl);

	TiXmlElement * root = new TiXmlElement("settings");
	doc.LinkEndChild(root);

	root->SetAttribute("digital_move", settings.digitalMove);
	root->SetAttribute("invert_look", settings.invertLook);
	root->SetAttribute("show_sticks", settings.showJoysticks);
	root->SetAttribute("joystick_mode", settings.joystickLookMode);
	root->SetAttribute("auto_hide_inventory", settings.autoHideInventory);
	root->SetAttribute("auto_hide_numbers", settings.autoHideNumbers);
	root->SetAttribute("weapon_wheel_enabled", settings.weaponWheelEnabled);
	root->SetAttribute("fixed_move_stick", settings.fixedMoveStick);
	root->SetAttribute("precision_shoot", settings.precisionShoot);
	root->SetAttribute("show_custom", settings.alwaysShowCust);

	root->SetDoubleAttribute("alpha", settings.alpha);
	root->SetDoubleAttribute("look_sens", settings.lookSensitivity);
	root->SetDoubleAttribute("turn_sens", settings.turnSensitivity);
	root->SetDoubleAttribute("fwd_sens", settings.fwdSensitivity);
	root->SetDoubleAttribute("strafe_sens", settings.strafeSensitivity);
	root->SetDoubleAttribute("deadzone_sens", settings.deadzoneSensitivity);

	root->SetDoubleAttribute("precision_sens", settings.precisionSenitivity);

	root->SetAttribute("dbl_tap_left", settings.dblTapLeft);
	root->SetAttribute("dbl_tap_right", settings.dblTapRight);

	root->SetAttribute("volume_up", settings.volumeUp);
	root->SetAttribute("volume_down", settings.volumeDown);

	root->SetAttribute("default_color", settings.defaultColor);

	LOGTOUCH("Saving settings to %s\n", filename.c_str());
	return doc.SaveFile(filename);
}

bool touchSettings_load(std::string filename)
{
	LOGTOUCH("Loading settings from %s\n", filename.c_str());

	bool ok = false;

	TiXmlDocument *doc = new TiXmlDocument(filename.c_str());

	if(!doc->LoadFile())
	{
		LOGTOUCH("Failed to open settings file\n");
	}
	else
	{

		TiXmlHandle hDoc(doc);
		TiXmlElement* root = hDoc.FirstChild("settings").Element();

		root->QueryBoolAttribute("digital_move", &settings.digitalMove);
		root->QueryBoolAttribute("invert_look", &settings.invertLook);
		root->QueryBoolAttribute("show_sticks", &settings.showJoysticks);
		root->QueryBoolAttribute("joystick_mode", &settings.joystickLookMode);
		root->QueryBoolAttribute("auto_hide_inventory", &settings.autoHideInventory);
		root->QueryBoolAttribute("auto_hide_numbers", &settings.autoHideNumbers);
		root->QueryBoolAttribute("weapon_wheel_enabled", &settings.weaponWheelEnabled);
		root->QueryBoolAttribute("fixed_move_stick", &settings.fixedMoveStick);
		root->QueryBoolAttribute("precision_shoot", &settings.precisionShoot);
		root->QueryBoolAttribute("show_custom", &settings.alwaysShowCust);

		root->QueryFloatAttribute("alpha",  &settings.alpha);
		root->QueryFloatAttribute("look_sens",  &settings.lookSensitivity);
		root->QueryFloatAttribute("turn_sens",  &settings.turnSensitivity);
		root->QueryFloatAttribute("fwd_sens",  &settings.fwdSensitivity);
		root->QueryFloatAttribute("strafe_sens",  &settings.strafeSensitivity);
		root->QueryFloatAttribute("deadzone_sens",  &settings.deadzoneSensitivity);

		root->QueryFloatAttribute("precision_sens",  &settings.precisionSenitivity);

		root->QueryUnsignedAttribute("dbl_tap_left",  &settings.dblTapLeft);
		root->QueryUnsignedAttribute("dbl_tap_right",  &settings.dblTapRight);

		root->QueryUnsignedAttribute("volume_up",  &settings.volumeUp);
		root->QueryUnsignedAttribute("volume_down",  &settings.volumeDown);

		root->QueryUnsignedAttribute("default_color",  &settings.defaultColor);
		ok = true;
	}

	signal_settingChange.emit(settings);

	applyControlValues();

	delete doc;

	return ok;
}

bool touchSettings_save()
{
	return touchSettings_save(settingsFilename);
}

static void resetDefaults()
{
	settings.alpha = 0.5;
	settings.invertLook = false;
	settings.lookSensitivity = 1;
	settings.turnSensitivity = 1;
	settings.fwdSensitivity = 1;
	settings.strafeSensitivity = 1;
	settings.deadzoneSensitivity = 0.1;

	settings.digitalMove = false;
	settings.showJoysticks = true;
	settings.joystickLookMode = false;
	settings.autoHideInventory = true;
	settings.autoHideNumbers = true;
	settings.weaponWheelEnabled = true;
	settings.fixedMoveStick = false;
	settings.precisionShoot = false;
	settings.precisionSenitivity = 0.3;
	settings.alwaysShowCust = false;

	settings.dblTapLeft = 0;
	settings.dblTapRight = 0;
	settings.volumeUp = 0;
	settings.volumeDown = 0;

	settings.defaultColor = COLOUR_WHITE;

	signal_settingChange.emit(settings);
}

static void applyControlValues()
{
	if(rootControls != NULL)
	{
		((UI_Slider *)rootControls->getControl("slider_alpha"))->setValue(settings.alpha);
		((UI_Slider *)rootControls->getControl("slider_look"))->setValue(settings.lookSensitivity / 2);
		((UI_Slider *)rootControls->getControl("slider_turn"))->setValue(settings.turnSensitivity / 2);
		((UI_Slider *)rootControls->getControl("slider_fwd"))->setValue(settings.fwdSensitivity / 2);
		((UI_Slider *)rootControls->getControl("slider_strafe"))->setValue(settings.strafeSensitivity / 2);
		((UI_Slider *)rootControls->getControl("slider_deadzone"))->setValue(settings.deadzoneSensitivity);

		((UI_ColorPicker *)rootControls->getControl("color_picker"))->setColor(settings.defaultColor);

		((UI_Switch *)rootControls->getControl("digital_move"))->setValue(settings.digitalMove);
		((UI_Switch *)rootControls->getControl("invert_switch"))->setValue(settings.invertLook);
		((UI_Switch *)rootControls->getControl("fixed_move_stick"))->setValue(settings.fixedMoveStick);
		((UI_Switch *)rootControls->getControl("joystick_look_switch"))->setValue(settings.joystickLookMode);

		((UI_Switch *)rootControls->getControl("auto_hide_inventory"))->setValue(settings.autoHideInventory);
		((UI_Switch *)rootControls->getControl("auto_hide_number"))->setValue(settings.autoHideNumbers);
		((UI_Switch *)rootControls->getControl("weapon_wheel_enabled"))->setValue(settings.weaponWheelEnabled);
		((UI_Switch *)rootControls->getControl("show_joy_sticks"))->setValue(settings.showJoysticks);
		((UI_Switch *)rootControls->getControl("show_custom"))->setValue(settings.alwaysShowCust);

		((UI_Switch *)rootControls->getControl("precision_shoot"))->setValue(settings.precisionShoot);
		((UI_Slider *)rootControls->getControl("slider_precision"))->setValue((settings.precisionSenitivity - 0.2f) / 0.5f);

		((UI_DropDown *)rootControls->getControl("dbl_tap_left"))->setSelected(settings.dblTapLeft);
		((UI_DropDown *)rootControls->getControl("dbl_tap_right"))->setSelected(settings.dblTapRight);

		((UI_DropDown *)rootControls->getControl("volume_up"))->setSelected(settings.volumeUp);
		((UI_DropDown *)rootControls->getControl("volume_down"))->setSelected(settings.volumeDown);
	}
}

static void buttonPress(int state, int code)
{
	if(state == 0 && code == UI_WINDOW_BUTTON_BACK)
	{
		rootControls->setEnabled(false);    // controlsEnabled will get called automatically
		LOGTOUCH("Window close");
	}

	if(code == BUTTON_RESET)
	{
		resetDefaults();
		applyControlValues();
	}
	else if(code == BUTTON_RESET_POS)
	{
		container->resetDefaults();
	}
	else if(code == BUTTON_EDIT_BUTTONS)
	{
		showButtonListWindow(container);
	}
}

static void sliderChange(uint32_t uid, float value)
{
	if(uid == SLIDER_ALPHA)
	{
		settings.alpha = value;
	}
	else if(uid == SLIDER_LOOK)
	{
		settings.lookSensitivity = value * 2;
	}
	else if(uid == SLIDER_TURN)
	{
		settings.turnSensitivity = value * 2;
	}
	else if(uid == SLIDER_FWD)
	{
		settings.fwdSensitivity = value * 2;
	}
	else if(uid == SLIDER_STRAFE)
	{
		settings.strafeSensitivity = value * 2;
	}
	else if(uid == SLIDER_DEADZONE)
	{
		settings.deadzoneSensitivity = value;
	}
	else if(uid == SLIDER_PRECISION)
	{
		settings.precisionSenitivity =  0.2 + (value * 0.5);
	}
}

static void switchChange(uint32_t uid, bool value)
{
	if(uid == SWITCH_INVERT_LOOK)
	{
		settings.invertLook = value;
	}
	else if(uid == SWITCH_JOYSTICKS)
	{
		settings.showJoysticks = value;
		signal_settingChange.emit(settings);
	}
	else if(uid == SWITCH_JOYSTICK_MODE)
	{
		settings.joystickLookMode = value;
	}
	else if(uid == SWITCH_HIDE_INV)
	{
		settings.autoHideInventory = value;
	}
	else if(uid == SWITCH_HIDE_NBRS)
	{
		settings.autoHideNumbers = value;
	}
	else if(uid == SWITCH_WEAP_WHEEL)
	{
		settings.weaponWheelEnabled = value;
	}
	else if(uid == SWITCH_FIXED_MOVE)
	{
		settings.fixedMoveStick = value;
	}
	else if(uid == SWITCH_PRECISION_SHOOT)
	{
		settings.precisionShoot = value;
	}
	else if(uid == SWITCH_SHOW_CUSTOM)
	{
		settings.alwaysShowCust = value;
	}
	else if(uid == SWITCH_DIGITAL_MOVE)
	{
		settings.digitalMove = value;
	}
}

static void dropDownChange(uint32_t uid, uint32_t value)
{
	if(uid == DROPDOWN_DBL_TAP_LEFT)
	{
		settings.dblTapLeft = value;
	}
	else if(uid == DROPDOWN_DBL_TAP_RIGHT)
	{
		settings.dblTapRight = value;
	}
	else if(uid == DROPDOWN_VOLUME_UP)
	{
		settings.volumeUp = value;
	}
	else if(uid == DROPDOWN_VOLUME_DOWN)
	{
		settings.volumeDown = value;
	}
}

static bool colorChange(uint32_t uid, uint32_t color)
{
	if(color != COLOUR_NONE)
	{
		settings.defaultColor = color;
		((UI_ColorPicker *)rootControls->getControl("color_picker"))->setColor(settings.defaultColor);
		return false; //OK color
	}
	else
		return true; //return true so picker does not close
}

// Gets called by a signal when the controls are enabled or disabled
static void controlsEnabled(bool enabled)
{
	if(!enabled)
	{
		touchSettings_save(settingsFilename);
		signal_settingChange.emit(settings);
	}
}


sigc::signal<void, tTouchSettings> *getSettingsSignal()
{
	return &signal_settingChange;
}


UI_Controls *createDefaultSettingsUI(TouchControlsContainer *con, std::string settingsFile)
{
	settingsFilename = settingsFile;
	container = con;

	if(rootControls == NULL)
	{
		//Defaults
		resetDefaults();

		// Then try and load file
		touchSettings_load(settingsFilename);


		rootControls = new UI_Controls("ui_settings");

		float textSize = 0.07f;
		uint32_t windownLeft = 3;
		uint32_t windowRight = 24;

		//rootControls->addControl ( new UI_TextBox ( "text",         touchcontrols::RectF ( windownLeft, 2, 22, 4 ), "font_dual", 1, UI_TEXT_CENTRE, "Touch settings", 0.09 ) );
		// rootControls->addControl ( new Button ( "close", touchcontrols::RectF ( windownLeft, 2, windownLeft + 2, 4 ), "ui_back_arrow", BUTTON_CLOSE ) );
		float y = 1;;

		// Draws backwards so need background last
		UI_Window *window =  new UI_Window("bg_window", touchcontrols::RectF(windownLeft, y, windowRight, 14), "Touch settings", "ui_background");
		rootControls->addControl(window);
		window->signal.connect(sigc::ptr_fun(&buttonPress));

		y += 2;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Transparency:", textSize, COLOUR_ORANGE));
		UI_Slider *slider =   new UI_Slider("slider_alpha",  touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_ALPHA, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 2.2;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Look Up/Down sens:", textSize));
		slider =              new UI_Slider("slider_look",  touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_LOOK, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 1.5;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Look Left/Right sens:", textSize));
		slider =              new UI_Slider("slider_turn",  touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_TURN, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 2.2;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Move Fwd/Back sens:", textSize, COLOUR_GREEN3));
		slider =              new UI_Slider("slider_fwd",    touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_FWD, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 1.5;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Move Strafe sens:", textSize, COLOUR_GREEN3));
		slider =              new UI_Slider("slider_strafe",    touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_STRAFE, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 1.5;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Move dead-zone:", textSize, COLOUR_GREEN3));
		slider =              new UI_Slider("slider_deadzone",    touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_DEADZONE, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 1.5;

		rootControls->addControl(new UI_TextBox("text",         touchcontrols::RectF(windownLeft, y, 12, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Digital move (WASD):", textSize, COLOUR_GREEN3));
		UI_Switch *swtch =      new UI_Switch("digital_move", touchcontrols::RectF(13, y + 0.2, 16, y + 1.8), SWITCH_DIGITAL_MOVE, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		y += 2;

		rootControls->addControl(new UI_TextBox("text",          touchcontrols::RectF(windownLeft, y, 9.5, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Default color:", textSize));
		UI_ColorPicker *colorPicker = new UI_ColorPicker("color_picker",  touchcontrols::RectF(10, y + 0.2, 13, y + 1.8), DEFAULT_COLOR, 0);
		rootControls->addControl(colorPicker);
		colorPicker->signal.connect(sigc::ptr_fun(&colorChange));

		UI_Button *buttonEdit =   new UI_Button("edit_buttons",  touchcontrols::RectF(13, y, windowRight, y + 2), BUTTON_EDIT_BUTTONS, "font_dual", 0, UI_TEXT_CENTRE, "Hide/Show buttons", textSize, "ui_button_bg");
		buttonEdit->signal.connect(sigc::ptr_fun(&buttonPress));
		rootControls->addControl(buttonEdit);

		y += 2;

		rootControls->addControl(new UI_TextBox("text",          touchcontrols::RectF(windownLeft, y, 9.5, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Invert look:", textSize));
		swtch =      new UI_Switch("invert_switch", touchcontrols::RectF(10, y + 0.2, 13, y + 1.8), SWITCH_INVERT_LOOK, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(13, y, 21, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Joystick Look:", textSize));
		swtch =      new UI_Switch("joystick_look_switch",  touchcontrols::RectF(21, y + 0.2, 24, y + 1.8), SWITCH_JOYSTICK_MODE, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		y += 2;

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(windownLeft, y, 9.5, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Fixed Move:", textSize));
		swtch =      new UI_Switch("fixed_move_stick",       touchcontrols::RectF(10, y + 0.2, 13, y + 1.8), SWITCH_FIXED_MOVE, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(13, y, 21, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Show Custom:", textSize));
		swtch =      new UI_Switch("show_custom",  touchcontrols::RectF(21, y + 0.2, 24, y + 1.8), SWITCH_SHOW_CUSTOM, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		y += 2;

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(windownLeft, y, 9.5, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Precision shoot:", textSize));
		swtch =      new UI_Switch("precision_shoot",  touchcontrols::RectF(10, y + 0.2, 13, y + 1.8), SWITCH_PRECISION_SHOOT, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		slider =   new UI_Slider("slider_precision",  touchcontrols::RectF(13, y, windowRight - 1, y + 2), SLIDER_PRECISION, "ui_slider_bg1", "ui_slider_handle");
		slider->signal.connect(sigc::ptr_fun(&sliderChange));
		rootControls->addControl(slider);

		y += 2;

		UI_DropDown *dblTapLeft = new UI_DropDown("dbl_tap_left",  touchcontrols::RectF(windownLeft, y, 13, y + 2), DROPDOWN_DBL_TAP_LEFT,  "font_dual", 0, "Double tap left  :  ", "None:Use:Jump:Fire:Alt-fire", textSize, "ui_dropdown_bg");
		rootControls->addControl(dblTapLeft);
		dblTapLeft->signal.connect(sigc::ptr_fun(&dropDownChange));

		UI_DropDown *dblTapRight = new UI_DropDown("dbl_tap_right",  touchcontrols::RectF(13, y, windowRight, y + 2), DROPDOWN_DBL_TAP_RIGHT,  "font_dual", 0, "Double tap right  :  ", "None:Use:Jump:Fire:Alt-fire", textSize, "ui_dropdown_bg");
		rootControls->addControl(dblTapRight);
		dblTapRight->signal.connect(sigc::ptr_fun(&dropDownChange));

		y += 2;

		UI_DropDown *volumeUp = new UI_DropDown("volume_up",  touchcontrols::RectF(windownLeft, y, 13, y + 2), DROPDOWN_VOLUME_UP,  "font_dual", 0, "Volume btn up  :  ", "None:Use:Jump:Fire:Alt-fire", textSize, "ui_dropdown_bg");
		rootControls->addControl(volumeUp);
		volumeUp->signal.connect(sigc::ptr_fun(&dropDownChange));

		UI_DropDown *volumeDown = new UI_DropDown("volume_down",  touchcontrols::RectF(13, y, windowRight, y + 2), DROPDOWN_VOLUME_DOWN,  "font_dual", 0, "Volume btn down  :  ", "None:Use:Jump:Fire:Alt-fire", textSize, "ui_dropdown_bg");
		rootControls->addControl(volumeDown);
		volumeDown->signal.connect(sigc::ptr_fun(&dropDownChange));

		y += 2;

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(windownLeft, y, 9.5, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Auto-hide inven:", textSize));
		swtch =      new UI_Switch("auto_hide_inventory",       touchcontrols::RectF(10, y + 0.2, 13, y + 1.8), SWITCH_HIDE_INV, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(13, y, 21, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Auto-hide numbers", textSize));
		swtch =      new UI_Switch("auto_hide_number",  touchcontrols::RectF(21, y + 0.2, 24, y + 1.8), SWITCH_HIDE_NBRS, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		y += 2;

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(windownLeft, y, 9.5, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Weapon wheel:", textSize));
		swtch =      new UI_Switch("weapon_wheel_enabled",       touchcontrols::RectF(10, y + 0.2, 13, y + 1.8), SWITCH_WEAP_WHEEL, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		rootControls->addControl(new UI_TextBox("text",   touchcontrols::RectF(13, y, 21, y + 2), "font_dual", 0, UI_TEXT_RIGHT, "Show joysticks:", textSize));
		swtch =      new UI_Switch("show_joy_sticks",       touchcontrols::RectF(21, y + 0.2, 24, y + 1.8), SWITCH_JOYSTICKS, "ui_switch2_on", "ui_switch2_off");
		swtch->signal.connect(sigc::ptr_fun(&switchChange));
		rootControls->addControl(swtch);

		y += 2;

		UI_Button *buttonReset =   new UI_Button("reset_button_pos",  touchcontrols::RectF(windownLeft + 1, y, 13 - 1, y + 2), BUTTON_RESET_POS, "font_dual", 0, UI_TEXT_CENTRE, "Reset positions", textSize, "ui_button_bg");
		buttonReset->signal.connect(sigc::ptr_fun(&buttonPress));
		rootControls->addControl(buttonReset);

		UI_Button *buttonResetSettings =   new UI_Button("reset_button_set",  touchcontrols::RectF(13 + 1, y, windowRight - 1, y + 2), BUTTON_RESET, "font_dual", 0, UI_TEXT_CENTRE, "Reset settings", textSize, "ui_button_bg");
		buttonResetSettings->signal.connect(sigc::ptr_fun(&buttonPress));
		rootControls->addControl(buttonResetSettings);

		applyControlValues();

		rootControls->signalEnable.connect(sigc::ptr_fun(&controlsEnabled));
	}

	return rootControls;
}



}