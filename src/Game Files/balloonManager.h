/**
 *	balloonManager.h - Jan van der Kamp, 2011
 */
#ifndef BALLOONMANAGER_H_INCLUDED
#define BALLOONMANAGER_H_INCLUDED

#include <pf/pflib.h>
#include <pf/vec.h>
#include <pf/rect.h>
#include <vector>
#include <list>

#include "gameVariables.h"
#include "ball.h"

/** @struct TStateVariables - This struct contains variables for keeping track of game difficulty and player progress
 *
 *	@property 	uint16_t		mMinWaitForBalloon			Current shortest time to wait for new balloon 
 *  @property 	uint16_t		mMaxWaitForBalloon			Current longest time to wait for new balloon
 *	@property 	uint16_t		mCurrentWaitForBalloon		Actual time to wait for new balloon (random in range of min-max)
 *  @property 	uint32_t		mTimeSinceLastBalloon		Time since last balloon added to scren
 *	@property 	uint16_t		mBalloonsAddedSoFar			Number of balloons added since start
 *  @property 	uint16_t		mBalloonsBurstSoFar			Number of balloons added since start
 *	@property 	uint16_t		mBalloonsBurstToLevelUp		Number of balloons necessary to burst in order to level up
 *	@property 	uint16_t		mScore						Current score
 *  @property 	uint16_t		mLevel						Current level
 *	@property 	uint16_t		mWaitTimeDecrease			Time in milliseconds that mMinWaitForBalloon & mMaxWaitForBalloon 
 *  														decrease by when new level is reached
 *	@property 	uint16_t		mNumColoursInPlay			Range of colours that bullets can be
 *  @property 	TVec2			mBalloonVelocity			Velocity of balloon
 *	@property 	TReal			mBalloonVelocityIncrease	Amount that mBalloonVelocity.y increases by when new level is reached
 */
struct TStateVariables
{
	TStateVariables(uint16_t minWaitForBalloon, uint16_t maxWaitForBalloon, uint16_t balloonsBurstToLevelUp, 
					uint16_t waitTimeDecrease, const TVec2& balloonVelocity, TReal balloonVelocityIncrease,
					uint16_t numColoursInPlay);
	uint16_t	mMinWaitForBalloon;
	uint16_t	mMaxWaitForBalloon;
	uint16_t	mCurrentWaitForBalloon;
	uint32_t	mTimeSinceLastBalloon;
	uint16_t	mBalloonsAddedSoFar;
	uint16_t	mBalloonsBurstSoFar;
	uint16_t	mBalloonsBurstToLevelUp;
	uint16_t	mScore;
	uint16_t	mLevel;
	uint16_t	mWaitTimeDecrease;
	uint16_t	mNumColoursInPlay;
	TVec2		mBalloonVelocity;
	TReal		mBalloonVelocityIncrease;
};


/** @class TBalloonManager - This class manages the balloons which are falling and keeps track of the difficulty of the game.
 *  							 When the player moves up a level the balloons fall faster and more frequently. Variables relevant 
 *								 to game difficulty are stored in an instance of TStateVariables.
 *  							 This class inherits from IObject for the Draw/Update interface.
 *	@property 	TReal								mBalloonScale			The scale of the falling balloons 
 *  @property 	uint16_t							mBalloonRadius			The radius of the falling balloons
 *	@property 	std::list<TBall>					mBalloons				Falling balloons which must be burst
 *	@property 	std::vector<TTextureRef>			mBalloonTextures		Textures used to display balloons on screen
 *  @property 	std::vector<TAnimatedTextureRef>	mBalloonBurstTextures	Textures used for burst balloon animation
 *	@property 	TStateVariables						mVars					Variables to keep track of game difficulty
*/
class TBalloonManager : public IObject
{
public:
	TBalloonManager(TReal balloonScale, const TStateVariables& stateVariables);
	virtual ~TBalloonManager() {}
	void AssignAssets(const std::vector<TTextureRef>& balloonTextures, const std::vector<TAnimatedTextureRef>& balloonBurstTextures);
	void Reset(uint16_t minWaitForBalloon, uint16_t maxWaitForBalloon, const TVec2& balloonVelocity, 
			   uint16_t balloonsBurstToLevelUp);
	virtual void				Draw()			const;
	virtual void				Update(uint32_t elapsedTime);
	void						TestForCollisions(std::list<TBall>& bullets);
	const std::list<TBall>&		GetBalloons()	const	{ return mBalloons; }
	uint16_t					GetScore()		const	{ return mVars.mScore; }
	uint16_t					GetLevel()		const	{ return mVars.mLevel; }
private: 
	// copying disallowed
	TBalloonManager(const TBalloonManager &balloonManager);
	TBalloonManager& operator=(const TBalloonManager &balloonManager);
	void						AddBalloonCheck();
	void						IncreaseLevelCheck();
	void						CleanUpContents();

	const TReal								mBalloonScale;
	uint16_t								mBalloonRadius;
	std::list<TBall>						mBalloons;
	std::vector<TTextureRef>				mBalloonTextures;
	std::vector<TAnimatedTextureRef>		mBalloonBurstTextures;
	TStateVariables							mVars;
};


#endif // BALLOONMANAGER_H_INCLUDED