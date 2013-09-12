/**
 *	game.cpp - Jan van der Kamp, 2011
 */
#include <pf/pflib.h>
#include "pf/debug.h"

#include "game.h"
#include "../settings.h"
#include "../globaldefines.h"

using std::vector;

PFTYPEIMPL_DC(TGame);

/** @function TGame::TGame - Default Constructor	Constructs game objects and other member variables with default values given in
 *													gameVariables.h
 */
TGame::TGame() :
mStateVariables(gameVars::initialMinWaitForBalloon, 
			    gameVars::initialMaxWaitForBalloon,
				gameVars::balloonsBurstToLevelUp,
				gameVars::waitTimeDecrease,
				gameVars::initialBalloonVelocity, 
				gameVars::balloonVelocityIncrease,
				gameVars::initialNumColoursInPlay),
mBalloonManager(gameVars::balloonScale,
				mStateVariables),
 mCannon(gameVars::cannonPosition,
		 gameVars::bulletScale,
		 gameVars::initialNumColoursInPlay),
mBarrier(gameVars::initialBarrierPosition, 
		 gameVars::barrierRiseSpeed,	
		 gameVars::initialBarrierParallaxDifference, 
		 gameVars::barrierLevelHeight),
mToUpdate(),
mToDraw(),
mMessageText("",gameVars::messageW,gameVars::messageH, TTextGraphic::EFlags::kHAlignCenter,"fonts/DomCasualStd-Bold.mvec",gameVars::textSize),
mInfoButton("",gameVars::messageW,gameVars::messageH*2, TTextGraphic::EFlags::kHAlignCenter,"fonts/DomCasualStd-Bold.mvec",gameVars::textSize),
mBasicButton("",gameVars::messageW,gameVars::messageH, TTextGraphic::EFlags::kHAlignCenter,"fonts/DomCasualStd-Bold.mvec",gameVars::textSize),
mPausedButton("",gameVars::messageW,gameVars::messageH, TTextGraphic::EFlags::kHAlignCenter,"fonts/DomCasualStd-Bold.mvec",gameVars::textSize),
mHelpTextButton("", gameVars::helpTextW, gameVars::helpTextH, TTextGraphic::EFlags::kHAlignCenter,"fonts/DomCasualStd-Bold.mvec",gameVars::textSize),
mHelpTextStr1(""), mHelpTextStr2(""), mPauseButtonStr(""), mUnpauseButtonStr(""), mHelpButtonStr(""), mNewGameButtonStr(""), mQuitButtonStr(""), mScoreStr(""), 
mLevelStr(""), mGameOverStr(""), mPausedStr(""), mGameState(HELP), mBalloonTextures(), mBalloonBurstTextures(), mBarrierTextures(), mCannonTexture(), mHudBackground(), 
mLastLoopTime()
{
	TPlatform::SetConfig( "vsync", "1" );
	TTextGraphic::SetBoldOverride("fonts/DomCasualStd-Bold.mvec", true, .1f);

	// Load strings and set text
	LoadStrings();
	mInfoButton.SetText(mScoreStr+": 0\n"+mLevelStr+": 1");
	mHelpTextButton.SetText(mHelpTextStr1);

	// Load and assign assets to game objects
	LoadAssets();
	mBalloonManager.AssignAssets(mBalloonTextures, mBalloonBurstTextures);
	mCannon.AssignAssets(mCannonTexture, mBalloonTextures, mBalloonBurstTextures);
	mBarrier.AssignAssets(mBarrierTextures);

	mToUpdate.push_back(&mCannon);
	mToUpdate.push_back(&mBarrier);
	mToUpdate.push_back(&mBalloonManager);
	
	mToDraw.push_back(&mCannon);
	mToDraw.push_back(&mBalloonManager);

	// height that barrier will reach when game over occurs. 
	// This should coincide with the cannons loaded bullet becoming covered by the barrier.
	TReal gameOverHeight = mCannon.GetPosition().y - mCannon.GetBulletRadius();
	mBarrier.SetGameOverHeight(gameOverHeight);

	// Set game boundary
	TBall::SetBounds(TURect(0, gameVars::hudBoundary, SCREEN_WIDTH, SCREEN_HEIGHT));

	mLastLoopTime = TPlatform::GetInstance()->GetTime();
}


/** @function TGame::Init - This function initializes the window and is called by the system only in Lua initialization
 *		@param 		style				Style of the window
 */
