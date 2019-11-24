#include "UI_Switch.h"
#include "TouchControlsConfig.h"


#define TAP_TIME  250

using namespace touchcontrols;

UI_Switch::UI_Switch(std::string tag, RectF pos, uint32_t uid, std::string on_image, std::string off_image) : ControlSuper(TC_TYPE_UI_SWITCH, tag, pos)
{
	this->on_image = on_image;
	this->off_image = off_image;
	this->uid = uid;

	glTexOn = glTexOff = 0;

	isOn = false;

	updateSize();
}

bool UI_Switch::getValue()
{
	return isOn;
}

void UI_Switch::setValue(bool v)
{
	isOn = v;
}

void UI_Switch::updateSize()
{
	glRect.resize(controlPos.right - controlPos.left, controlPos.bottom - controlPos.top);
}

bool UI_Switch::processPointer(int action, int pid, float x, float y)
{
	if(action == P_DOWN)
	{
		if(controlPos.contains(x, y))
		{
			touchId = pid;
			tapDetect.reset();
			tapDetect.processPointer(action, pid, x, y);
			return true;
		}
	}
	else if(action == P_UP)
	{
		tapDetect.processPointer(action, pid, x, y);

		if(touchId == pid)
		{
			if(tapDetect.didTap())
			{
				// Toggle switch
				isOn = !isOn;
				signal.emit(uid, isOn);
			}

			touchId = -1;
		}
	}
	else if(action == P_MOVE)
	{
		tapDetect.processPointer(action, pid, x, y);
	}

	return false;
}

void UI_Switch::resetOutput()
{

}

bool UI_Switch::initGL()
{
	int x, y;
	glTexOn = loadTextureFromPNG(on_image, x, y);
	glTexOff = loadTextureFromPNG(off_image, x, y);
	return false;
}


bool UI_Switch::drawGL(bool forEditor)
{
	//setScissor();
	if(isOn)
		gl_drawRect(glTexOn, controlPos.left, controlPos.top, glRect);
	else
		gl_drawRect(glTexOff, controlPos.left, controlPos.top, glRect);

	return false;
}

void UI_Switch::saveXML(TiXmlDocument &doc)
{

}

void UI_Switch::loadXML(TiXmlDocument &doc)
{

}
