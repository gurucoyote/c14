/** 
 * @file hbprefscool.cpp
 * @author Henri Beauchamp
 * @brief Cool VL Viewer preferences panel
 *
 * $LicenseInfo:firstyear=2008&license=viewergpl$
 * 
 * Copyright (c) 2008-2010, Henri Beauchamp.
 * 
 * Second Life Viewer Source Code
 * The source code in this file ("Source Code") is provided by Linden Lab
 * to you under the terms of the GNU General Public License, version 2.0
 * ("GPL"), unless you have obtained a separate licensing agreement
 * ("Other License"), formally executed by you and Linden Lab.  Terms of
 * the GPL can be found in doc/GPL-license.txt in this distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * There are special exceptions to the terms and conditions of the GPL as
 * it is applied to this Source Code. View the full text of the exception
 * in the file doc/FLOSS-exception.txt in this software distribution, or
 * online at http://secondlifegrid.net/programs/open_source/licensing/flossexception
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL LINDEN LAB SOURCE CODE IS PROVIDED "AS IS." LINDEN LAB MAKES NO
 * WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY,
 * COMPLETENESS OR PERFORMANCE.
 * $/LicenseInfo$
 */

#include "llviewerprecompiledheaders.h"

#include "hbprefscool.h"

#include "llstartup.h"
#include "llviewercontrol.h"
#include "lluictrlfactory.h"
#include "llcheckboxctrl.h"
#include "llcombobox.h"
#include "llcolorswatch.h"

class HBPrefsCoolImpl : public LLPanel
{
public:
	HBPrefsCoolImpl();
	/*virtual*/ ~HBPrefsCoolImpl() { };

	/*virtual*/ void refresh();

	void apply();
	void cancel();

private:
	static void onCommitCheckBoxShowButton(LLUICtrl* ctrl, void* user_data);
	static void onCommitCheckBoxRestrainedLove(LLUICtrl* ctrl, void* user_data);
	static void onCommitCheckBoxSpeedRez(LLUICtrl* ctrl, void* user_data);
	static void onCommitCheckBoxAfterRestart(LLUICtrl* ctrl, void* user_data);
	void refreshValues();

	BOOL mDoubleClickTeleport;
	BOOL mDoubleClickAutoPilot;
	BOOL mHideMasterRemote;
	BOOL mShowChatButton;
	BOOL mShowIMButton;
	BOOL mShowFriendsButton;
	BOOL mShowGroupsButton;
	BOOL mShowFlyButton;
	BOOL mShowSnapshotButton;
	BOOL mShowSearchButton;
	BOOL mShowBuildButton;
	BOOL mShowRadarButton;
	BOOL mShowMiniMapButton;
	BOOL mShowMapButton;
	BOOL mShowInventoryButton;
	BOOL mUseOldChatHistory;
	BOOL mIMTabsVerticalStacking;
	BOOL mUseOldStatusBarIcons;
	BOOL mUseOldTrackingDots;
	BOOL mAllowMUpose;
	BOOL mAutoCloseOOC;
	BOOL mPrivateLookAt;
	BOOL mFetchInventoryOnLogin;
	BOOL mRestrainedLove;
	BOOL mPreviewAnimInWorld;
	BOOL mSpeedRez;
	BOOL mRevokePermsOnStandUp;
	BOOL mRezWithLandGroup;
	BOOL mStackMinimizedTopToBottom;
	BOOL mStackMinimizedRightToLeft;
	BOOL mHideTeleportProgress;
	BOOL mHighlightOwnNameInChat;
	BOOL mHighlightOwnNameInIM;
	BOOL mHighlightDisplayName;
	BOOL mTeleportHistoryDeparture;
	BOOL mAvatarPhysics;
	LLColor4 mOwnNameChatColor;
	std::string mHighlightNicknames;
	U32 mStackScreenWidthFraction;
	U32 mSpeedRezInterval;
	U32 mDecimalsForTools;
	U32 mTimeFormat;
	U32 mDateFormat;
};