void TGame::Init(TWindowStyle&  style)
{
	pfparent::Init(style);	
	
	FindParentModal()->SetDefaultFocus(this);
	//FindParentModal()->SetWindowSize(800,600);

	StartWindowAnimation( 16 ); 
}

/** @function TGame::LoadAssets - This function loads any image assets used in the game
 */
void TGame::LoadAssets()
{
	mBalloonTextures.push_back(TTexture::Get("images/balloon1"));
	mBalloonTextures.push_back(TTexture::Get("images/balloon2"));
	mBalloonTextures.push_back(TTexture::Get("images/balloon3"));
	mBalloonTextures.push_back(TTexture::Get("images/balloon4"));
	mBalloonTextures.push_back(TTexture::Get("images/balloon5"));
	mBalloonTextures.push_back(TTexture::Get("images/balloon6"));
	
	mBarrierTextures.push_back(TTexture::Get("images/barrier1"));
	mBarrierTextures.push_back(TTexture::Get("images/barrier2"));
	mBarrierTextures.push_back(TTexture::Get("images/barrier3"));
	mBarrierTextures.push_back(TTexture::Get("images/barrier4"));
	mBarrierTextures.push_back(TTexture::Get("images/barrier5"));
	mBarrierTextures.push_back(TTexture::Get("images/barrier6"));

	mBalloonBurstTextures.push_back(TAnimatedTexture::Get("anim/balloon-burst1.xml"));
	mBalloonBurstTextures.push_back(TAnimatedTexture::Get("anim/balloon-burst2.xml"));
	mBalloonBurstTextures.push_back(TAnimatedTexture::Get("anim/balloon-burst3.xml"));
	mBalloonBurstTextures.push_back(TAnimatedTexture::Get("anim/balloon-burst4.xml"));
	mBalloonBurstTextures.push_back(TAnimatedTexture::Get("anim/balloon-burst5.xml"));
	mBalloonBurstTextures.push_back(TAnimatedTexture::Get("anim/balloon-burst6.xml"));

	mCannonTexture = TTexture::Get("images/arrow");
	mHudBackground = TSprite::Create(0, TTexture::Get("images/hudBackground"));
	mHudBackground->GetDrawSpec() = TDrawSpec(TVec2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2));

	mBasicButton.SetImage(TSprite::Create(0, TTexture::Get("images/button")));
	mPausedButton.SetImage(TSprite::Create(0, TTexture::Get("images/pausedButton")));
	mHelpTextButton.SetImage(TSprite::Create(0, TTexture::Get("images/helpTextBox")));
	mInfoButton.SetImage(TSprite::Create(0, TTexture::Get("images/infoBG")));
}

/** @function TGame::LoadStrings - This function loads strings from the global string table,
 *								   which are defined in strings.xml
 */
void TGame::LoadStrings()
{
	TPlatform * platform = TPlatform::GetInstance();
	TStringTable * stringTable = platform->GetStringTable();

	// Get strings from string table
	mHelpTextStr1 = str(stringTable->GetString("helpString1")+"<br>"+
						 stringTable->GetString("helpString2")+"<br>"+"<br>"+
						 stringTable->GetString("helpString3"));
	mHelpTextStr2 = str(stringTable->GetString("helpString4")+"<br>"+"<br>"+
						 stringTable->GetString("helpString5")+"<br>"+"<br>"+
						 stringTable->GetString("helpString6"));
	mPauseButtonStr = stringTable->GetString("pause");
	mUnpauseButtonStr = stringTable->GetString("unpause");
	mNewGameButtonStr = stringTable->GetString("newGame");
	mHelpButtonStr = stringTable->GetString("help");
	mQuitButtonStr = stringTable->GetString("quit");
	mScoreStr = stringTable->GetString("score");
	mLevelStr = stringTable->GetString("level");
	mGameOverStr = stringTable->GetString("gameOver");
	mPausedStr = stringTable->GetString("paused");
}

/** @function TGame::Reset - This function resets mBalloonManager & mBarrier to default values,
 *							 and should be called when a new game is started.
 */
void TGame::Reset()
{	
	mBalloonManager.Reset(gameVars::initialMinWaitForBalloon, 
						  gameVars::initialMaxWaitForBalloon, 
						  gameVars::initialBalloonVelocity, 						 
						  gameVars::balloonsBurstToLevelUp);
	mBarrier.Reset();
}

/** @function TGame::OnTaskAnimate - Since StartWindowAnimation was called with a value of 16, this function
 *									 will be called every 16 milliseconds to give a frame rate of around 60 fps.
 */
