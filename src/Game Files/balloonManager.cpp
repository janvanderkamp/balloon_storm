/**
 *	balloonManager.cpp - Jan van der Kamp, 2011
 */
#include "balloonManager.h"

using std::vector;
using std::list;

/** @function TStateVariables::TStateVariables - Constructor			Takes parameters to construct TStateVariables with
 *		@param 		minWaitForBalloon				Current shortest time to wait for new balloon
 *		@param 		maxWaitForBalloon				Current longest time to wait for new balloon
 *		@param 		balloonsBurstToLevelUp			Number of balloons necessary to burst in order to level up
 *		@param 		balloonVelocity					Velocity of balloons
 *		@param 		balloonVelocityIncrease			Amount that mBalloonVelocity.y increases by when new level is reached
 *		@param 		numColoursInPlay				Range of colours that bullets can be
 */
TStateVariables::TStateVariables(uint16_t minWaitForBalloon, uint16_t maxWaitForBalloon, uint16_t balloonsBurstToLevelUp, 
								 uint16_t waitTimeDecrease, const TVec2& balloonVelocity, TReal balloonVelocityIncrease,
								 uint16_t numColoursInPlay) :
								 mMinWaitForBalloon(minWaitForBalloon),
								 mMaxWaitForBalloon(maxWaitForBalloon),
								 mCurrentWaitForBalloon((minWaitForBalloon + maxWaitForBalloon) / 2),
								 mTimeSinceLastBalloon(),
								 mBalloonsAddedSoFar(),
								 mBalloonsBurstSoFar(),
								 mBalloonsBurstToLevelUp(balloonsBurstToLevelUp),
								 mScore(),
								 mLevel(1),
								 mWaitTimeDecrease(waitTimeDecrease),
								 mNumColoursInPlay(numColoursInPlay),
								 mBalloonVelocity(balloonVelocity),
								 mBalloonVelocityIncrease(balloonVelocityIncrease)
								 {}


/** @function TBalloonManager::TBalloonManager - Constructor			Takes parameters to construct TBalloonManager with
 *		@param 		balloonScale				Size of the falling balloons fired
 *		@param 		stateVariables				Variables for tracking difficulty and progress
 */
TBalloonManager::TBalloonManager(TReal balloonScale, const TStateVariables& stateVariables) :
							     mBalloonScale(balloonScale),
							     mBalloonRadius(),
							     mBalloons(),
								 mBalloonTextures(),
								 mBalloonBurstTextures(),
								 mVars(stateVariables)
							     {}

/** @function TBalloonManager::AssignAssets - Seperate function to assign image assets to TBalloonManager. AssignAssets is used so 
 *											  that TBalloonManager's constructor can be called by TGame's default constructor, and 
 *											  AssignAssets should then be called in TGame's default constructor.
 *		@param 		balloonTextures				Textures used to display balloons on screen
 *		@param 		balloonBurstTextures		Animated textures for when bullets have been burst
 */
void TBalloonManager::AssignAssets(const std::vector<TTextureRef>& balloonTextures, 
								   const std::vector<TAnimatedTextureRef>& balloonBurstTextures)
{
	mBalloonTextures = balloonTextures;
	mBalloonBurstTextures = balloonBurstTextures;

	mBalloonRadius = uint16_t((balloonTextures[0]->GetWidth() / 2) * mBalloonScale);
}

/** @function TBalloonManager::Reset - This function resets variables in mVars and should be called any time a 
 *									   new game is started.
 *		@param 		mMinWaitForBalloon			Current shortest time to wait for new balloon 
 *		@param		mMaxWaitForBalloon			Current longest time to wait for new balloon
 *		@param 		balloonVelocity				Velocity of balloons
 *		@param 		balloonsBurstToLevelUp		Number of balloons necessary to burst in order to level up
 */
void TBalloonManager::Reset(uint16_t minWaitForBalloon, uint16_t maxWaitForBalloon, const TVec2& balloonVelocity, 
							uint16_t balloonsBurstToLevelUp)
{
	mVars.mMinWaitForBalloon = minWaitForBalloon;
	mVars.mMaxWaitForBalloon = maxWaitForBalloon;
	mVars.mCurrentWaitForBalloon = (minWaitForBalloon + maxWaitForBalloon) / 2;
	mVars.mTimeSinceLastBalloon = 0;
	mVars.mBalloonVelocity = balloonVelocity;
	mVars.mScore = 0;
	mVars.mLevel = 1;
	mVars.mBalloonsBurstToLevelUp = balloonsBurstToLevelUp;
	mVars.mBalloonsBurstSoFar = 0;
	mBalloons.clear();
}

/** @function TBalloonManager::Draw - Draws the falling balloons to the screen 
*/
void TBalloonManager::Draw() const
{
	// Exception could be thrown here if AssignAssets has not been called
	for(list<TBall>::const_iterator balloon = mBalloons.begin(); 
		balloon != mBalloons.end(); ++balloon) 
			balloon->Draw();
}

/** @function TBalloonManager::Update - Calls TBall::Update on all balloons, also introduces more colours according 
 *										to difficulty before checking whether a new balloon should be added, a new
 *										level has been reached, and removing any balloons which are not needed any more.
 *		@param 		elapsedTime			Time in milliseconds since last frame
 */
