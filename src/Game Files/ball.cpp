/**
 *	ball.cpp - Jan van der Kamp, 2011
 */
#include "ball.h"

/** @function TBall::TBall - Constructor			Takes parameters to construct ball with
 *		@param 		position				The position of the ball
 *		@param 		velocity				The velocity of the ball
 *		@param 		scale					The scale of the ball
 *		@param 		colour					The colour of the ball
 *		@param 		balloonTexture			Texture used to display ball on screen
 *		@param 		balloonBurstTexture		Animated sprite for when ball has been burst
 *		@param 		isBullet				Whether the ball is a bullet
 */
TBall::TBall(const TVec2& position, const TVec2& velocity, TReal scale, uint16_t colour, 
		     const TTextureRef& balloonTexture, const TAnimatedTextureRef& balloonBurstTexture, bool isBullet) :
mColour(colour),
mScale(scale), 
mIsBullet(isBullet),
mPosition(position), 
mVelocity(velocity), 
mRadius(), 
mRemoveThisBall(false),
mBalloonBurst(false),
mBalloonTexture(balloonTexture), 
mBalloonBurstImage()
{
	mBalloonBurstImage = TAnimatedSprite::Create(0);
	mBalloonBurstImage->SetTexture(balloonBurstTexture);

	mRadius = uint16_t(balloonTexture->GetWidth() * mScale / 2);
}

/** @function TBall::CollisionTest - Tests for a collision with another ball and modifies each accordingly.
 *									 If they are both balloons, they will burst regardless of colour. If one is 
 *									 a bullet they will burst if they are the same colour, otherwise they will
 *									 bounce off each other. Returns true if they both burst.
 *
 *		@param 		other			 Another ball to test against for a collision
 *
 *		@return		true if balloon and not bullet burst
 */
bool TBall::CollisionTest(TBall& other)
{
	if(mBalloonBurst || other.IsBurst())
		return false;

		// Collision has occurred between *this and other
		if((mPosition - other.GetPosition()).Length() <= mRadius + other.GetRadius()) 
		{
			if((mIsBullet && !other.IsBullet()) && mColour != other.GetColour()) 
			{
				//SetToBurst(); 
				mBalloonBurst = true;
				mVelocity = gameVars::burstBalloonVelocity;
				other.SetVelocity((other.GetPosition() - mPosition).Normalize());
			} 
			else if((!mIsBullet && other.IsBullet()) && mColour != other.GetColour()) 
			{
				other.SetToBurst();
				mVelocity = (mPosition - other.GetPosition()).Normalize();
			} 
			else 
			{
				this->SetToBurst();
				other.SetToBurst();
				return true;
			}
		}	
	return false;
}

/** @function TBall::Draw - Draws the ball to the screen. If mBalloonBurst is true, the burst animation
 *							is played, otherwise the static image is shown.
 */
void TBall::Draw() const
{
	if(mBalloonBurst) 
	{
		if(!mBalloonBurstImage->IsPlaying())
			mBalloonBurstImage->Play();

		TDrawSpec drawSpec(mPosition, 1.f, mScale);
		mBalloonBurstImage->Draw(drawSpec);		
	} else 
		mBalloonTexture->DrawSprite(mPosition.x, mPosition.y, 1.f, mScale);
}

/** @function TBall::Update - Updates the position of the ball and alters its velocity if has
 *							  bounced off the side of the screen. If mIsBullet is true then 
 *							  it won't bounce off the top of the screen.
 *		@param 		elapsedTime			Time in milliseconds since last frame
 */
void TBall::Update(uint32_t elapsedTime)
{
	mPosition += mVelocity * TReal(elapsedTime);

	// multiply mRadius by 2 to ensure ball is not visible behind barriers before removing
 	if(mPosition.y > sBounds.y2 + mRadius * 2 ||
	   mPosition.y < -(int32_t)mRadius)	
		mRemoveThisBall = true;

	if(mPosition.x < mRadius && mVelocity.x < 0.f) 
		mVelocity.x *= -1.f;
	else if(mPosition.x > sBounds.x2 - mRadius && mVelocity.x > 0.f) 
			 mVelocity.x *= -1.f;
	else if(mPosition.y < sBounds.y1 + mRadius && mVelocity.y < 0.f) 
			 mVelocity.y *= -1.f;

	if(mBalloonBurstImage && mBalloonBurstImage->IsPlaying() &&
	   uint32_t(mBalloonBurstImage->GetCurrentFrame()) == mBalloonBurstImage->GetNumFrames()-1) {
			mBalloonBurstImage->Stop();
			mRemoveThisBall = true;
		}
}

// Initialize static member variable TBall::sBounds
TRect TBall::sBounds = TRect();