//------------------------------------------------------------------
// Copyright (c) 2004-2008 PlayFirst, Inc. All rights reserved.
// Part of the Playground SDK(tm).
//
// This file contains proprietary information of PlayFirst, Inc.
// It is provided subject to a license agreement. It may not be
// reproduced, used, disclosed, sold or transferred without the
// prior written consent of PlayFirst, Inc.
//------------------------------------------------------------------



#define PLAYFIRST_MAIN

#include "key.h"
#include "serversubmit.h"
#include "settings.h"
#include "main.h"

/*
 * Early initialization of Playground values. TPlatform has not yet
 * been initialized here, so only static functions on TPlatform are
 * legal! At present this is basically only SetConfig(). This function
 * is really designed only for setting up values that TPlatform will need
 * during initialization; most application initialization still belongs in
 * Main().
 */
void pfPreInit()
{
	// Set the name of the game. This name will be used in the user: and
	// common: data folder names.
	TPlatform::SetConfig( TPlatform::kGameName, "Playground SDK Demo" );

	TPlatform::SetConfig( TPlatform::kVerboseDebug, "1" );

	// Set the PlayFirst game information. All PlayFirst games MUST set the
	// information below in order to successfully communicate with PlayFirst
	// services, such as the hiscore system. Removing this code will cause
	// the hiscore system to not work.
	// DO NOT REMOVE THIS CODE IF YOU PLAN ON USING PLAYFIRST SERVICES
	// ---------------------------------------------------------------
	TPlatform::SetConfig(TPlatform::kEncryptionKey,ENCRYPTION_KEY);

	// Set the hiscore handle of the Playground game.
	TPlatform::SetConfig(TPlatform::kPFGameHandle,PFGAMEHANDLE);

	// Publisher defaults to PlayFirst; you can change it here
	TPlatform::SetConfig(TPlatform::kPublisherName,"PlayFirst");

	for (int i = 0; i < sizeof(PFGAMEMODENAMES)/sizeof(char *); i++)
	{
		str name = str::getFormatted("%s%d", TPlatform::kPFGameModeName, i+1);
		TPlatform::SetConfig(name,PFGAMEMODENAMES[i]);
	}

	for (int i = 0; i < sizeof(PFGAMEMEDALNAMES)/sizeof(char *); i++)
	{
		str name = str::getFormatted("%s%d", TPlatform::kPFGameMedalName, i+1);
		TPlatform::SetConfig(name,PFGAMEMEDALNAMES[i]);
	}
	// ---------------------------------------------------------------
}

void SetUpElapsedTimeMetricLine( int minutes )
{
}

/*
 * This function sets up the metrics-gathering code in Playground SDK.
 * If you're attempting to understand the basics of Playground right
 * now, you can safely ignore it until later. :)
 *
 * See the documentation on TGameState and metrics for more
 * information.
 */
static void InitializeGameState()
{
}

/**
 * This function is called by Playground after basic engine
 * initialization, so you can set up real values here.
 *
 * Return value should be true if you want the game to continue
 * initializing. Otherwise return false and Playground should
 * exit.
 *
 * The string table has been loaded by this point.
 */
bool pfInit( const char * /*cmdLine*/ )
{
	TPlatform * p = TPlatform::GetInstance();

	TPlatform::SetConfig( TPlatform::kWindowTitle, p->GetStringTable()->GetString("windowtitle") );

	TSettings::CreateSettings();
	TSettings::GetInstance()->InitGameToSettings();

	TWindowManager * wm = TWindowManager::GetInstance();
	wm->AddWindowType("GameWindow",TGame::ClassId());
	
	wm->GetScript()->RunScript("scripts/mainloop.lua");

	return true;
}

bool pfEvent( const TEvent & event )
{
	if ( event.mType == TEvent::kClose )
	{
		TWindowManager::GetInstance()->GetScript()->RunScript("scripts/quitverify.lua");
	}

	if ( event.mType == TEvent::kFullScreenToggle )
	{
		TSettings::GetInstance()->UpdateFullScreen();
	}

	// Pass the event to the Window manager for further processing
	TWindowManager::GetInstance()->HandleEvent(&event);

	return true; // True to keep on going!
}

// You can put any periodic update events here. Or you can just register
// your events with TPlatform::AdoptTask() or TModalWindow::AdoptTask(),
// depending on whether the task should ALWAYS execute, or only when the
// modal window is on top.
void pfUpdate( float elapsedTime )
{
	TWindowManager::GetInstance()->Update(elapsedTime);
}

// Add any custom drawing code here you want. This function is called once
// per frame when it's time to draw.
//
// Most people won't need to modify this function at all. If you do want to add
// your own drawing code, don't forget you'll need to add a TRenderer::BeginDraw/EndDraw()
// pair. In TWindow::Draw()-derived functions, BeginDraw() has already been called for
// you.
void pfDraw()
{
	TWindowManager::GetInstance()->DrawStack();
}

// Add any of your own shut-down code here.
void pfShutDown()
{
	TSettings::DeleteSettings();
}

