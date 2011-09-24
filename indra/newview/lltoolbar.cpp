/** 
 * @file lltoolbar.cpp
 * @author James Cook, Richard Nelson
 * @brief Large friendly buttons at bottom of screen.
 *
 * $LicenseInfo:firstyear=2002&license=viewergpl$
 * 
 * Copyright (c) 2002-2009, Linden Research, Inc.
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
 * online at
 * http://secondlifegrid.net/programs/open_source/licensing/flossexception
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

#include "lltoolbar.h"

#include "imageids.h"
#include "llfontgl.h"
#include "llrect.h"
#include "llparcel.h"

#include "llagent.h"
#include "llbutton.h"
#include "llfocusmgr.h"
#include "llviewercontrol.h"
#include "llmenucommands.h"
#include "llimview.h"
#include "lluiconstants.h"
#include "llvoavatar.h"
#include "lltooldraganddrop.h"
#include "llinventoryview.h"
#include "llfloateravatarlist.h"
#include "llfloaterchatterbox.h"
#include "llfloaterfriends.h"
#include "llfloatergroups.h"
#include "llfloatersnapshot.h"
#include "lltoolmgr.h"
#include "llui.h"
#include "llviewermenu.h"
#include "llfirstuse.h"
#include "llviewerparcelmgr.h"
#include "lluictrlfactory.h"
#include "llviewerwindow.h"
#include "lltoolgrab.h"

#if LL_DARWIN

	#include "llresizehandle.h"

	// This class draws like an LLResizeHandle but has no interactivity.
	// It's just there to provide a cue to the user that the lower right corner of the window functions as a resize handle.
	class LLFakeResizeHandle : public LLResizeHandle
	{
	public:
		LLFakeResizeHandle(const std::string& name, const LLRect& rect, S32 min_width, S32 min_height, ECorner corner = RIGHT_BOTTOM )
		: LLResizeHandle(name, rect, min_width, min_height, corner )
		{
			
		}

		virtual BOOL	handleHover(S32 x, S32 y, MASK mask)   { return FALSE; };
		virtual BOOL	handleMouseDown(S32 x, S32 y, MASK mask)  { return FALSE; };
		virtual BOOL	handleMouseUp(S32 x, S32 y, MASK mask)   { return FALSE; };

	};

#endif // LL_DARWIN

//
// Globals
//

LLToolBar *gToolBar = NULL;
S32 TOOL_BAR_HEIGHT = 20;

//
// Statics
//
F32	LLToolBar::sInventoryAutoOpenTime = 1.f;

//
// Functions
//

LLToolBar::LLToolBar(const std::string& name, const LLRect& r)
:	LLPanel(name, r, BORDER_NO)
#if LL_DARWIN
	, mResizeHandle(NULL)
#endif // LL_DARWIN
{
	setIsChrome(TRUE);
	setFollows(FOLLOWS_LEFT | FOLLOWS_RIGHT | FOLLOWS_BOTTOM);
	LLUICtrlFactory::getInstance()->buildPanel(this, "panel_toolbar.xml");
	setFocusRoot(TRUE);
}


BOOL LLToolBar::postBuild()
{
	childSetAction("communicate_btn", onClickCommunicate, this);
	childSetControlName("communicate_btn", "ShowCommunicate");

	childSetAction("chat_btn", onClickChat, this);
	childSetControlName("chat_btn", "ChatVisible");

	childSetAction("friends_btn", onClickFriends, this);
	childSetControlName("friends_btn", "ShowFriends");

	childSetAction("groups_btn", onClickGroups, this);
	childSetControlName("groups_btn", "ShowGroups");

	childSetAction("fly_btn", onClickFly, this);
	childSetControlName("fly_btn", "FlyBtnState");

	childSetAction("snapshot_btn", onClickSnapshot, this);
	childSetControlName("snapshot_btn", "");

	childSetAction("directory_btn", onClickDirectory, this);
	childSetControlName("directory_btn", "ShowDirectory");

	childSetAction("build_btn", onClickBuild, this);
	childSetControlName("build_btn", "BuildBtnState");

	childSetAction("minimap_btn", onClickMiniMap, this);
	childSetControlName("minimap_btn", "ShowMiniMap");

	childSetAction("radar_btn", onClickRadar, this);
	childSetControlName("radar_btn", "ShowRadar");

	childSetAction("map_btn", onClickMap, this);
	childSetControlName("map_btn", "ShowWorldMap");

	childSetAction("inventory_btn", onClickInventory, this);
	childSetControlName("inventory_btn", "ShowInventory");
#if 0
	childSetAction("appearance_btn", onClickAppearance, this);
	childSetControlName("appearance_btn", "");

	childSetAction("sit_btn", onClickSit, this);
	childSetControlName("sit_btn", "SitBtnState");
#endif

	for (child_list_const_iter_t child_iter = getChildList()->begin();
		 child_iter != getChildList()->end(); ++child_iter)
	{
		LLView *view = *child_iter;
		LLButton* buttonp = dynamic_cast<LLButton*>(view);
		if(buttonp)
		{
			buttonp->setSoundFlags(LLView::SILENT);
		}
	}

#if LL_DARWIN
	if(mResizeHandle == NULL)
	{
		LLRect rect(0, 0, RESIZE_HANDLE_WIDTH, RESIZE_HANDLE_HEIGHT);
		mResizeHandle = new LLFakeResizeHandle(std::string(""), rect, RESIZE_HANDLE_WIDTH, RESIZE_HANDLE_HEIGHT);
		this->addChildAtEnd(mResizeHandle);
	}
#endif // LL_DARWIN

	layoutButtons();

	return TRUE;
}

LLToolBar::~LLToolBar()
{
	// LLView destructor cleans up children
}


BOOL LLToolBar::handleDragAndDrop(S32 x, S32 y, MASK mask, BOOL drop,
									 EDragAndDropType cargo_type,
									 void* cargo_data,
									 EAcceptance* accept,
									 std::string& tooltip_msg)
{
	LLButton* inventory_btn = getChild<LLButton>("inventory_btn");
	if (!inventory_btn) return FALSE;

	LLInventoryView* active_inventory = LLInventoryView::getActiveInventory();

	if(active_inventory && active_inventory->getVisible())
	{
		mInventoryAutoOpen = FALSE;
	}
	else if (inventory_btn->getRect().pointInRect(x, y))
	{
		if (mInventoryAutoOpen)
		{
			if (!(active_inventory && active_inventory->getVisible()) && 
			mInventoryAutoOpenTimer.getElapsedTimeF32() > sInventoryAutoOpenTime)
			{
				LLInventoryView::showAgentInventory();
			}
		}
		else
		{
			mInventoryAutoOpen = TRUE;
			mInventoryAutoOpenTimer.reset();
		}
	}

	return LLPanel::handleDragAndDrop(x, y, mask, drop, cargo_type, cargo_data, accept, tooltip_msg);
}

// static
void LLToolBar::toggle(void*)
{
	BOOL show = gSavedSettings.getBOOL("ShowToolBar");                      
	gSavedSettings.setBOOL("ShowToolBar", !show);                           
	gToolBar->setVisible(!show);
}


// static
BOOL LLToolBar::visible(void*)
{
	return gToolBar->getVisible();
}


void LLToolBar::layoutButtons()
{
	// Always spans whole window. JC                                        
	const S32 FUDGE_WIDTH_OF_SCREEN = 4;                                    
	const S32 PAD = 2;
	S32 width = gViewerWindow->getWindowWidth() + FUDGE_WIDTH_OF_SCREEN;    
	S32 count = getChildCount();
	if (!count) return;
	BOOL show = gSavedSettings.getBOOL("ShowChatButton");
	childSetVisible("chat_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowIMButton");
	childSetVisible("communicate_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowFriendsButton");
	childSetVisible("friends_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowGroupsButton");
	childSetVisible("groups_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowFlyButton");
	childSetVisible("fly_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowSnapshotButton");
	childSetVisible("snapshot_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowSearchButton");
	childSetVisible("directory_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowBuildButton");
	childSetVisible("build_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowRadarButton");
	childSetVisible("radar_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowMiniMapButton");
	childSetVisible("minimap_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowMapButton");
	childSetVisible("map_btn", show);
	if (!show) count--;
	show = gSavedSettings.getBOOL("ShowInventoryButton");
	childSetVisible("inventory_btn", show);
	if (!show) count--;

	if (count < 1)
	{
		// No button in the toolbar !  Hide it.
		gSavedSettings.setBOOL("ShowToolBar", FALSE);
		return;
	}

#if LL_DARWIN
	// this function may be called before postBuild(), in which case mResizeHandle won't have been set up yet.
	if(mResizeHandle != NULL)
	{
		// a resize handle has been added as a child, increasing the count by one.
		count--;
		
		if(!gViewerWindow->getWindow()->getFullscreen())
		{
			// Only when running in windowed mode on the Mac, leave room for a resize widget on the right edge of the bar.
			width -= RESIZE_HANDLE_WIDTH;

			LLRect r;
			r.mLeft = width - PAD;
			r.mBottom = 0;
			r.mRight = r.mLeft + RESIZE_HANDLE_WIDTH;
			r.mTop = r.mBottom + RESIZE_HANDLE_HEIGHT;
			mResizeHandle->setRect(r);
			mResizeHandle->setVisible(TRUE);
		}
		else
		{
			mResizeHandle->setVisible(FALSE);
		}
	}
#endif // LL_DARWIN

	// We actually want to extend "PAD" pixels off the right edge of the    
	// screen, such that the rightmost button is aligned.                   
	F32 segment_width = (F32)(width + PAD) / (F32)count;                    
	S32 btn_width = lltrunc(segment_width - PAD);                           

	// Evenly space all views
	S32 height = -1;
	S32 i = count - 1;
	for (child_list_const_iter_t child_iter = getChildList()->begin();
		 child_iter != getChildList()->end(); ++child_iter)
	{
		LLView *btn_view = *child_iter;
		LLButton* buttonp = dynamic_cast<LLButton*>(btn_view);
		if (buttonp && btn_view->getVisible())
		{
			if (height < 0)
			{
				height = btn_view->getRect().getHeight();
			}
			S32 x = llround(i*segment_width);                               
			S32 y = 0;                                                      
			LLRect r;                                                               
			r.setOriginAndSize(x, y, btn_width, height);                    
			btn_view->setRect(r);                                           
			i--;                                                            
		}
	}                                                                       
}


// virtual
void LLToolBar::reshape(S32 width, S32 height, BOOL called_from_parent)
{
	LLPanel::reshape(width, height, called_from_parent);

	layoutButtons();
}


// Per-frame updates of visibility
void LLToolBar::refresh()
{
	static S32 previous_visibility = -1; // Ensure we use setVisible() on startup
	static LLCachedControl<bool> show_toolbar(gSavedSettings, "ShowToolBar", true);
	bool visible = show_toolbar && !gAgent.cameraMouselook();

	if (previous_visibility != (S32)visible)
	{
		setVisible(visible);
		if (visible)
		{
			// In case there would be no button to show,
			// it would re-hide the toolbar (on next frame)
			layoutButtons();
		}
		previous_visibility = (S32)visible;
	}
	if (!visible) return;

	BOOL sitting = FALSE;
	if (gAgent.getAvatarObject())
	{
		sitting = gAgent.getAvatarObject()->mIsSitting;
	}
	childSetEnabled("fly_btn", (gAgent.canFly() || gAgent.getFlying()) && !sitting);
 
//MK
	if (gRRenabled)
	{
		childSetEnabled("build_btn", LLViewerParcelMgr::getInstance()->agentCanBuild() && !gAgent.mRRInterface.mContainsRez && !gAgent.mRRInterface.mContainsEdit);
		childSetEnabled("minimap_btn", !gAgent.mRRInterface.mContainsShowminimap);
		childSetEnabled("radar_btn", !gAgent.mRRInterface.mContainsShownames);
		childSetEnabled("map_btn", !gAgent.mRRInterface.mContainsShowworldmap && !gAgent.mRRInterface.mContainsShowloc);
		childSetEnabled("inventory_btn", !gAgent.mRRInterface.mContainsShowinv);
	}
	else
//mk
		childSetEnabled("build_btn", LLViewerParcelMgr::getInstance()->agentCanBuild());

	// Check to see if we're in build mode
	BOOL build_mode = LLToolMgr::getInstance()->inEdit();
	// And not just clicking on a scripted object
	if (LLToolGrab::getInstance()->getHideBuildHighlight())
	{
		build_mode = FALSE;
	}
	gSavedSettings.setBOOL("BuildBtnState", build_mode);
}


// static
void LLToolBar::onClickCommunicate(void* user_data)
{
	LLFloaterChatterBox::toggleInstance(LLSD());
}


// static
void LLToolBar::onClickChat(void* user_data)
{
	handle_chat(NULL);
}

// static
void LLToolBar::onClickFly(void*)
{
	gAgent.toggleFlying();
}

#if 0
// static
void LLToolBar::onClickAppearance(void*)
{
	if (gAgent.areWearablesLoaded())
	{
		gAgent.changeCameraToCustomizeAvatar();
	}
}

// static
void LLToolBar::onClickSit(void*)
{
	if (!(gAgent.getControlFlags() & AGENT_CONTROL_SIT_ON_GROUND))
	{
		// sit down
		gAgent.setFlying(FALSE);
		gAgent.setControlFlags(AGENT_CONTROL_SIT_ON_GROUND);

		// Might be first sit
		LLFirstUse::useSit();
	}
	else
	{
		// stand up
		gAgent.setFlying(FALSE);
//MK
		if (gRRenabled && gAgent.mRRInterface.mContainsUnsit)
		{
			return;
		}
//mk
		gAgent.setControlFlags(AGENT_CONTROL_STAND_UP);
//MK
		if (gRRenabled && gAgent.mRRInterface.contains ("standtp"))
		{
			gAgent.mRRInterface.mSnappingBackToLastStandingLocation = TRUE;
			gAgent.teleportViaLocationLookAt (gAgent.mRRInterface.mLastStandingLocation);
			gAgent.mRRInterface.mSnappingBackToLastStandingLocation = FALSE;
		}
//mk
	}
}
#endif

// static
void LLToolBar::onClickSnapshot(void*)
{
	LLFloaterSnapshot::show (0);
}


// static
void LLToolBar::onClickDirectory(void*)
{
	handle_find(NULL);
}


// static
void LLToolBar::onClickBuild(void*)
{
	toggle_build_mode();
}


// static
void LLToolBar::onClickMiniMap(void*)
{
	handle_mini_map(NULL);
}


// static
void LLToolBar::onClickRadar(void*)
{
	LLFloaterAvatarList::toggle(NULL);
}


// static
void LLToolBar::onClickMap(void*)
{
	handle_map(NULL);
}


// static
void LLToolBar::onClickFriends(void*)
{
	LLFloaterFriends::toggle();
}


// static
void LLToolBar::onClickGroups(void*)
{
	LLFloaterGroups::toggle();
}


// static
void LLToolBar::onClickInventory(void*)
{
	handle_inventory(NULL);
}