HBPrefsCoolImpl::HBPrefsCoolImpl()
 : LLPanel(std::string("Cool Preferences Panel"))
{
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_preferences_cool.xml");
	childSetCommitCallback("show_chat_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_im_button_check",				onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_friends_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_group_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_fly_button_check",				onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_snapshot_button_check",		onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_search_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_build_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_radar_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_minimap_button_check",			onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_map_button_check",				onCommitCheckBoxShowButton, this);
	childSetCommitCallback("show_inventory_button_check",		onCommitCheckBoxShowButton, this);
	childSetCommitCallback("restrained_love_check",				onCommitCheckBoxRestrainedLove, this);
	childSetCommitCallback("speed_rez_check",					onCommitCheckBoxSpeedRez, this);
	childSetCommitCallback("use_old_chat_history_check",		onCommitCheckBoxAfterRestart, this);
	childSetCommitCallback("im_tabs_vertical_stacking_check",	onCommitCheckBoxAfterRestart, this);
	childSetCommitCallback("use_old_statusbar_icons_check",		onCommitCheckBoxAfterRestart, this);
	refresh();
}

//static
void HBPrefsCoolImpl::onCommitCheckBoxShowButton(LLUICtrl* ctrl, void* user_data)
{
	HBPrefsCoolImpl* self = (HBPrefsCoolImpl*)user_data;
	LLCheckBoxCtrl* check = (LLCheckBoxCtrl*)ctrl;
	if (!self || !check) return;

	bool enabled = check->get();
	if (enabled && gSavedSettings.getBOOL("ShowToolBar") == FALSE)
	{
		gSavedSettings.setBOOL("ShowToolBar", TRUE);
	}
}

//static
void HBPrefsCoolImpl::onCommitCheckBoxRestrainedLove(LLUICtrl* ctrl, void* user_data)
{
	HBPrefsCoolImpl* self = (HBPrefsCoolImpl*)user_data;
	LLCheckBoxCtrl* check = (LLCheckBoxCtrl*)ctrl;
	if (!self || !check) return;

	if (check->get())
	{
		gSavedSettings.setBOOL("FetchInventoryOnLogin",	TRUE);
		self->childSetValue("fetch_inventory_on_login_check", TRUE);
		self->childDisable("fetch_inventory_on_login_check");
	}
	else
	{
		self->childEnable("fetch_inventory_on_login_check");
	}
	LLNotifications::instance().add("InEffectAfterRestart");
}

//static
void HBPrefsCoolImpl::onCommitCheckBoxSpeedRez(LLUICtrl* ctrl, void* user_data)
{
	HBPrefsCoolImpl* self = (HBPrefsCoolImpl*)user_data;
	LLCheckBoxCtrl* check = (LLCheckBoxCtrl*)ctrl;
	if (!self || !check) return;

	bool enabled = check->get();
	self->childSetEnabled("speed_rez_interval", enabled);
	self->childSetEnabled("speed_rez_seconds", enabled);
}

//static
void HBPrefsCoolImpl::onCommitCheckBoxAfterRestart(LLUICtrl* ctrl, void* user_data)
{
	HBPrefsCoolImpl* self = (HBPrefsCoolImpl*)user_data;
	LLCheckBoxCtrl* check = (LLCheckBoxCtrl*)ctrl;
	if (!self || !check) return;

	BOOL saved = FALSE;
	std::string control = check->getControlName();
	if (control == "UseOldChatHistory")
	{
		saved = self->mUseOldChatHistory;
	}
	else if (control == "IMTabsVerticalStacking")
	{
		saved = self->mIMTabsVerticalStacking;
	}
	else if (control == "UseOldStatusBarIcons")
	{
		saved = self->mUseOldStatusBarIcons;
	}
 	if (saved != check->get())
	{
		LLNotifications::instance().add("InEffectAfterRestart");
	}
}

