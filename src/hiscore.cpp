//------------------------------------------------------------------
// Copyright (c) 2004-2008 PlayFirst, Inc. All rights reserved.
// Part of the Playground SDK(tm).
//
// This file contains proprietary information of PlayFirst, Inc.
// It is provided subject to a license agreement. It may not be
// reproduced, used, disclosed, sold or transferred without the
// prior written consent of PlayFirst, Inc.
//------------------------------------------------------------------


#include "hiscore.h"
#include "globaldefines.h"
#include "settings.h"
#include "serversubmit.h"

#include <pf/pflib.h>

// -----------------------------------------




PFTYPEIMPL_DC(THiscore);

THiscore::THiscore() :
	mState(eLocalView),
	mCurrentCategory(0),
	mCurrentEditField(0),
	mScoreOffset(0)
{
	mpPFHiscores = TSettings::GetInstance()->GetHiscores();
	
	TScript * s = TWindowManager::GetInstance()->GetScript();
	ScriptRegisterMemberDirect(s,"GetState",this,THiscore::GetState);
	ScriptRegisterMemberDirect(s,"EligibleScoreAvailable",this,THiscore::EligibleScoreAvailable);
	ScriptRegisterMemberDirect(s,"ScoreAvailable",this,THiscore::ScoreAvailable);
	ScriptRegisterMemberDirect(s,"UpdateText",this,THiscore::UpdateText);
	ScriptRegisterMemberDirect(s,"LaunchPrivacyPolicy",this,THiscore::LaunchPrivacyPolicy);
	ScriptRegisterMemberDirect(s,"SubmissionDone",this,THiscore::SubmissionDone);
}


THiscore::~THiscore()
{
	TScript * s = TWindowManager::GetInstance()->GetScript();
	ScriptUnregisterFunction(s,"GetState");
	ScriptUnregisterFunction(s,"EligibleScoreAvailable");
	ScriptUnregisterFunction(s,"ScoreAvailable");
	ScriptUnregisterFunction(s,"UpdateText");
	ScriptUnregisterFunction(s,"SubmissionDone");
}


void THiscore::PostChildrenInit(TWindowStyle & style )
{
	TWindow::PostChildrenInit(style);
	
	StartWindowAnimation(20);
	
	for (int i = 0; i < NUM_SCORES_ON_SCREEN; i++)
	{
		TWindow *window;
		str buf;
		buf.format("name%d", i+1);
		window = GetChildWindow(buf, -1);
		if (window)
		{
			mNames[i] = window->GetCast<TText>();
		}
		buf.format("score%d", i+1);
		window = GetChildWindow(buf, -1);
		if (window)
		{
			mScores[i] = window->GetCast<TText>();
		}
		buf.format("%d", i+1);
		window = GetChildWindow(buf, -1);
		if (window)
		{
			mNumbers[i] = window->GetCast<TText>();
		}
		buf.format("p1_%d", i+1);
		window = GetChildWindow(buf, -1);
		if (window)
		{
			mP1s[i] = window->GetCast<TImage>();
		}
	}
	
	
	if (TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreLocalOnly))
	{
		TWindow *panel = GetChildWindow("leftpanel", -1);
		
		TRect rect;
		panel->GetWindowRect(&rect);
		panel->SetWindowPos(rect.x1 + 130, rect.y1);
		
	}
	
	
	TWindow *gameModeWindow = GetChildWindow("gamemode", -1);
	if (gameModeWindow)
	{
		mGameMode = gameModeWindow->GetCast<TText>();
	}
	UpdateText();
	UpdateButtons();
	
}

void THiscore::UpdateButtons()
{
	TWindowManager::GetInstance()->GetScript()->DoLuaString("UpdateButtons();");
}

