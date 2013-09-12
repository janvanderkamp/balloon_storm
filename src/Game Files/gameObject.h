/**
 *	gameObject.h - Jan van der Kamp, 2011
 */
#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED

#include <pf/pflib.h>

/** @class IObject - This abstract base class is inheritted from by TBall, TCannon, TBarrier, and TBalloonManager,
 *					 in order for these classes to be used polymorphically when Drawing/Updating in TGame.
 */
class IObject {
public:
	virtual void Draw() const = 0;  
	virtual void Update(uint32_t elapsedTime) = 0;  
	virtual ~IObject() {}
};

#endif GAMEOBJECT_H_INCLUDED