//------------------------------------------------------------------
// Copyright (c) 2004-2008 PlayFirst, Inc. All rights reserved.
// Part of the Playground SDK(tm).
//
// This file contains proprietary information of PlayFirst, Inc.
// It is provided subject to a license agreement. It may not be
// reproduced, used, disclosed, sold or transferred without the
// prior written consent of PlayFirst, Inc.
//------------------------------------------------------------------

#include "settings.h"
#include "globaldefines.h"

#include <pf/pflib.h>
#include <pf/windowmanager.h>
#include <pf/script.h>
#include <pf/soundmanager.h>
#include <pf/stringtable.h>

#include "key.h"



TSettings *gSettings = NULL;

TSettings *TSettings::GetInstance()
{
	return gSettings;
}

TSettings *TSettings::CreateSettings()
{
	if (gSettings)
	{
		delete gSettings;
	}
	gSettings = new TSettings();
	return gSettings;
}
void TSettings::DeleteSettings()
{
	if (gSettings)
	{
		delete gSettings;
	}
	gSettings = NULL;
}

TSettings::TSettings()
{
	// Register some useful global GUI script functions. These functions will be available
	// in the global Lua GUI script.
	TScript * s = TWindowManager::GetInstance()->GetScript();
	ScriptRegisterMemberDirect( s, "ToggleFullScreen", this, TSettings::ToggleFullScreen );
	ScriptRegisterMemberDirect( s, "ToggleLetterbox", this, TSettings::ToggleLetterbox );
	ScriptRegisterMemberDirect( s, "ToggleSoundMute", this, TSettings::ToggleSoundMute );
	ScriptRegisterMemberDirect( s, "SetCurrentGameMode", this, TSettings::SetCurrentGameMode );
	ScriptRegisterMemberDirect( s, "GetNumUsers", this, TSettings::GetNumUsers );
	ScriptRegisterMemberDirect( s, "CreateNewUser", this, TSettings::CreateNewUser );
	ScriptRegisterMemberDirect( s, "SetCurrentUser", this, TSettings::SetCurrentUser );
	ScriptRegisterMemberDirect( s, "ChangeCurrentUserName", this, TSettings::ChangeCurrentUserName );
	ScriptRegisterMemberDirect( s, "DeleteUser", this, TSettings::DeleteUser );
	ScriptRegisterMemberDirect( s, "IsNameInUse", this, TSettings::IsNameInUse );
	ScriptRegisterMemberDirect( s, "QuitToMainMenu", this, TSettings::QuitToMainMenu );
	ScriptRegisterMemberDirect( s, "LogHighScore", this, TSettings::LogHighScore );
	ScriptRegisterMemberDirect( s, "NumMedalsToSubmit", this, TSettings::NumMedalsToSubmit );
	ScriptRegisterMemberDirect( s, "NumEarnedMedals", this, TSettings::NumEarnedMedals );
	ScriptRegisterMemberDirect( s, "GetMedalName", this, TSettings::GetMedalName );
	ScriptRegisterMemberDirect( s, "GetMedalType", this, TSettings::GetMedalType );
	ScriptRegisterMemberDirect( s, "GetMedalGameMode", this, TSettings::GetMedalGameMode );
	ScriptRegisterMemberDirect( s, "GetMedalGameData", this, TSettings::GetMedalGameData );
	ScriptRegisterMemberDirect( s, "GetPFGameModeName", this, TSettings::GetPFGameModeName );
	ScriptRegisterMemberDirect( s, "GetPFMedalName", this, TSettings::GetPFMedalName );

	// Load in saved preferences file
	mPreferences = new TPrefsDB();
	mHiscores = new TPfHiscores();

	SetCurrentGameMode(0);
	SetCurrentUser(GetCurrentUser());

}

TSettings::~TSettings()
{
	delete mPreferences;
	delete mHiscores;
}

#if _DEBUG
#define DEFAULT_FULLSCREEN 0
#else
#define DEFAULT_FULLSCREEN 1
#endif
#define DEFAULT_LETTERBOX 1