bool TGame::OnTaskAnimate()
{
// Update the screen
	TWindowManager::GetInstance()->InvalidateScreen();

// Calculate elapsed time since the last time we were called
	uint32_t thisLoop = TPlatform::GetInstance()->GetTime();
	uint32_t elapsedTime = thisLoop - mLastLoopTime;
	mLastLoopTime = thisLoop;

	Update( elapsedTime );

	return true;
}

/** @function TGame::Update - Used to update the game objects, test for collisions between balls, and for balloons
 *							  sinking below lower boundary. This function also updates mInfoButton based on current 
 *							  score & level, and increases the amount of colours with difficulty
 *		@param 		elapsedTime			Time in milliseconds since last frame
 */
void TGame::Update( uint32_t elapsedTime )
{	
	if(mGameState==UNPAUSED)
	{
		// Update game objects
		for(vector<IObject*>::iterator iter = mToUpdate.begin(); iter != mToUpdate.end(); ++iter)
			(*iter)->Update(elapsedTime);
				
		mBalloonManager.TestForCollisions(mCannon.GetBulletsFired());

		// Check for balloons which are sinking
		if(mBarrier.TestForSinkingBalloons(mBalloonManager.GetBalloons()) ||
		   mBarrier.TestForSinkingBalloons(mCannon.GetBulletsFired())) {
			mMessageText.SetText(mGameOverStr);
			mGameState = GAMEOVER;
		}
		// Update the games boundary
		TBall::SetBounds(TRect(0, gameVars::hudBoundary, SCREEN_WIDTH, int32_t(mBarrier.GetPosition().y)));

		// Update info text
		str gameInfo = AppendIntToStr(mBalloonManager.GetScore(), mScoreStr+": ") + '\n';
		gameInfo += AppendIntToStr(mBalloonManager.GetLevel(), mLevelStr+": ");
		mInfoButton.SetText(gameInfo);

		// Increase colours with difficulty
		mCannon.SetNumColours(gameVars::initialNumColoursInPlay + 
							  mBalloonManager.GetLevel() / 
							  gameVars::levelsToPassForNewColour);
	}
}

/** @function TGame::AppendIntToStr - This function converts an int to a str and appends it to the end of a str.
 *		@param 		number				Number to convert
 *		@param 		textToAppendTo		Text to append the number to
 *
 *		@return		textToAppendTo		textToAppendTo with number at the end
 */
str TGame::AppendIntToStr(int32_t number, str textToAppendTo)
{
	str intAsStr = ConvertIntToStr(number);
	textToAppendTo += intAsStr;
	return textToAppendTo;
}

/** @function TGame::ConvertIntToStr - This function converts an int to a str and returns it
 *		@param 		numerator				Number to convert
 *
 *		@return		intAsStr			numerator as str
 */
str TGame::ConvertIntToStr(int32_t numerator)
{
	vector<char> scoreChars;
	str intAsStr;

	while(numerator >= 10) {
		uint32_t temp = numerator / 10;
		uint32_t remainder = numerator % 10;
		numerator = temp;
		scoreChars.push_back(char(remainder + 48));
	}
	scoreChars.push_back(char(numerator + 48));

	for(vector<char>::reverse_iterator rit = scoreChars.rbegin(); rit < scoreChars.rend(); ++rit)
		intAsStr += *rit;
	
	return intAsStr;
}

/** @function TGame::Draw - This function draws all game objects and buttons to the screen. It uses a switch statement
 *							to handle the different states that the game may be in. In the UNPAUSD state, it first draws
 *							the backgound section of mBarrier, before drawing the cannon and balloon manager. It then
 *							draws the foreground section of mBarrier.
 */
