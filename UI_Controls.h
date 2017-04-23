//
// Created by Emile on 18/04/2017.
//

#ifndef OPENTOUCH_UI_CONTROLS_H
#define OPENTOUCH_UI_CONTROLS_H


#include <string>
#include <stdbool.h>
#include <vector>

#include "ControlSuper.h"
#include "PointF.h"

namespace touchcontrols
{


class UI_Controls
{
public:
    UI_Controls( std::string t );

    bool processPointer(int action, int pid, float x, float y);
	int draw ();

    void addControl( ControlSuper *cntrl );
    void setEnabled(bool v);
    bool getEnabled();


	void initGL ();
	void setAlpha(float a);
    void fade(fadedir_t dir,int steps);

private:
    std::string tag;
    std::vector<ControlSuper *> controls;
	bool enabled;

	float yOffset; // Used to scroll up/down
    PointF finger1;
    bool scrolling;
};

}
#endif //OPENTOUCH_UI_CONTROLS_H