void TSettings::InitGameToSettings()
{
	TPlatform * p = TPlatform::GetInstance();

	// Set up the application display
	bool fullScreen = false;
	bool letterbox = false;

	if (mPreferences->GetInt("fullscreen", DEFAULT_FULLSCREEN) == 1)
	{
		fullScreen = true;
	}
	if (mPreferences->GetInt("letterbox", DEFAULT_LETTERBOX) == 1)
	{
		letterbox = true;
	}

	p->SetDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, fullScreen, letterbox);

	SetSoundForCurrentUser();
}

bool TSettings::IsFullScreen()
{
	return TPlatform::GetInstance()->IsFullscreen();
}

bool TSettings::IsLetterbox()
{
	return TPlatform::GetInstance()->IsLetterbox();
}

void TSettings::UpdateFullScreen()
{
	bool full = TPlatform::GetInstance()->IsFullscreen();

	if (!full)
	{
		mPreferences->SetInt("fullscreen", 0);
	}
	else
	{
		mPreferences->SetInt("fullscreen", 1);
	}
}

bool TSettings::ToggleFullScreen()
{
	bool full = TPlatform::GetInstance()->IsFullscreen();

	bool success = true;

	if (!full)
	{
		if (TPlatform::GetInstance()->SetFullscreen(true))
		{
			mPreferences->SetInt("fullscreen", 1);
		}
		else
		{
			success = false;
		}
	}
	else
	{
		if (TPlatform::GetInstance()->SetFullscreen(false))
		{
			mPreferences->SetInt("fullscreen", 0);
		}
		else
		{
			success = false;
		}
	}

	return success;
}


bool TSettings::ToggleLetterbox()
{
	bool letterbox = TPlatform::GetInstance()->IsLetterbox();

	bool success = true;

	if (!letterbox)
	{
		if (TPlatform::GetInstance()->SetLetterbox(true))
		{
			mPreferences->SetInt("letterbox", 1);
		}
		else
		{
			success = false;
		}
	}
	else
	{
		if (TPlatform::GetInstance()->SetLetterbox(false))
		{
			mPreferences->SetInt("letterbox", 0);
		}
		else
		{
			success = false;
		}
	}

	return success;
}


void TSettings::ToggleSoundMute()
{
	int soundOff = mPreferences->GetInt("soundoff", 0, TPrefsDB::kCurrentUser);
	soundOff = (soundOff+1)%2;

	TGameState::GetInstance()->SetState(TGameState::kMute,soundOff);
	mPreferences->SetInt("soundoff", soundOff, TPrefsDB::kCurrentUser);

	// In case some other process is overriding the game's mute request
	soundOff = TGameState::GetInstance()->QueryMuteGame();

	if(soundOff == 0)
	{
		TSoundManager::GetInstance()->SetVolume(1);
	}
	else
	{
		TSoundManager::GetInstance()->SetVolume(0);
	}
}

void TSettings::SetCurrentGameMode(int gameMode)
{
	if (gameMode < 0 || gameMode >= eMaxGameModes) return;

	mGameMode = (eGameMode)gameMode;

	mHiscores->SetProperty(TPfHiscores::eGameMode, PFGAMEMODENAMES[mGameMode]);
}


void TSettings::CreateNewUser(str userName)
{
	int numUsers = GetNumUsers();
	mPreferences->SetStr("name", userName, numUsers);
	// copy the sound settings from the current user so that the sound isn't jarring
	// when creating a new user
	int currentUser = GetCurrentUser();
	int soundVal = mPreferences->GetInt("soundlevel", DEFAULT_SOUND_VOLUME, currentUser);
	int musicVal = mPreferences->GetInt("musiclevel", DEFAULT_SOUND_VOLUME, currentUser);
	int soundOff = mPreferences->GetInt("soundoff", 0, currentUser);
	mPreferences->SetInt("soundlevel", soundVal, numUsers);
	mPreferences->SetInt("musiclevel", musicVal, numUsers);
	mPreferences->SetInt("soundoff", soundOff, numUsers);

	SetCurrentUser(numUsers);

}

void TSettings::ChangeCurrentUserName(str newName)
{
	mPreferences->SetStr("name", newName, TPrefsDB::kCurrentUser);
}

str TSettings::GetCurrentUserName()
{
	str username = mPreferences->GetStr("name", "", TPrefsDB::kCurrentUser);
	return username;
}

