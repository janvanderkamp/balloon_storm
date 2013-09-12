//------------------------------------------------------------------
// Copyright (c) 2004-2008 PlayFirst, Inc. All rights reserved.
// Part of the Playground SDK(tm).
//
// This file contains proprietary information of PlayFirst, Inc.
// It is provided subject to a license agreement. It may not be
// reproduced, used, disclosed, sold or transferred without the
// prior written consent of PlayFirst, Inc.
//------------------------------------------------------------------


#include "serversubmit.h"

PFTYPEIMPL_DC(TServerSubmit);


TServerSubmit::TServerSubmit()
{
	mpPFHiscores = TSettings::GetInstance()->GetHiscores();

	TScript * s = TWindowManager::GetInstance()->GetScript();
	ScriptRegisterMemberDirect(s,"SubmitToServer",this,TServerSubmit::SubmitToServer);
}

TServerSubmit::~TServerSubmit()
{
	TScript * s = TWindowManager::GetInstance()->GetScript();
	ScriptUnregisterFunction(s,"SubmitToServer");
}

void TServerSubmit::Register()
{
	TWindowManager::GetInstance()->AddWindowType("SubmitWindow",TServerSubmit::ClassId());
	ScriptRegisterDirect(TWindowManager::GetInstance()->GetScript(), "GetLuaServerSubmitSetupVars", TServerSubmit::GetLuaSetupVars);

}

void TServerSubmit::SwitchModes( bool submit )
{
	TWindowManager::GetInstance()->GetScript()->DoLuaString( str::getFormatted("SwitchModes(%s)",submit?"true":"false") );
}

void TServerSubmit::SubmitToServer(str name, str account, str pass, bool remember, bool medalsOnly)
{
	bool success;
	TPfHiscores::ESubmitMode mode = TPfHiscores::kSubmitAll;
	if (medalsOnly)
	{
		mode = TPfHiscores::kSubmitMedal;
	}
	if (account.has_data())
	{
		success=mpPFHiscores->SubmitData(account, pass, remember, mode);
	}
	else
	{
		TSettings::GetInstance()->GetPreferences()->SetStr("_anonusername", name, TSettings::GetInstance()->GetCurrentUser(), true );
		success=mpPFHiscores->SubmitData(name, NULL, false, mode);
	}

	ASSERT(success);

	SwitchModes(true);
	StartWindowAnimation(20);
}

str TServerSubmit::GetLuaSetupVars(bool medalsMode)
{
	str setupString;
	str tmp ;

	tmp = TSettings::GetInstance()->GetPreferences()->GetStr("_anonusername","", TSettings::GetInstance()->GetCurrentUser() );

	setupString.format("gNameEdit='%s'; ", tmp );

	str userName;
	str password;
	if (TSettings::GetInstance()->GetHiscores()->GetRememberedUserInfo(&userName, &password))
	{
		setupString += str::getFormatted("gAccountEdit='%s'; ", userName );
		setupString += str::getFormatted("gPassEdit='%s'; ", password );
		setupString += "gRemember=true; ";
	}
	else
	{
		setupString += "gAccountEdit=''; gPassEdit=''; gRemember=false; ";
	}



	if (medalsMode == true)
	{
		setupString+= "gMedalsMode=true;";
	}
	else
	{
		setupString+= "gMedalsMode=false;";
		int score = 0;
		TSettings::GetInstance()->GetHiscores()->GetUserBestScore(TPfHiscores::eLocalBest, &score, NULL, NULL);
		setupString += str::getFormatted("gEligibleScore=%d", score);
	}
	return setupString;
}

bool TServerSubmit::OnMessage(TMessage * message)
{
	if (message->mType == TMessage::kTextEditChanged)
	{
		// Enable/disable the window based on text value.
		//
		// We can't use the magic TCustomTextEdit enable window feature here because
		// either of the two edit fields can have text to enable the button.
		TTextEdit *textEdit = GetChildWindow("nameedit", -1)->GetCast<TTextEdit>();
		TTextEdit *textEdit2 = GetChildWindow("accountedit", -1)->GetCast<TTextEdit>();
		if (textEdit->GetText().has_data() || textEdit2->GetText().has_data() )
		{
			TWindow *okwindow = GetChildWindow("submittoserver", -1);
			okwindow->AddFlags(kVisible);
		}
		else
		{
			TWindow *okwindow = GetChildWindow("submittoserver", -1);
			okwindow->RemoveFlags(kVisible);
			TButton *button = okwindow->GetCast<TButton>();
			button->OnMouseLeave();
		}
		TWindowManager::GetInstance()->InvalidateScreen();
		return true;
	}

	return false;
}

bool TServerSubmit::OnTaskAnimate()
{
	str msg;
	bool qual;
	TPfHiscores::EStatus status = mpPFHiscores->GetServerRequestStatus(&msg, &qual);
	if (status == TPfHiscores::eError)
	{
		TWindow *childWindow;

		childWindow = GetChildWindow("submiterror", -1);
		if (childWindow)
		{
			TText *text = childWindow->GetCast<TText>();
			text->AddFlags(TWindow::kVisible);
			text->SetText(msg);
		}

		TWindow *button;
		button = GetChildWindow("submitconnect", -1);
		if (button) button->RemoveFlags(TWindow::kVisible);

		button = GetChildWindow("submiterrorok", -1);
		if (button) button->AddFlags(TWindow::kVisible);

		return false;
	}
	else if (status == TPfHiscores::eSuccess)
	{
		if (qual)
		{
			TWindowManager::GetInstance()->GetScript()->DoLuaString("gReturnValue='qualified'");
		}
		else
		{
			TWindowManager::GetInstance()->GetScript()->DoLuaString("gReturnValue='success'");
		}
		TWindowManager::GetInstance()->PopModal( FindParentModal()->GetID());
		return false;
	}
	return true;
}