void THiscore::HideAllText()
{
	TWindow *text;
	for (int i = 0; i < NUM_SCORES_ON_SCREEN; i++)
	{
		mNames[i]->RemoveFlags(TWindow::kVisible);
		mScores[i]->RemoveFlags(TWindow::kVisible);
		mNumbers[i]->RemoveFlags(TWindow::kVisible);
		mP1s[i]->RemoveFlags(TWindow::kVisible);
	}
	
	text = GetChildWindow("eligible", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("eligibleasterisk", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("server", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("error", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("info", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("category", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("local", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("global", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("topplayers", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("congratulations", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("dnq", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("yourrank", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("yourrankglobalinfo", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	text = GetChildWindow("rankvalue", -1);
	if (text) text->RemoveFlags(TWindow::kVisible);
	
	
	// These are really buttons, but associated with text
	TWindow *button;
	button = GetChildWindow("scrollup", -1);
	if (button) button->RemoveFlags(TWindow::kVisible);
	
	button = GetChildWindow("scrolldown", -1);
	if (button) button->RemoveFlags(TWindow::kVisible);
}

void THiscore::UpdateText()
{
	HideAllText();
	
	// Get the name of the current game mode
	TPlatform * platform = TPlatform::GetInstance();
	TStringTable * stringTable = platform->GetStringTable();
	TSettings * settings = TSettings::GetInstance();
	
	str gameModeStringKey = settings->GetPFGameModeName( settings->GetCurrentGameMode());
	mGameMode->SetText(stringTable->GetString( gameModeStringKey )); // Each game mode has a string defined in the string table
	
	if ( platform->IsEnabled( TPlatform::kHiscoreLocalOnly ))
	{
		TWindow *panel = GetChildWindow("rightpanel", -1);
		panel->RemoveFlags(kVisible);
		panel = GetChildWindow("rightpanelsmall", -1);
		panel->RemoveFlags(kVisible);
	}
	else
	{
		if (mState == eLocalView)
		{
			TWindow *panel = GetChildWindow("rightpanel", -1);
			panel->AddFlags(kVisible);
			panel = GetChildWindow("rightpanelsmall", -1);
			panel->RemoveFlags(kVisible);
			
		}
		else
		{
			TWindow *panel = GetChildWindow("rightpanel", -1);
			panel->RemoveFlags(kVisible);
			panel = GetChildWindow("rightpanelsmall", -1);
			panel->AddFlags(kVisible);
			
		}
	}
	
	if (mState == eLocalView || mState == eGlobalView)
	{
		UpdateScoresTable();
	}
	
	if (mState == eLocalView)
	{
		TWindow *text;
		int eligibleScore;
		if(mpPFHiscores->GetUserBestScore(TPfHiscores::eLocalEligible, &eligibleScore, NULL, NULL))
		{
			text = GetChildWindow("eligible", -1);
			if (text) text->AddFlags(TWindow::kVisible);
			
			text = GetChildWindow("eligibleasterisk", -1);
			if (text) text->AddFlags(TWindow::kVisible);
		}
		
		text = GetChildWindow("info", -1);
		if (text) text->AddFlags(TWindow::kVisible);
	}
	
	if (mState == eGlobalView)
	{
		TText *text;
		
		TWindow *childWindow = GetChildWindow("category", -1);
		if (childWindow)
		{
			text = childWindow->GetCast<TText>();
			text->AddFlags(TWindow::kVisible);
			
			str name;
			if (mpPFHiscores->GetCategoryName(mCurrentCategory, &name))
			{
				text->SetText(name);
			}
		}
		
		childWindow = GetChildWindow("yourrankglobalinfo", -1);
		if (childWindow) childWindow->AddFlags(TWindow::kVisible);
		
		int myRank;
		bool globalBest = mpPFHiscores->GetUserBestScore(TPfHiscores::eGlobalBest, NULL, &myRank, NULL);
		
		if (mSubmitState == eSuccess)
		{
			childWindow = GetChildWindow("congratulations", -1);
			if (childWindow)
			{
				text = childWindow->GetCast<TText>();
				text->AddFlags(TWindow::kVisible);
			}
			
			childWindow = GetChildWindow("yourrankglobalinfo", -1);
			if (childWindow) childWindow->RemoveFlags(TWindow::kVisible);
		}
		else if (mSubmitState == eDNQ)
		{
			childWindow = GetChildWindow("dnq", -1);
			if (childWindow)
			{
				text = childWindow->GetCast<TText>();
				text->AddFlags(TWindow::kVisible);
			}
			childWindow = GetChildWindow("yourrankglobalinfo", -1);
			if (childWindow) childWindow->RemoveFlags(TWindow::kVisible);
			
		}
		
		if (globalBest)
		{
			childWindow = GetChildWindow("yourrank", -1);
			if (childWindow)
			{
				text = childWindow->GetCast<TText>();
				text->AddFlags( TWindow::kVisible);
				
				str rankStr;
				rankStr.format("%d", myRank);
				str rankValue = stringTable->GetString("yourrank", rankStr);
				text->SetText(rankValue);
			}
			
			childWindow = GetChildWindow("yourrankglobalinfo", -1);
			if (childWindow) childWindow->RemoveFlags(TWindow::kVisible);
		}
		
		
		
	}
	if (mState == eRequestingCategories ||
		mState == eRequestingScores ||
		mState == eSubmitting)
	{
		TWindow *text;
		text = GetChildWindow("server", -1);
		if (text) text->AddFlags( TWindow::kVisible);
	}
	
	if (mState == eError)
	{
		TText *text;
		TWindow *childWindow = GetChildWindow("error", -1);
		if (childWindow)
		{
			text = childWindow->GetCast<TText>();
			text->AddFlags( TWindow::kVisible);
			text->SetText(mServerError);
		}
		
		
	}
	
	if ( TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreLocalOnly ) )
	{
		TWindow *text;
		text = GetChildWindow("info", -1);
		if (text) text->RemoveFlags(TWindow::kVisible);
		
		text = GetChildWindow("eligible", -1);
		if (text) text->RemoveFlags(TWindow::kVisible);
		
		text = GetChildWindow("eligibleasterisk", -1);
		if (text) text->RemoveFlags(TWindow::kVisible);
	}
	
}

void THiscore::UpdateScoresTable()
{
	int numScores = 0;
	
	bool local = true;
	if (mState == eGlobalView)
	{
		numScores = mpPFHiscores->GetScoreCount(false);
		TWindow *text;
		text = GetChildWindow("global", -1);
		if ( text && !TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreLocalOnly ) ) text->AddFlags(TWindow::kVisible);
		local = false;
		if (mScoreOffset > numScores - 5)
		{
			mScoreOffset = numScores - 5;
			
		}
		
		if (mScoreOffset < 0)
		{
			mScoreOffset = 0;
		}
		
		if (mScoreOffset > 0)
		{
			TWindow *scroll;
			scroll = GetChildWindow("scrollup", -1);
			scroll->AddFlags(kVisible);
		}
		else
		{
			TWindow *scroll;
			scroll = GetChildWindow("scrollup", -1);
			scroll->RemoveFlags(kVisible);
		}
		
		if (mScoreOffset + NUM_SCORES_ON_SCREEN < numScores)
		{
			TWindow *scroll;
			scroll = GetChildWindow("scrolldown", -1);
			scroll->AddFlags(kVisible);
		}
		else
		{
			TWindow *scroll;
			scroll = GetChildWindow("scrolldown", -1);
			scroll->RemoveFlags(kVisible);
		}
	}
	else
	{
		numScores = mpPFHiscores->GetScoreCount(true);
		TWindow *text;
		text = GetChildWindow("local", -1);
		if (text && !TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreLocalOnly ) ) text->AddFlags(TWindow::kVisible);
		
		text = GetChildWindow("topplayers", -1);
		if (text && TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreLocalOnly )) text->AddFlags(TWindow::kVisible);
		
		TWindow *scroll;
		scroll = GetChildWindow("scrollup", -1);
		scroll->RemoveFlags(kVisible);
		
		scroll = GetChildWindow("scrolldown", -1);
		scroll->RemoveFlags(kVisible);
		mScoreOffset = 0;
	}
	
	
	for (int i = 0; i + mScoreOffset < numScores && i < NUM_SCORES_ON_SCREEN; i++)
	{
		int rank;
		bool anonymous;
		str name;
		str gameData;
		int score;
		mpPFHiscores->GetScore(local, i+mScoreOffset, &rank, &name, &anonymous, &score, &gameData);
		
		mNames[i]->AddFlags(TWindow::kVisible);
		mNames[i]->SetText(name);
		
		TRect bounds;
		bool done = false;
		int fontSize = 20;
		// Shrink-to-fit
		while(!done && fontSize > 1)
		{
			mNames[i]->GetTextBounds(&bounds);
			if (bounds.x2<(int)mNames[i]->GetWindowWidth()
				&& (bounds.y1>=0)
				&& bounds.y2<=(int)mNames[i]->GetWindowHeight() ) // One line only
			{
				done=true;
			}
			else
			{
				--fontSize;
				mNames[i]->GetTextGraphic()->SetLineHeight( fontSize );
			}
		}
		
		str buf;
		buf.format("%d", score);
		mScores[i]->SetText(buf);
		mScores[i]->AddFlags(TWindow::kVisible);
		mNumbers[i]->AddFlags(TWindow::kVisible);
		buf.format("%d.", rank);
		mNumbers[i]->SetText(buf);
		if (anonymous == false
			&& !TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreLocalOnly )
			&& !TPlatform::GetInstance()->IsEnabled( TPlatform::kHiscoreAnonymous ))
		{
			mP1s[i]->AddFlags(TWindow::kVisible);
		}
	}
	
	
}

bool THiscore::OnMessage(TMessage * message)
{
	if (message->mType != TMessage::kButtonPress) return false;
	
	// To anyone who might be reading this:
	// This is an awkward way to get button messages. It's roughly how
	// messages are dispatched in Windows, and I'm sure that's what the
	// original author had in mind for a model, but it's not the
	// Playground-way to do things.
	
	bool handled = false;
	if (message->mName == "gamemodeleft")
	{
		if (mState == eGlobalView || mState == eLocalView)
		{
			int currentGameMode = (int)TSettings::GetInstance()->GetCurrentGameMode();
			currentGameMode--;
			if (currentGameMode < 0) currentGameMode = (int)TSettings::eMaxGameModes - 1;
			TSettings::GetInstance()->SetCurrentGameMode(currentGameMode);
			if (mState == eGlobalView)
			{
				mState = eRequestingScores;
				mpPFHiscores->RequestScores(mCurrentCategory);
			}
			mSubmitState = eNone;
			UpdateText();
			UpdateButtons();
			handled = true;
		}
	}
	else if (message->mName == "gamemoderight")
	{
		if (mState == eGlobalView || mState == eLocalView)
		{
			int currentGameMode = (int)TSettings::GetInstance()->GetCurrentGameMode();
			currentGameMode++;
			if (currentGameMode >= (int)TSettings::eMaxGameModes) currentGameMode = 0;
			TSettings::GetInstance()->SetCurrentGameMode(currentGameMode);
			if (mState == eGlobalView)
			{
				mState = eRequestingScores;
				mpPFHiscores->RequestScores(mCurrentCategory);
			}
			mSubmitState = eNone;
			UpdateText();
			UpdateButtons();
			handled = true;
		}
	}
	else if (message->mName == "categoryleft")
	{
		mCurrentCategory--;
		if (mCurrentCategory < 0)
		{
			mCurrentCategory = mpPFHiscores->GetCategoryCount() - 1;
		}
		mState = eRequestingScores;
		mpPFHiscores->RequestScores(mCurrentCategory);
		UpdateText();
		UpdateButtons();
		handled = true;
	}
	else if (message->mName == "categoryright")
	{
		mCurrentCategory++;
		if (mCurrentCategory >= mpPFHiscores->GetCategoryCount())
		{
			mCurrentCategory = 0;
		}
		mState = eRequestingScores;
		mpPFHiscores->RequestScores(mCurrentCategory);
		UpdateText();
		UpdateButtons();
		handled = true;
	}
	else if (message->mName == "view")
	{
		mSubmitState = eNone;
		mState = eRequestingCategories;
		mpPFHiscores->RequestCategoryInformation();
		UpdateText();
		UpdateButtons();
	}
	else if (message->mName == "viewlocal")
	{
		mState = eLocalView;
		UpdateText();
		UpdateButtons();
	}
	else if (message->mName == "scrollup")
	{
		mScoreOffset -= NUM_SCORES_ON_SCREEN;
		UpdateText();
	}
	else if (message->mName == "scrolldown")
	{
		mScoreOffset += NUM_SCORES_ON_SCREEN;
		UpdateText();
	}
	
	return handled;
}

void THiscore::SubmissionDone(bool qualified)
{
	mState = eRequestingCategories;
	mpPFHiscores->RequestCategoryInformation();
	
	if (qualified)
	{
		mSubmitState = eSuccess;
	}
	else
	{
		mSubmitState = eDNQ;
	}
	
	UpdateText();
	UpdateButtons();
}





/**
 * Called if you have initiated a window animation with
 * TWindow::StartWindowAnimation.
 *
 * @return True to continue animating. False to stop.
 */
bool THiscore::OnTaskAnimate()
{
	if (mState == eLocalView ||
		mState == eGlobalView ||
		mState == eError)
	{
		return true;
	}
	
	if (mState == eRequestingCategories ||
		mState == eRequestingScores)
	{
		str msg;
		bool qual;
		TPfHiscores::EStatus status = mpPFHiscores->GetServerRequestStatus(&msg, &qual);
		if (status == TPfHiscores::eError)
		{
			mServerError = msg;
			mState = eError;
			UpdateText();
			UpdateButtons();
			TWindowManager::GetInstance()->InvalidateScreen();
		}
		else if (status == TPfHiscores::eSuccess)
		{
			if (mState == eRequestingCategories)
			{
				mState = eRequestingScores;
				mpPFHiscores->RequestScores(mCurrentCategory);
			}
			else if (mState == eRequestingScores)
			{
				mState = eGlobalView;
				UpdateText();
				UpdateButtons();
				TWindowManager::GetInstance()->InvalidateScreen();
			}
		}
	}
	
	return true;
}

bool THiscore::EligibleScoreAvailable()
{
	int score;
	return mpPFHiscores->GetUserBestScore(TPfHiscores::eLocalEligible, &score, NULL, NULL);
}


bool THiscore::ScoreAvailable()
{
	int score;
	return mpPFHiscores->GetUserBestScore(TPfHiscores::eLocalBest, &score, NULL, NULL);
}


void THiscore::LaunchPrivacyPolicy()
{
	TPlatform::GetInstance()->OpenBrowser("http://www.playfirst.com/privacypolicy.html") ;
}