void HBPrefsCoolImpl::refreshValues()
{
	mDoubleClickTeleport		= gSavedSettings.getBOOL("DoubleClickTeleport");
	mDoubleClickAutoPilot		= (mDoubleClickTeleport ? FALSE : gSavedSettings.getBOOL("DoubleClickAutoPilot"));
	mHideMasterRemote			= gSavedSettings.getBOOL("HideMasterRemote");
	mShowChatButton				= gSavedSettings.getBOOL("ShowChatButton");
	mShowIMButton				= gSavedSettings.getBOOL("ShowIMButton");
	mShowFriendsButton			= gSavedSettings.getBOOL("ShowFriendsButton");
	mShowGroupsButton			= gSavedSettings.getBOOL("ShowGroupsButton");
	mShowFlyButton				= gSavedSettings.getBOOL("ShowFlyButton");
	mShowSnapshotButton			= gSavedSettings.getBOOL("ShowSnapshotButton");
	mShowSearchButton			= gSavedSettings.getBOOL("ShowSearchButton");
	mShowBuildButton			= gSavedSettings.getBOOL("ShowBuildButton");
	mShowRadarButton			= gSavedSettings.getBOOL("ShowRadarButton");
	mShowMiniMapButton			= gSavedSettings.getBOOL("ShowMiniMapButton");
	mShowMapButton				= gSavedSettings.getBOOL("ShowMapButton");
	mShowInventoryButton		= gSavedSettings.getBOOL("ShowInventoryButton");
	mUseOldChatHistory			= gSavedSettings.getBOOL("UseOldChatHistory");
	mIMTabsVerticalStacking		= gSavedSettings.getBOOL("IMTabsVerticalStacking");
	mUseOldStatusBarIcons		= gSavedSettings.getBOOL("UseOldStatusBarIcons");
	mUseOldTrackingDots			= gSavedSettings.getBOOL("UseOldTrackingDots");
	mAllowMUpose				= gSavedSettings.getBOOL("AllowMUpose");
	mAutoCloseOOC				= gSavedSettings.getBOOL("AutoCloseOOC");
	mPrivateLookAt				= gSavedSettings.getBOOL("PrivateLookAt");
	mRestrainedLove				= gSavedSettings.getBOOL("RestrainedLove");
	if (mRestrainedLove)
	{
		mFetchInventoryOnLogin	= TRUE;
		gSavedSettings.setBOOL("FetchInventoryOnLogin",	TRUE);
	}
	else
	{
		mFetchInventoryOnLogin	= gSavedSettings.getBOOL("FetchInventoryOnLogin");
	}
	mPreviewAnimInWorld			= gSavedSettings.getBOOL("PreviewAnimInWorld");
	mSpeedRez					= gSavedSettings.getBOOL("SpeedRez");
	mSpeedRezInterval			= gSavedSettings.getU32("SpeedRezInterval");
	mDecimalsForTools			= gSavedSettings.getU32("DecimalsForTools");
	mRevokePermsOnStandUp		= gSavedSettings.getBOOL("RevokePermsOnStandUp");
	mRezWithLandGroup			= gSavedSettings.getBOOL("RezWithLandGroup");
	mStackMinimizedTopToBottom	= gSavedSettings.getBOOL("StackMinimizedTopToBottom");
	mStackMinimizedRightToLeft	= gSavedSettings.getBOOL("StackMinimizedRightToLeft");
	mStackScreenWidthFraction	= gSavedSettings.getU32("StackScreenWidthFraction");
	mHideTeleportProgress		= gSavedSettings.getBOOL("HideTeleportProgress");
	mHighlightOwnNameInChat		= gSavedSettings.getBOOL("HighlightOwnNameInChat");
	mHighlightOwnNameInIM		= gSavedSettings.getBOOL("HighlightOwnNameInIM");
	mOwnNameChatColor			= gSavedSettings.getColor4("OwnNameChatColor");
	if (LLStartUp::getStartupState() == STATE_STARTED)
	{
		mHighlightNicknames		= gSavedPerAccountSettings.getString("HighlightNicknames");
		mHighlightDisplayName	= gSavedPerAccountSettings.getBOOL("HighlightDisplayName");
	}
	mTeleportHistoryDeparture	= gSavedSettings.getBOOL("TeleportHistoryDeparture");
	mAvatarPhysics				= gSavedSettings.getBOOL("AvatarPhysics");
}