void TBalloonManager::Update(uint32_t elapsedTime)
{
	for(list<TBall>::iterator balloon = mBalloons.begin(); 
		balloon != mBalloons.end(); ++balloon) 
			balloon->Update(elapsedTime);

	mVars.mTimeSinceLastBalloon += elapsedTime;
	
	// Introduce more colours as difficulty progresses
	if(mVars.mNumColoursInPlay < mBalloonTextures.size()) 
		mVars.mNumColoursInPlay = gameVars::initialNumColoursInPlay + 
								  mVars.mLevel / 
								  gameVars::levelsToPassForNewColour;

	// Check whether a new balloon should be added to game
	AddBalloonCheck();
	IncreaseLevelCheck();
	CleanUpContents();
}

/** @function TBalloonManager::TestForCollisions - This function first tests for a collision between bullets and balloons,
 *												   increasing the score by 1 if one occurrs, and then tests for a collision 
 * 												   between balloons themselves, increasing the score by 2 if one occurrs.
 *		@param 		bullets			Bullets which have been fired by the cannon.
 */
void TBalloonManager::TestForCollisions(std::list<TBall>& bullets)
{
	// First check for collisions between bullets and balloons
	for(list<TBall>::iterator balloon = mBalloons.begin(); balloon != mBalloons.end(); ++balloon)
		for(list<TBall>::iterator bullet = bullets.begin(); bullet != bullets.end(); ++bullet)
			if(bullet->CollisionTest(*balloon)) {
				mVars.mScore += mVars.mLevel;
				mVars.mBalloonsBurstSoFar++;
			}
	
	// Second check for collisions between any balloons which have been sent flying
	for(list<TBall>::iterator balloonOne = mBalloons.begin(); balloonOne != mBalloons.end(); ++balloonOne)
		for(list<TBall>::iterator balloonTwo = mBalloons.begin(); balloonTwo != mBalloons.end(); ++balloonTwo)
			if(balloonOne != balloonTwo && balloonOne->CollisionTest(*balloonTwo)) {
				mVars.mScore += mVars.mLevel * 2;
				mVars.mBalloonsBurstSoFar++;
			}
}

/** @function TBalloonManager::AddBalloonCheck - This function checks whether enough time has passed since the last balloon
 *												 to add a new balloon. If it has, a new balloon is added at the top of the screen
 *												 with a random X value and colour. A new time to wait for the next balloon is then
 *												 computed randomly in the range: mVars.mMinWaitForBalloon to mVars.mMaxWaitForBalloon
 */
void TBalloonManager::AddBalloonCheck()
{
	if(mVars.mTimeSinceLastBalloon >= mVars.mCurrentWaitForBalloon)	{
		mVars.mTimeSinceLastBalloon = 0;
		
		// Determine position and color of new balloon
		TReal xPosition = TReal(mBalloonRadius + TPlatform::GetInstance()->Rand() % (TBall::GetBounds().x2 - mBalloonRadius * 2));
		uint16_t balloonColor = TPlatform::GetInstance()->Rand() % (mVars.mNumColoursInPlay-1);
		
		mBalloons.push_back(TBall(TVec2(xPosition, TReal(TBall::GetBounds().y1 - mBalloonRadius)), mVars.mBalloonVelocity, mBalloonScale, 
							balloonColor, mBalloonTextures[balloonColor], mBalloonBurstTextures[balloonColor], false));
		
		// Get random time to wait for next balloon within range
		mVars.mCurrentWaitForBalloon = mVars.mMinWaitForBalloon + 
									   TPlatform::GetInstance()->Rand() % (mVars.mMaxWaitForBalloon - mVars.mMinWaitForBalloon);
		mVars.mBalloonsAddedSoFar++;
	}
}

/** @function TBalloonManager::IncreaseLevelCheck - This function checks whether enough balloons have been burst in order to level
 *													up, if they have, the game gets harder by adding balloons more frequently,
 *													that fall faster too.
 */
void TBalloonManager::IncreaseLevelCheck()
{
	if(mVars.mBalloonsBurstSoFar >= mVars.mBalloonsBurstToLevelUp * mVars.mLevel)	{
		mVars.mMinWaitForBalloon -= mVars.mWaitTimeDecrease;
		mVars.mMaxWaitForBalloon -= mVars.mWaitTimeDecrease;;
		mVars.mCurrentWaitForBalloon = (mVars.mMinWaitForBalloon + mVars.mMaxWaitForBalloon) / 2;
		mVars.mBalloonVelocity.y += mVars.mBalloonVelocityIncrease;
		mVars.mLevel++;
	}
}

/** @function TBalloonManager::CleanUpContents - Loops through mBalloons and removes any which have mRemoveThisBall set to true,
 *												 which could be due to falling below the lower boundary or finishing bursting.
 */
void TBalloonManager::CleanUpContents()
{
	list<TBall>::iterator balloon = mBalloons.begin();
	while(balloon != mBalloons.end())
	{
		if(balloon->GetRemove()) 
			balloon = mBalloons.erase(balloon);
		 else ++balloon;
	}
}