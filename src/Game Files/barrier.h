/**
 *	barrier.h - Jan van der Kamp, 2011
 */
#ifndef BARRIER_H_INCLUDED
#define BARRIER_H_INCLUDED

#include <pf/pflib.h>
#include <pf/vec.h>
#include <vector>
#include <list> 

#include "gameVariables.h"
#include "ball.h"

/** @class TBarrier - This class represents a pile of balloons which are at the bottom of the screen and rise when the player lets
 *					  a falling balloon sink below the bottom. It is made up of 6 images of balloons, each with successively smaller
 *					  balloons to give the illusion of depth. These images move up at different speeds to reinforce this illusion.
 * 					  The three far images should be drawn first with DrawBackground(), then the falling balloons and the cannon,
 *					  before the three near images being drawn with DrawForeground(). This gives the illusion of balloons sinking
 *					  into the middle of the pile. If the barrier gets high enough to cover the image of the cannon base, it's 
 *					  game over. This height is determined by the top of the first of the near images, ie the 4th element of
 *					  mBarrierTextures. This image should have a region in the middle at the top that has full opacity.
 *					  This class inherits from IObject for the Draw/Update interface.
 * 
 *	@property 	TVec2		mOriginalPosition			The overall position of the barrier when the game started
 *	@property 	TReal		mLevelHeight				Height at which each element of mBarrierTextures appear at the same height
 *	@property 	TReal		mRiseSpeed					Speed at which barrier rises
 *	@property 	TVec2		mPosition					The overall position of the barrier
 *	@property 	TVec2		mPositionLastFrame			The overall position of the barrier at the last frame
 *	@property 	TReal		mHeightAdjust				When a balloon which has sunk is removed from the game, this value
 *														keeps the barrier at the height it was just before the balloon is removed
 *	@property 	TReal		mParallaxAdjust				Value to scale the difference between mPosition.y and mLevelHeight by
 *	@property 	TReal		mGameOverVisibleHeight		Height at which top level of 4th element of mBarrierTextures must be
 *														before game over. Should be set to just above the bullet at the base of 
 *														the cannon.
 *	@property 	TReal		newGame						Whether a new game has just begun.
 *	@property 	TReal		mBarrierTextures			Textures to represent piles of balloons at the bottom of the screen 
 */
class TBarrier : public IObject
{
public:
	TBarrier(const TVec2& position, TReal riseSpeed, TReal initialParallaxDifference, TReal levelHeight);
	virtual ~TBarrier() {}
	void AssignAssets(const std::vector<TTextureRef>& barrierTextures);
	void Reset() { mHeightAdjust = 0.f; newGame = true; }
	virtual void Update(uint32_t elapsedTime);
	virtual void Draw()	const;
	void SetGameOverHeight(TReal gameOverHeight)	{ mGameOverVisibleHeight = gameOverHeight; 
													  mPosition.y = mGameOverVisibleHeight; }
	void DrawBackground()	const;
	void DrawForeground()	const;
	bool TestForSinkingBalloons(const std::list<TBall>& balloons);
	TVec2 GetPosition()		const		{ return mPosition; }
private:
	// copying disallowed
	TBarrier(const TBarrier &barrier);
	TBarrier& operator=(const TBarrier &barrier);
	TReal TBarrier::RelativeParallaxHeight(std::vector<TTextureRef>::size_type s) const;

	const TVec2					mOriginalPosition;
	const TReal					mLevelHeight;
	const TReal					mRiseSpeed;
	TVec2						mPosition;
	TVec2						mPositionLastFrame;
	TReal						mHeightAdjust;
	TReal						mParallaxAdjust;
	TReal						mGameOverVisibleHeight;
	bool						newGame;
	std::vector<TTextureRef>	mBarrierTextures;
};

#endif //BARRIER_H_INCLUDED