void HBPrefsCoolImpl::refresh()
{
	refreshValues();

	if (LLStartUp::getStartupState() != STATE_STARTED)
	{
		childDisable("restrained_love_check");
		childDisable("highlight_nicknames_text");
		childDisable("highlight_display_name_check");
	}
	else
	{
		childSetValue("highlight_nicknames_text", mHighlightNicknames);
		childSetValue("highlight_display_name_check", mHighlightDisplayName);
	}

	if (mRestrainedLove)
	{
		childSetValue("fetch_inventory_on_login_check", TRUE);
		childDisable("fetch_inventory_on_login_check");
	}
	else
	{
		childEnable("fetch_inventory_on_login_check");
	}

	if (mSpeedRez)
	{
		childEnable("speed_rez_interval");
		childEnable("speed_rez_seconds");
	}
	else
	{
		childDisable("speed_rez_interval");
		childDisable("speed_rez_seconds");
	}

	std::string format = gSavedSettings.getString("ShortTimeFormat");
	if (format.find("%p") == -1)
	{
		mTimeFormat = 0;
	}
	else
	{
		mTimeFormat = 1;
	}

	format = gSavedSettings.getString("ShortDateFormat");
	if (format.find("%m/%d/%") != -1)
	{
		mDateFormat = 2;
	}
	else if (format.find("%d/%m/%") != -1)
	{
		mDateFormat = 1;
	}
	else
	{
		mDateFormat = 0;
	}

	// time format combobox
	LLComboBox* combo = getChild<LLComboBox>("time_format_combobox");
	if (combo)
	{
		combo->setCurrentByIndex(mTimeFormat);
	}

	// date format combobox
	combo = getChild<LLComboBox>("date_format_combobox");
	if (combo)
	{
		combo->setCurrentByIndex(mDateFormat);
	}
}

void HBPrefsCoolImpl::cancel()
{
	gSavedSettings.setBOOL("DoubleClickTeleport",		mDoubleClickTeleport);
	gSavedSettings.setBOOL("DoubleClickAutoPilot",		mDoubleClickTeleport ? FALSE : mDoubleClickAutoPilot);
	gSavedSettings.setBOOL("HideMasterRemote",			mHideMasterRemote);
	gSavedSettings.setBOOL("ShowChatButton",			mShowChatButton);
	gSavedSettings.setBOOL("ShowIMButton",				mShowIMButton);
	gSavedSettings.setBOOL("ShowFriendsButton",			mShowFriendsButton);
	gSavedSettings.setBOOL("ShowGroupsButton",			mShowGroupsButton);
	gSavedSettings.setBOOL("ShowFlyButton",				mShowFlyButton);
	gSavedSettings.setBOOL("ShowSnapshotButton",		mShowSnapshotButton);
	gSavedSettings.setBOOL("ShowSearchButton",			mShowSearchButton);
	gSavedSettings.setBOOL("ShowBuildButton",			mShowBuildButton);
	gSavedSettings.setBOOL("ShowRadarButton",			mShowRadarButton);
	gSavedSettings.setBOOL("ShowMiniMapButton",			mShowMiniMapButton);
	gSavedSettings.setBOOL("ShowMapButton",				mShowMapButton);
	gSavedSettings.setBOOL("ShowInventoryButton",		mShowInventoryButton);
	gSavedSettings.setBOOL("UseOldChatHistory",			mUseOldChatHistory);
	gSavedSettings.setBOOL("IMTabsVerticalStacking",	mIMTabsVerticalStacking);
	gSavedSettings.setBOOL("UseOldStatusBarIcons",		mUseOldStatusBarIcons);
	gSavedSettings.setBOOL("UseOldTrackingDots",		mUseOldTrackingDots);
	gSavedSettings.setBOOL("AllowMUpose",				mAllowMUpose);
	gSavedSettings.setBOOL("AutoCloseOOC",				mAutoCloseOOC);
	gSavedSettings.setBOOL("PrivateLookAt",				mPrivateLookAt);
	gSavedSettings.setBOOL("FetchInventoryOnLogin",		mFetchInventoryOnLogin);
	gSavedSettings.setBOOL("RestrainedLove",			mRestrainedLove);
	gSavedSettings.setBOOL("PreviewAnimInWorld",		mPreviewAnimInWorld);
	gSavedSettings.setBOOL("SpeedRez",					mSpeedRez);
	gSavedSettings.setU32("SpeedRezInterval",			mSpeedRezInterval);
	gSavedSettings.setU32("DecimalsForTools",			mDecimalsForTools);
	gSavedSettings.setBOOL("RevokePermsOnStandUp",		mRevokePermsOnStandUp);
	gSavedSettings.setBOOL("RezWithLandGroup",			mRezWithLandGroup);
	gSavedSettings.setBOOL("StackMinimizedTopToBottom",	mStackMinimizedTopToBottom);
	gSavedSettings.setBOOL("StackMinimizedRightToLeft",	mStackMinimizedRightToLeft);
	gSavedSettings.setU32("StackScreenWidthFraction",	mStackScreenWidthFraction);
	gSavedSettings.setBOOL("HideTeleportProgress",		mHideTeleportProgress);
	gSavedSettings.setBOOL("HighlightOwnNameInChat",	mHighlightOwnNameInChat);
	gSavedSettings.setBOOL("HighlightOwnNameInIM",		mHighlightOwnNameInIM);
	gSavedSettings.setColor4("OwnNameChatColor",		mOwnNameChatColor);
	if (LLStartUp::getStartupState() == STATE_STARTED)
	{
		gSavedPerAccountSettings.setString("HighlightNicknames", mHighlightNicknames);
		gSavedPerAccountSettings.setBOOL("HighlightDisplayName", mHighlightDisplayName);
	}
	gSavedSettings.setBOOL("TeleportHistoryDeparture",	mTeleportHistoryDeparture);
	gSavedSettings.setBOOL("AvatarPhysics",				mAvatarPhysics);
}

