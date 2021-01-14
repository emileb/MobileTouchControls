

#ifndef _UI_TouchDefaultSettings_H_
#define _UI_TouchDefaultSettings_H_

#include "UI_Controls.h"
#include "TouchControlsContainer.h"

namespace touchcontrols
{

typedef struct
{
	float alpha;
	float lookSensitivity;
	float turnSensitivity;
	float fwdSensitivity;
	float strafeSensitivity;
	float deadzoneSensitivity;

	float precisionSenitivity;

	bool invertLook;
	bool showJoysticks;
	bool joystickLookMode;
	bool autoHideInventory;
	bool autoHideNumbers;
	bool weaponWheelEnabled;
	bool fixedMoveStick;
	bool precisionShoot;

	bool alwaysShowCust;

	uint32_t dblTapLeft;
	uint32_t dblTapRight;

	uint32_t volumeUp;
	uint32_t volumeDown;

	uint32_t defaultColor;

} tTouchSettings;


UI_Controls *createDefaultSettingsUI(TouchControlsContainer *con, std::string settingsFile);
sigc::signal<void, tTouchSettings> *getSettingsSignal();

bool touchSettings_save();
bool touchSettings_save(std::string filename);
bool touchSettings_load(std::string filename);


}

#endif