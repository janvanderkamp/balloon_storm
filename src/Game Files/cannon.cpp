/**
 *	cannon.cpp - Jan van der Kamp, 2011
 */
#include "cannon.h"

using std::vector;
using std::list;

/** @function TCannon::TCannon - Constructor			Takes parameters to construct cannon with
 *		@param 		position				Position of the base of the cannon
 *		@param 		bulletScale				Size of the bullets fired
 *		@param 		numColoursInPlay		Range of colours that bullets can be
 */
TCannon::TCannon(const TVec2& position, const TReal& bulletScale, uint16_t numColoursInPlay) :
mDirectionAtRest(0.f, -1.f),
mBulletScale(bulletScale),
mNumColoursInPlay(numColoursInPlay),
mPosition(position),
mLoadedBulletPosition(),
mDrawSpec(),
mAngle(0),
mBullets(),
mBulletsFired(),
mCannonTexture(),
mBalloonTextures(),
mBalloonBurstTextures()
{}

/** @function TCannon::AssignAssets - Seperate function to assign image assets to TCannon. AssignAssets is used so that TCannon's 
 *									  constructor can be called by TGame's default constructor, and AssignAssets should then be called
 *									  in TGame's default constructor.
 *		@param 		cannonTexture				Texture used to display cannon on screen
 *		@param 		balloonTextures				Textures used to display bullets on screen
 *		@param 		balloonBurstTextures		Animated textures for when bullets have been burst
 */
void TCannon::AssignAssets(const TTextureRef& cannonTexture, const std::vector<TTextureRef>& balloonTextures, 
						   const std::vector<TAnimatedTextureRef>& balloonBurstTextures)
{
	mCannonTexture = cannonTexture;
	mBalloonTextures = balloonTextures;
	mBalloonBurstTextures = balloonBurstTextures;
	
	mDrawSpec.mCenter = TVec2(TReal(cannonTexture->GetWidth()/2), TReal(cannonTexture->GetHeight()));
	mDrawSpec.mFlags = 1<<3;

	uint16_t firstBulletColour =  TPlatform::GetInstance()->Rand() % (mNumColoursInPlay-1);
	mBullets.push_back(TBall(mPosition, TVec2(), mBulletScale, firstBulletColour, 
							 mBalloonTextures[firstBulletColour], mBalloonBurstTextures[firstBulletColour], true));
}

/** @function TCannon::Draw - Draws the cannon and both it's loaded bullets and fired bullets to the screen. 
*/
void TCannon::Draw() const
{
	// Exception could be thrown here if AssignAssets has not been called

	mCannonTexture->DrawSprite(mDrawSpec);
	for(list<TBall>::const_iterator it = mBullets.begin(); it != mBullets.end(); ++it)
		it->Draw();
	for(list<TBall>::const_iterator it = mBulletsFired.begin(); it != mBulletsFired.end(); ++it)
		it->Draw();
}

/** @function TCannon::Update - Calls TBall::Update on any bullets and makes sure loaded
 *								bullet is at end of cannon
 *		@param 		elapsedTime			Time in milliseconds since last frame
 */
void TCannon::Update(uint32_t elapsedTime)
{
	// Update bullets which have been fired
	for(list<TBall>::iterator bullet = mBulletsFired.begin(); 
		bullet != mBulletsFired.end(); ++bullet) 
			bullet->Update(elapsedTime);
	
	// If user has reloaded, place first bullet at end of cannon
	if(mBullets.size() > 1) 
		mBullets.begin()->SetPosition(mPosition + 
									  mLoadedBulletPosition * 
									  TReal(mCannonTexture->GetHeight() + 
									  mBullets.begin()->GetRadius()));

	CleanUpContents();
}

/** @function TCannon::UpdateMousePosition - Updates the angle of the cannon based on the position
 *											 of the mouse cursor
 */
void TCannon::UpdateMousePosition(const TPoint& p)
{
	// Get angle of line from cannon base to p
	TVec2 mouseDirection(TReal(p.x) - mPosition.x, 
						 TReal(p.y) - mPosition.y);
	mouseDirection.Normalize();
	mAngle = acos(DotProduct(mouseDirection, mDirectionAtRest));
	
	if(p.x > mPosition.x)	
		mAngle *= -1.f;

	mLoadedBulletPosition = mouseDirection;

	// Modify mDrawSpec accordingly
	mDrawSpec.mMatrix = TMat3(cosf(mAngle), -sinf(mAngle), 0, 
							  sinf(mAngle), cosf(mAngle), 0, 
							  mPosition.x, mPosition.y, 1);
}

/** @function TCannon::Reload - Adds another bullet to mBullets. 
*/
void TCannon::Reload()
{
	uint16_t color =  TPlatform::GetInstance()->Rand() % (mNumColoursInPlay-1);
	mBullets.push_back(TBall(mPosition, TVec2(), mBulletScale, color, mBalloonTextures[color], mBalloonBurstTextures[color], true));
}

/** @function TCannon::Fire - Fires a bullet by moving one from mbullets to mBulletsFired and setting it's velocity
 *							  to the direction pointed by the cannon. Only fires a bullet if mBullets.size() >= 2 since
 *							  without this check bullets could be fired when user releases mouse button on a menu item
 */
void TCannon::Fire()
{
	// Avoid firing without first reloading
	if(mBullets.size() >= 2) {
		mBullets.begin()->SetVelocity(mLoadedBulletPosition);
		mBulletsFired.push_back(*mBullets.begin());
		mBullets.pop_front();
	}
}

/** @function TCannon::SetNumColours - Sets the amount of colours to choose from. This should increase with the players level
 *									   and should not be more than the number of ball images
 */
void TCannon::SetNumColours(uint16_t numColours) 
{ 
	if(mNumColoursInPlay < mBalloonTextures.size()) mNumColoursInPlay = numColours; 
}

/** @function TCannon::CleanUpContents - Loops through mBulletsFired and removes any which have mRemoveThisBall set to true,
 *										 which could be due to flying off screen or finishing bursting.
 */
void TCannon::CleanUpContents()
{
	list<TBall>::iterator bullet = mBulletsFired.begin();
	while(bullet != mBulletsFired.end())
	{
		if(bullet->GetRemove()) 
			bullet = mBulletsFired.erase(bullet);
		 else ++bullet;
	}
}
