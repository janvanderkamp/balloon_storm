/**
 *	gameVariables.h - Jan van der Kamp, 2011
 */
#ifndef GAMEVARIABLES_H_INCLUDED
#define GAMEVARIABLES_H_INCLUDED

#include <pf/pflib.h>
#include "../globaldefines.h"

/** @namespace gameVars - This namespace is a collection of const variables which can be set here to tweak the game, 
 *						  and avoids having magic numbers littered throughout the code.
 *
 *	@variable 	TColor				backgroundColour					The background colour of the whole screen
 *	@variable 	TVec2				burstBalloonVelocity				The velocity of a balloon once it has been burst
 *	@variable 	uint16_t			initialMinWaitForBalloon			Initial shortest time to wait for new balloon  
 *	@variable 	uint16_t			initialMaxWaitForBalloon			Initial longest time to wait for new balloon 
 *	@variable 	uint16_t			waitTimeDecrease					Time in milliseconds that mMinWaitForBalloon & mMaxWaitForBalloon 
 *																		decrease by when new level is reached 
 *	@variable 	uint16_t			balloonsBurstToLevelUp				Number of balloons necessary to burst in order to level up 
 *	@variable 	uint16_t			initialNumColoursInPlay				Range of colours that bullets can be 
 *	@variable 	uint16_t			levelsToPassForNewColour			Number of levels player needs to pass before introducing a new colour 
 *	@variable 	uint32_t			hudBoundary							Height at which HUD area stops
 *	@variable 	TReal				balloonScale						The scale of the falling balloons 
 *	@variable 	TReal				balloonVelocityIncrease				Amount that mBalloonVelocity.y increases by when new level is reached
 *	@variable 	TVec2				initialBalloonVelocity				Initial velocity of falling balloons
 *	@variable 	TVec2				cannonPosition						Position of base of cannon 
 *	@variable 	TReal				bulletScale							The scale of the cannon's bullets 
 *	@variable 	TReal				barrierRiseSpeed					Speed at which barrier rises
 *	@variable 	TReal				initialBarrierParallaxDifference	Difference in height between the first and last images of balloons which  
 *																		make up the barrier 
 *	@variable 	TReal				barrierLevelHeight					Height at which each element of mBarrierTextures appear at the same height 
 *	@variable 	TVec2				initialBarrierPosition				The overall position of the barrier when the game started 
 *	@variable 	uint32_t			textSize							The size of text on screen 
 *	@variable 	uint32_t			messageW							The width of messages to place on buttons etc. 
 *	@variable 	uint32_t			messageH							The height of messages to place on buttons etc. 
 *	@variable 	uint32_t			helpTextW							The width of the game instructions 
 *	@variable 	uint32_t			helpTextH							The height of the game instructions 
 *	@variable 	TVec2				pauseMSGPosition						Position that the pause text should be drawn at
 *	@variable 	TVec2				gameOverMSGPosition					Position that the game over text should be drawn at 
 *	@variable 	TVec2				gameInfoPosition							Position that the Score/Level button should be drawn at 
 *	@variable 	TVec2				pauseButtonPosition						Position that the Pause/Unpause button should be drawn at 
 *	@variable 	TVec2				newGameButtonPosition					Position that the New Game button should be drawn at 
 *	@variable 	TVec2				quitButtonPosition						Position that the Quit button should be drawn at 
 *	@variable 	TVec2				helpMSGPosition						Position that the Help button should be drawn at 
 */
namespace gameVars {
	// BACKGROUND COLOUR
	const TColor backgroundColour(.36f,.92f,.95f,1.f);

	// BALL VARIABLES
	const TVec2 burstBalloonVelocity(0, .1f);

	// BALLOON MANAGER VARIABLES
	const uint16_t initialMinWaitForBalloon = 4500;
	const uint16_t initialMaxWaitForBalloon = 6000;
	const uint16_t waitTimeDecrease = 200;
	const uint16_t balloonsBurstToLevelUp = 3;
	const uint16_t initialNumColoursInPlay = 3;
	const uint16_t levelsToPassForNewColour = 3;
	const uint32_t hudBoundary = 81;
	const TReal balloonScale = 0.7f; 
	const TReal balloonVelocityIncrease = 0.004f;
	const TVec2 initialBalloonVelocity(0, .03f);

	// CANNON VARIABLES
	const TVec2 cannonPosition(TReal(SCREEN_WIDTH / 2), TReal(SCREEN_HEIGHT - 100));
	const TReal bulletScale = .35f;  

	// BARRIER VARIABLES 
	const TReal barrierRiseSpeed = .1f;
	const TReal initialBarrierParallaxDifference = 100.f;
	const TReal barrierLevelHeight = SCREEN_HEIGHT - 200.f;
	const TVec2 initialBarrierPosition(TReal(SCREEN_WIDTH / 2), TReal(SCREEN_HEIGHT));

	// TEXT INFORMATION
	const uint32_t	textSize = 32;

	// MESSAGE POSITIONS
	const uint32_t  messageW = 150;
	const uint32_t  messageH = 32;
	const uint32_t  helpTextW = 400;
	const uint32_t  helpTextH = 400;
	const TVec2		pauseMSGPosition(SCREEN_WIDTH/2 - messageW/2, SCREEN_HEIGHT/2);
	const TVec2		gameOverMSGPosition(pauseMSGPosition);	
	
	const TVec2		gameInfoPosition(96.f, 39.f);
	const TVec2		pauseButtonPosition(248.f, 39.f);
	const TVec2		newGameButtonPosition(400.f, 39.f);
	const TVec2		helpButtonPosition(552.f, 39.f);
	const TVec2		quitButtonPosition(704.f, 39.f);
	const TVec2		helpMSGPosition(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
}

#endif // GAMEVARIABLES_H_INCLUDED