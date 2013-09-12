/**
 *	ball.h - Jan van der Kamp, 2011
 */
#ifndef BALL_H_INCLUDED
#define BALL_H_INCLUDED
 
#include <pf/pflib.h> 
#include <pf/vec.h>
#include <pf/rect.h>
#include "gameVariables.h"
#include "gameObject.h"

/** @class TBall - This class represents any balls on screen. It can be used for both balloons which are falling and 
 *				    must be burst, and bullets which are fired by the cannon. The bool isBullet signifies which.
 *				   This class inherits from IObject for the Draw/Update interface.
 *		@property 		uint16_t				mColour					The colour of the ball
 *		@property 		TReal					mScale					The scale of the ball
 *		@property 		bool					mIsBullet				Whether the ball is a bullet
 *		@property 		TVec2					mPosition				The position of the ball
 *		@property 		TVec2					mVelocity				The velocity of the ball
 *		@property 		uint16_t				mRadius					The radius of the ball
 *		@property 		bool					mRemoveThisBall			Whether ball should be removed from game
 *		@property 		bool					mBalloonBurst			Whether the balloon has been burst
 *		@property 		static TRect			sBounds					The boundary of the playing area
 *		@property 		TTextureRef				mBalloonTexture			Texture used to display ball on screen
 *		@property 		TAnimatedSpriteRef		mBalloonBurstImage		Animated sprite for when ball has been burst
 */
class TBall : public IObject
{
public:
	TBall(const TVec2& position, const TVec2& velocity, TReal scale, uint16_t colour, 
		  const TTextureRef& balloonTexture, const TAnimatedTextureRef& balloonBurstTexture, bool isBullet);
	virtual	~TBall() {}
	virtual void		Draw() const;
	virtual void		Update(uint32_t elapsedTime);
	bool				CollisionTest(TBall& other);
	uint16_t			GetColour()		const		{ return mColour; }
	uint16_t			GetRadius()		const		{ return mRadius; }
	const TVec2&		GetPosition()	const		{ return mPosition; }
	bool				GetRemove()		const		{ return mRemoveThisBall; }
	void				SetRemoveTrue()				{ mRemoveThisBall = true; }
	void				SetPosition(TVec2 position)	{ mPosition = position; }
	void				SetVelocity(TVec2 velocity)	{ mVelocity = velocity; }
	void				SetToBurst()				{ mBalloonBurst = true; mVelocity = gameVars::burstBalloonVelocity; }
	bool				IsBurst()					{ return mBalloonBurst; }
	bool				IsBullet()					{ return mIsBullet; }
	static void		SetBounds(const TRect& bounds)  { sBounds = bounds; }
	static const	TRect& GetBounds()				{ return sBounds; }
private:
	TBall& operator=(const TBall &ball);
	const uint16_t		mColour;
	const TReal			mScale;
	const bool			mIsBullet;
	TVec2				mPosition;
	TVec2				mVelocity;
	uint16_t			mRadius;
	bool				mRemoveThisBall;
	bool				mBalloonBurst;
	static TRect		sBounds;
	TTextureRef			mBalloonTexture;
	TAnimatedSpriteRef	mBalloonBurstImage;
};

#endif