void HBPrefsCoolImpl::apply()
{
	std::string short_date, long_date, short_time, long_time, timestamp;	

	LLComboBox* combo = getChild<LLComboBox>("time_format_combobox");
	if (combo)
	{
		mTimeFormat = combo->getCurrentIndex();
	}

	combo = getChild<LLComboBox>("date_format_combobox");
	if (combo)
	{
		mDateFormat = combo->getCurrentIndex();
	}

	if (mTimeFormat == 0)
	{
		short_time = "%H:%M";
		long_time  = "%H:%M:%S";
		timestamp  = " %H:%M:%S";
	}
	else
	{
		short_time = "%I:%M %p";
		long_time  = "%I:%M:%S %p";
		timestamp  = " %I:%M %p";
	}

	if (mDateFormat == 0)
	{
		short_date = "%Y-%m-%d";
		long_date  = "%A %d %B %Y";
		timestamp  = "%a %d %b %Y" + timestamp;
	}
	else if (mDateFormat == 1)
	{
		short_date = "%d/%m/%Y";
		long_date  = "%A %d %B %Y";
		timestamp  = "%a %d %b %Y" + timestamp;
	}
	else
	{
		short_date = "%m/%d/%Y";
		long_date  = "%A, %B %d %Y";
		timestamp  = "%a %b %d %Y" + timestamp;
	}

	gSavedSettings.setString("ShortDateFormat",	short_date);
	gSavedSettings.setString("LongDateFormat",	long_date);
	gSavedSettings.setString("ShortTimeFormat",	short_time);
	gSavedSettings.setString("LongTimeFormat",	long_time);
	gSavedSettings.setString("TimestampFormat",	timestamp);

	if (LLStartUp::getStartupState() == STATE_STARTED)
	{
		gSavedPerAccountSettings.setString("HighlightNicknames", childGetValue("highlight_nicknames_text"));
		gSavedPerAccountSettings.setBOOL("HighlightDisplayName", childGetValue("highlight_display_name_check"));
	}

	refreshValues();
}

//---------------------------------------------------------------------------

HBPrefsCool::HBPrefsCool()
:	impl(* new HBPrefsCoolImpl())
{
}

HBPrefsCool::~HBPrefsCool()
{
	delete &impl;
}

void HBPrefsCool::apply()
{
	impl.apply();
}

void HBPrefsCool::cancel()
{
	impl.cancel();
}

LLPanel* HBPrefsCool::getPanel()
{
	return &impl;
}