void TGame::Draw()
{
	TBegin2d draw;

	// First fill with background colour
	TRenderer * r = TRenderer::GetInstance();
	r->FillRect( TURect(0,0,SCREEN_WIDTH, SCREEN_HEIGHT), gameVars::backgroundColour );

	switch(mGameState)
	{
	case UNPAUSED :
		// Draw the background section of mBarrier
		mBarrier.DrawBackground();

		// Draw other game objects
		for(vector<IObject*>::iterator iter = mToDraw.begin(); iter != mToDraw.end(); ++iter)
			(*iter)->Draw();

		// Draw the foreground section of mBarrier
		mBarrier.DrawForeground();

		mHudBackground->Draw();
		mBasicButton.Draw(gameVars::pauseButtonPosition, mPauseButtonStr);
		mBasicButton.Draw(gameVars::newGameButtonPosition, mNewGameButtonStr);
		mBasicButton.Draw(gameVars::helpButtonPosition, mHelpButtonStr);
		mBasicButton.Draw(gameVars::quitButtonPosition, mQuitButtonStr);
		mInfoButton.Draw(gameVars::gameInfoPosition);
		break;
	case PAUSED :
		mHudBackground->Draw();
		mPausedButton.Draw(gameVars::pauseButtonPosition, mUnpauseButtonStr);
		mBasicButton.Draw(gameVars::newGameButtonPosition, mNewGameButtonStr);
		mBasicButton.Draw(gameVars::helpButtonPosition, mHelpButtonStr);
		mBasicButton.Draw(gameVars::quitButtonPosition, mQuitButtonStr);
		mMessageText.Draw(gameVars::pauseMSGPosition, gameVars::messageH*2);
		mInfoButton.Draw(gameVars::gameInfoPosition);
		break;
	case HELP :
		mHudBackground->Draw();
		mHelpTextButton.Draw(gameVars::helpMSGPosition);
		mInfoButton.Draw(gameVars::gameInfoPosition);
		break;
	case GAMEOVER :
		mBarrier.Draw();
		mHudBackground->Draw();
		mBasicButton.Draw(gameVars::newGameButtonPosition, mNewGameButtonStr);
		mBasicButton.Draw(gameVars::helpButtonPosition, mHelpButtonStr);
		mBasicButton.Draw(gameVars::quitButtonPosition, mQuitButtonStr);
		mInfoButton.Draw(gameVars::gameInfoPosition);
		mMessageText.Draw(gameVars::gameOverMSGPosition, gameVars::messageH*2);
		break;
	}
}

/** @function TGame::OnMouseDown - This function is called when the user clicks the left mouse button.
 *								   It then checks to see if the cursor was above any buttons when the mouse click happened.
 *								   If it wasn't, then mCannon loads a bullet to the end of itself.
 *		@param 		p					Mouse cursor position when mouse down event occurred
 *
 *		@return		true if event was handled in this function, false to keep searching for handlers
 */
bool TGame::OnMouseDown(const TPoint& p)
{	

	if(mGameState == PAUSED && mPausedButton.HitTest(p, gameVars::pauseButtonPosition)) {
		mGameState = UNPAUSED;
		return true;  
	}
	else if(mGameState == UNPAUSED && mBasicButton.HitTest(p, gameVars::pauseButtonPosition)) {
		mMessageText.SetText(mPausedStr);
		mGameState = PAUSED;
		return true;
	}
	else if(mBasicButton.HitTest(p, gameVars::newGameButtonPosition)) {
		Reset();
		mGameState = UNPAUSED;
		return true;
	}
	else if(mBasicButton.HitTest(p, gameVars::helpButtonPosition)) {
		mGameState = HELP;
		return true;
	}
	else if(mGameState == HELP && mHelpTextButton.GetText() != mHelpTextStr2 && mHelpTextButton.HitTest(p, gameVars::helpMSGPosition)) {
		mHelpTextButton.SetText(mHelpTextStr2);
		return true;
	}
	else if(mGameState == HELP && mHelpTextButton.GetText() != mHelpTextStr1 && mHelpTextButton.HitTest(p, gameVars::helpMSGPosition)) {
		mGameState = UNPAUSED;
		mHelpTextButton.SetText(mHelpTextStr1);
		return true;
	}
	else if(mGameState != HELP && mBasicButton.HitTest(p, gameVars::quitButtonPosition)) {
		TWindowManager::GetInstance()->GetScript()->RunScript("scripts/quitverify.lua");
		return true;
	}
	else if(mGameState == UNPAUSED && p.y > gameVars::hudBoundary) {
		mCannon.Reload();
		return true;
	}
	return false;
}

/** @function TGame::OnMouseUp - This function is called when the user releases the left mouse button, and tells
 *								 mCannon to fire a bullet.
 *		@param 		p					Mouse cursor position when mouse up event occurred
 *
 *		@return		true if event was handled in this function, false to keep searching for handlers
 */
bool TGame::OnMouseUp(const TPoint& p)
{	
	mCannon.Fire();
	return true;
}

/** @function TGame::OnMouseMove - This function is called when the user moves the mouse and is 
 *								   used to update the direction of mCannon
 *		@param 		p					Mouse cursor position when mouse up event occurred
 *
 *		@return		true if event was handled in this function, false to keep searching for handlers
 */
bool TGame::OnMouseMove(const TPoint& p)
{	
	mCannon.UpdateMousePosition(p);

	return true;
}
