/**
 *	barrier.cpp - Jan van der Kamp, 2011
 */
#include "barrier.h"

using std::vector;
using std::list;

/** @function TBarrier::TBarrier - Constructor			Takes parameters to construct TBarrier with
 *
 *		@param 		position						Initial position of the barrier.
 *		@param 		riseSpeed						Speed at which barrier rises
 *		@param 		initialParallaxDifference		Difference in height between the first and last images of balloons which  
 *													make up the barrier
 *		@param 		levelHeight						Height at which each element of mBarrierTextures appear at a level height
 */
TBarrier::TBarrier(const TVec2& position, TReal riseSpeed, TReal initialParallaxDifference, TReal levelHeight) : 
mOriginalPosition(position), 
mLevelHeight(levelHeight),
mRiseSpeed(riseSpeed), 
mPosition(position), 
mPositionLastFrame(),
mHeightAdjust(0.f), 
mParallaxAdjust(),
mGameOverVisibleHeight(),
newGame(true),
mBarrierTextures()
{
	mParallaxAdjust = (initialParallaxDifference / (position.y - levelHeight));
}

/** @function TBarrier::AssignAssets - Seperate function to assign image assets to TBarrier. AssignAssets is used so that TBarrier's 
 *									   constructor can be called by TGame's default constructor, and AssignAssets should then be called
 *									   in TGame's default constructor.
 *		@param 		barrierTextures				Textures to represent piles of balloons at the bottom of the screen
 */
void TBarrier::AssignAssets(const std::vector<TTextureRef>& barrierTextures)
{
	mBarrierTextures = barrierTextures;
	
	mParallaxAdjust /= mBarrierTextures.size();
}

/** @function TBarrier::Update - If a new game has just begun, this function will gradually lower the piles of balloons
 *								 to an acceptable height.
 *		@param 		elapsedTime			Time in milliseconds since last frame
 */
void TBarrier::Update(uint32_t elapsedTime)
{
	if(newGame) 
	{
		mPosition.y += TReal(elapsedTime) * mRiseSpeed;
		if(mPosition.y >= mOriginalPosition.y)
		{
			mPosition = mOriginalPosition;
			newGame = false;
		}	
	}
}

/** @function TBarrier::Draw - Draws all the balloon images together 
*/
void TBarrier::Draw() const
{
	DrawBackground();
	DrawForeground();
}

/** @function TBarrier::DrawBackground - Draws the background images together. Should be called early on in TGame::Draw()
*/
void TBarrier::DrawBackground() const
{
	for(vector<TTextureRef>::size_type s = 0; s != mBarrierTextures.size() / 2; ++s) {
		TReal heightAdjust = RelativeParallaxHeight(s);
		mBarrierTextures[s]->DrawSprite(mPosition.x, mPosition.y + heightAdjust);
	}
}

/** @function TBarrier::DrawForeground - Draws the foreground images together. Should be called later on in TGame::Draw()
*/
void TBarrier::DrawForeground() const
{
	for(vector<TTextureRef>::size_type s = mBarrierTextures.size() / 2; 
		s != mBarrierTextures.size(); ++s) 
		{
			TReal heightAdjust = RelativeParallaxHeight(s);
			mBarrierTextures[s]->DrawSprite(mPosition.x, mPosition.y + heightAdjust);
		}
}

/** @function TBarrier::TestForSinkingBalloons - This function tests for any balloons which have sunk below the lower boundary.
 *												 It loops through all balloons and adds together the amount that each one may 
 *												 have sunk below the lower boundary before decreasing mPosition.y by this amount.
 *												 If a balloon sinks the lowest amount possible and is removed from the game the
 *												 barrier whouls stay in the same position, so this is handled by mHeightAdjust.
 *												 If the barrier has risen so much that it covers the balloon at the base of the cannon
 *												 (mGameOverVisibleHeight) the function returns true, otherwise it returns false.												 
 *		@param 		balloons			Falling balloons on screen currently
 *
 *		@return		true if barrier has risen too far, which means game over
 */
bool TBarrier::TestForSinkingBalloons(const std::list<TBall>& balloons)
{
	// early 'out' if no balloons to test for or new game begun
	if(balloons.size() == 0 || newGame)
		return false;

	mPositionLastFrame = mPosition;
	TReal tempHeightAdjust = 0.f;

	// Loop through all balloons on screen and if any are below mPosition, 
	// add the difference to tempHeightAdjust
	for(list<TBall>::const_iterator balloon = balloons.begin(); 
		balloon != balloons.end(); ++balloon) 
		{
			if(balloon->GetPosition().y > mPosition.y)
				tempHeightAdjust += (balloon->GetPosition().y - mPosition.y) * mRiseSpeed;
		}
	
	mPosition.y = (mOriginalPosition.y - tempHeightAdjust - mHeightAdjust);
	TReal lastFramePosDifference = mPosition.y - mPositionLastFrame.y;
	
	// balloon has been removed due to falling below boundary, make allowance for this
	if(lastFramePosDifference >= 1.f) {
		mHeightAdjust += lastFramePosDifference;
		mPosition.y -= lastFramePosDifference;
	}

	// If barrier is too high, return true, game over.
	vector<TTextureRef>::size_type s = mBarrierTextures.size() / 2;
	TReal barrierOffset = RelativeParallaxHeight(s);
	// visibleHeight will give the top-most point of the first of the foreground textures
	TReal visibleHeight = mPosition.y + 
						  barrierOffset - 
						  TReal(mBarrierTextures[s]->GetHeight()) / 2;
	if(visibleHeight < mGameOverVisibleHeight)
		return true;
	
	return false;
}

/** @function TBarrier::RelativeParallaxHeight - This function takes an index of the queried texture and returns a TReal which 
 *												 should be added to mPosition when drawing this particular image on screen. 
 *												 This value will be smaller for elements at the beginning of mBarrierTextures
 *												 then elements toward the end of mBarrierTextures
 *		@param 		s			Index of the queried image
 *
 *		@return		Value needed to add to mPosition when drawing mBarrierTextures[s] to give the illusion of depth 
 */
TReal TBarrier::RelativeParallaxHeight(vector<TTextureRef>::size_type s) const
{
	TReal heightAdjust = s * mParallaxAdjust * (mPosition.y - mLevelHeight);

	// Make allowance for the difference in height between this and the first element of mBarrierTextures
	TReal temp = (mBarrierTextures[s]->GetHeight() - mBarrierTextures[0]->GetHeight()) / 2;
	heightAdjust += temp;
	return heightAdjust;
}