/**
 *	game.h - Jan van der Kamp, 2011
 */

#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED  

#include "gameVariables.h" 
#include "ball.h"
#include "balloonManager.h"
#include "cannon.h"
#include "barrier.h"
#include "basicButton.h"

/** @class TGame - This class inherits from TWindow and is used to display the game objects to the screen and update them.
 *				   It also manages a simple state machine for the UNPAUSED, PAUSED, HELP, and GAMEOVER states. It listens
 *				   for mousedown and mousemove events, and if transitions between states based on the user clicking on the
 *				   relevant buttons, which are shown on screen using TSpriteRefs
 *	@property 	TStateVariables						mStateVariables			Variables to keep track of game difficulty/progress, used to initialize 
 *																			mBalloonManager
 * 	@property 	TBalloonManager						mBalloonManager			Manages falling balloons on screen and keeps track of game difficulty/progress
 *	@property 	TCannon								mCannon					Used to shoot bullets at balloons
 *	@property 	TBarrier							mBarrier				If balloons fall to far, this rises until too high and game over is reached
 *	@property 	std::vector<IObject*>				mToUpdate				Used to update mBalloonManager, mCannon, and mBarrier polymorphically
 *	@property 	std::vector<IObject*>				mToDraw					Used to draw mBalloonManager, mCannon, polymorphically, mBarrier needs
 *																			to be drawn using separate background and foreground draw functions
 *	@property 	TTextGraphic						mMessageText			Used to display various info to the screen
 * 	@property 	TBasicButton						mInfoButton				Used to display on screen info on the score and level of the game
 * 	@property 	TBasicButton						mBasicButton			Used to draw the Pause, Quit, Help and New Game buttons
 * 	@property 	TBasicButton						mPausedButton			Used to draw the Unpause button
 *	@property 	TBasicButton						mHelpTextButton			Used to display info on how to play the game to the screen
 *	@property 	str									mHelpTextStr1			str containing info for the first page of instructions, loaded from strings.xml
 *	@property 	str									mHelpTextStr2			str containing info for the second page of instructions, loaded from strings.xml
 *	@property 	str									mPauseButtonStr			str containing the title of the Pause button, loaded from strings.xml
 *	@property 	str									mUnpauseButtonStr		str containing the title of the Unpause button, loaded from strings.xml
 *	@property 	str									mHelpButtonStr			str containing the title of the Help button, loaded from strings.xml
 *	@property 	str									mNewGameButtonStr		str containing the title of the New Game button, loaded from strings.xml
 *	@property 	str									mQuitButtonStr			str containing the title of the Quit button, loaded from strings.xml
 *	@property 	str									mScoreStr				str containing the word SCORE, loaded from strings.xml
 *	@property 	str									mLevelStr				str containing the word LEVEL, loaded from strings.xml
 *	@property 	str									mGameOverStr			str containing info for the Game Over message, loaded from strings.xml
 *	@property 	str									mPausedStr				str containing info for the Paused message, loaded from strings.xml
 *	@property 	uint16_t							mGameState				The current state the game is in
 *	@property 	std::vector<TTextureRef>			mBalloonTextures		Textures used to display balloons on screen
 *	@property 	std::vector<TAnimatedTextureRef>	mBalloonBurstTextures	Textures used for burst balloon animation
 * 	@property 	std::vector<TTextureRef>			mBarrierTextures		Textures to represent piles of balloons at the bottom of the screen
 *	@property 	TTextureRef							mCannonTexture			Texture used to display cannon on screen
 *	@property 	TSpriteRef							mHudBackground			Background for the HUD display
 *	@property 	uint32_t							mLastLoopTime			Time since last frame
 */
class TGame : public TWindow
{
	PFTYPEDEF_DC(TGame,TWindow)
public:
	TGame();
	~TGame() {}

	void Draw();

	virtual bool OnMouseDown(const TPoint& point);
	virtual bool OnMouseUp(const TPoint& point);
	virtual bool OnMouseMove(const TPoint& point);
	virtual void Init(TWindowStyle& style);
	virtual bool OnTaskAnimate();

	// game states
	enum {
		UNPAUSED = 0,
		PAUSED,
		HELP,
		GAMEOVER
	};
	
private:
	void Update( uint32_t elapsedTime );
	void LoadAssets();
	void LoadStrings();
	void Reset();
	
	str ConvertIntToStr(int32_t);
	str AppendIntToStr(int32_t number, str textToAppendTo = "");
	
	// Game objects
	TStateVariables mStateVariables;
	TBalloonManager mBalloonManager;
	TCannon mCannon;
	TBarrier mBarrier;
	std::vector<IObject*> mToUpdate;
	std::vector<IObject*> mToDraw;

	// Text Graphics
	TTextGraphic mMessageText;

	// Buttons
	TBasicButton mInfoButton;
	TBasicButton mBasicButton;
	TBasicButton mPausedButton;
	TBasicButton mHelpTextButton;

	// Strings
	str mHelpTextStr1, mHelpTextStr2, mPauseButtonStr, mUnpauseButtonStr, mHelpButtonStr;
	str mNewGameButtonStr, mQuitButtonStr, mScoreStr, mLevelStr, mGameOverStr, mPausedStr;

	// Game State
	uint16_t mGameState;

	// Assets
	std::vector<TTextureRef> mBalloonTextures;
	std::vector<TAnimatedTextureRef> mBalloonBurstTextures;
	std::vector<TTextureRef> mBarrierTextures;
	TTextureRef mCannonTexture;	
	TSpriteRef mHudBackground;

	uint32_t mLastLoopTime; 
};

#endif // GAME_H_INCLUDED