str TSettings::GetUserName(int index)
{
	return mPreferences->GetStr("name", "", index);
}

void TSettings::SetCurrentUser(int userNum)
{
	int numUsers = GetNumUsers();
	if (userNum >= numUsers) return;

	mPreferences->SetCurrentUser(userNum);
	if (userNum >= 0)
	{
		mHiscores->SetProperty(TPfHiscores::ePlayerName, GetCurrentUserName() );
		SetSoundForCurrentUser();
	}

}

int TSettings::GetCurrentUser()
{
	return mPreferences->GetCurrentUser();
}

int TSettings::GetNumUsers()
{
	return mPreferences->GetNumUsers();
}

void TSettings::SetSoundForCurrentUser()
{
	// initialize volume settings
	int soundVal = mPreferences->GetInt("soundlevel", DEFAULT_SOUND_VOLUME, TPrefsDB::kCurrentUser);
	int musicVal = mPreferences->GetInt("musiclevel", DEFAULT_SOUND_VOLUME, TPrefsDB::kCurrentUser);

	TSoundManager::GetInstance()->SetTypeVolume(TSound::kSFXSoundGroup, (float)soundVal/1000.0f);
	TSoundManager::GetInstance()->SetTypeVolume(TSound::kMusicSoundGroup, (float)musicVal/1000.0f);


	int soundOff = mPreferences->GetInt("soundoff", 0, TPrefsDB::kCurrentUser);
	if(soundOff == 0)
	{
		TSoundManager::GetInstance()->SetVolume(1);
	}
	else
	{
		TSoundManager::GetInstance()->SetVolume(0);
	}
}

void TSettings::DeleteUser(int userNum)
{
	mHiscores->ClearPlayerData(GetUserName(userNum));
	mPreferences->DeleteUser(userNum);
	userNum--;
	if (userNum < 0)
	{
		userNum = GetNumUsers()-1;
	}
	SetCurrentUser(userNum);
}

bool TSettings::IsNameInUse(str name)
{
	int numUsers = GetNumUsers();

	for (int i = 0; i < numUsers; i++)
	{
		if (name == GetUserName(i))
		{
			return true;
		}
	}

	return false;
}

void TSettings::QuitToMainMenu()
{
	TWindowManager * wm = TWindowManager::GetInstance();

	while (wm->GetTopModalWindow() && wm->GetTopModalWindow()->GetParent())
	{
		wm->PopModal( wm->GetTopModalWindow()->GetID() );
	}
}

void TSettings::LogHighScore(int score)
{
	bool replace = false;

	GetHiscores()->KeepScore(score, replace, NULL);
	TGameState::GetInstance()->SetState("LastGameHiScore", score);
}


str TSettings::GetPFGameModeName(int index)
{
	if (index >= 0 && index < sizeof(PFGAMEMODENAMES)/sizeof(char *))
	{
		return PFGAMEMODENAMES[index];
	}
	else
	{
		return "";
	}
}

str TSettings::GetPFMedalName(int index)
{
	if (index >= 0 && index < sizeof(PFGAMEMEDALNAMES)/sizeof(char *))
	{
		return PFGAMEMEDALNAMES[index];
	}
	else
	{
		return "";
	}
}

int TSettings::NumMedalsToSubmit()
{
	return mHiscores->GetNumMedalsToSubmit();
}

int TSettings::NumEarnedMedals()
{
	return mHiscores->GetNumMedalsEarned();
}

str TSettings::GetMedalName(int index)
{
	str name;
	mHiscores->GetEarnedMedalInfo(index, &name, NULL, NULL, NULL);
	return name;
}

int TSettings::GetMedalType(int index)
{
	TPfHiscores::EMedalType type;
	mHiscores->GetEarnedMedalInfo(index, NULL, &type, NULL, NULL);
	return type;
}

str TSettings::GetMedalGameMode(int index)
{
	str mode;
	mHiscores->GetEarnedMedalInfo(index, NULL, NULL, &mode, NULL);
	return mode;
}

str TSettings::GetMedalGameData(int index)
{
	str data;
	mHiscores->GetEarnedMedalInfo(index, NULL, NULL, NULL, &data);
	return data;
}