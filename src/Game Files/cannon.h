/**
 *	cannon.h - Jan van der Kamp, 2011
 */
#ifndef CANNON_H_INCLUDED
#define CANNON_H_INCLUDED

#include <pf/pflib.h>
#include <pf/vec.h>

#include <vector>
#include <list>

#include "gameVariables.h"
#include "ball.h"

/** @class TCannon - This class represents the cannon which can fire bullets. The angle of the cannon is determined by the
 *					 position of the mouse cursor. Bullets are represented by the TBall class. They are loaded onto the end of 
 *					 the cannon with the left mouse down event, and fired with the left mouse up event.
 *				     This class inherits from IObject for the Draw/Update interface.
 *	@property 	TVec2								mDirectionAtRest		Direction that gives 0.f for mAngle
 *	@property 	TReal								mBulletScale			Size of the bullets fired
 *	@property 	uint16_t							mNumColoursInPlay		Range of colours that bullets can be
 *	@property 	TVec2								mPosition				Position of the base of the cannon
 *	@property 	TVec2								mLoadedBulletPosition	Position of bullet before firing (at end of cannon)
 *	@property 	TDrawSpec							mDrawSpec				TDrawSpec for the cannon image
 *	@property 	TReal								mAngle					Angle that cannon makes with mDirectionAtRest
 *	@property 	std::list<TBall>					mBullets				Bullets loaded on cannon, always <= 2
 *	@property 	std::list<TBall>					mBulletsFired			Bullets that have been fired
 *	@property 	TTextureRef							mCannonTexture			Texture used to display cannon on screen
 *	@property 	std::vector<TTextureRef>			mBalloonTextures		Textures used to display bullets on screen
 *	@property 	std::vector<TAnimatedTextureRef>	mBalloonBurstTextures	Animated textures for when bullets have been burst
 */

class TCannon : public IObject
{
public:
	TCannon(const TVec2& position, const TReal& bulletScale, uint16_t numColoursInPlay);
	virtual ~TCannon() {}
	virtual void		Draw() const;
	virtual void		Update(uint32_t elapsedTime);
	void				AssignAssets(const TTextureRef& cannonTexture,	const std::vector<TTextureRef>& balloonTextures, 
									 const std::vector<TAnimatedTextureRef>& balloonBurstTextures);
	void				UpdateMousePosition(const TPoint& p);
	void				Reload();
	void				Fire();
	void				SetNumColours(uint16_t numColours); 
	const TVec2&		GetPosition()		{ return mPosition; }
	const TReal			GetBulletRadius()	{ return mBalloonTextures[0]->GetWidth() * (mBulletScale / 2); }
	std::list<TBall>&	GetBulletsFired()		{ return mBulletsFired; }
private:
	// copying disallowed
	TCannon(const TCannon &cannon);
	TCannon& operator=(const TCannon &cannon);
	void				CleanUpContents();
	
	const TVec2							mDirectionAtRest;
	const TReal							mBulletScale;
	uint16_t							mNumColoursInPlay;
	TVec2								mPosition;
	TVec2								mLoadedBulletPosition;
	TDrawSpec							mDrawSpec;
	TReal								mAngle;
	std::list<TBall>					mBullets;
	std::list<TBall>					mBulletsFired;
	TTextureRef							mCannonTexture;
	std::vector<TTextureRef>			mBalloonTextures;
	std::vector<TAnimatedTextureRef>	mBalloonBurstTextures;
};


#endif //CANNON_H_INCLUDED