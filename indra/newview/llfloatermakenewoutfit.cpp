/** 
 * @file llfloatermakenewoutfit.cpp
 * @brief The Make New Outfit floater implementation
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

#include "llfloatermakenewoutfit.h"

#include "lluictrlfactory.h" 

#include "llagent.h"
#include "llstartup.h"			// for gIsInSecondLife
#include "llviewercontrol.h"
#include "llvoavatar.h"
#include "llwearable.h"

LLFloaterMakeNewOutfit* LLFloaterMakeNewOutfit::sInstance = NULL;

LLFloaterMakeNewOutfit::LLFloaterMakeNewOutfit()
:	LLFloater(std::string("make new outfit")),
	mIsDirty(true)
{
    LLUICtrlFactory::getInstance()->buildFloater(this, "floater_make_new_outfit.xml");
}

//virtual
LLFloaterMakeNewOutfit::~LLFloaterMakeNewOutfit()
{
    sInstance = NULL;
}

//virtual
BOOL LLFloaterMakeNewOutfit::postBuild()
{
	// Build list of check boxes
	// NOTE: floater_make_new_outfit.xml needs to be updated if wearables or
	//		 attachments are added or their names are changed !

	// Wearables
	for (S32 i = 0; i < WT_COUNT; i++)
	{
		std::string name = "checkbox_" + LLWearable::typeToTypeLabel((EWearableType)i);
		mCheckBoxList.push_back(std::make_pair(name, i));
		childSetCommitCallback(name, onCommitCheckBox, this);
	}

	// Attachment points
	LLVOAvatar* avatar = gAgent.getAvatarObject();
	if (avatar)
	{
		LLVOAvatar::attachment_map_t::iterator iter;
		for (iter = avatar->mAttachmentPoints.begin();
			 iter != avatar->mAttachmentPoints.end(); )
		{
			LLVOAvatar::attachment_map_t::iterator curiter = iter++;
			LLViewerJointAttachment* attachment = curiter->second;
			S32	attachment_pt = curiter->first;	
			std::string name = "checkbox_" + attachment->getName();
			mCheckBoxList.push_back(std::make_pair(name, attachment_pt));
			childSetCommitCallback(name, onCommitCheckBox, this);
		}
	}

	if (!gIsInSecondLife && !gSavedSettings.getBOOL("OSAllowInventoryLinks"))
	{
		childSetEnabled("checkbox_use_links", false);
		childSetValue("checkbox_use_links", FALSE);
	}

	childSetAction("Save", onButtonSave, this); 
	childSetAction("Cancel", onButtonCancel, this); 
	return TRUE;
}

//virtual
void LLFloaterMakeNewOutfit::draw()
{
	if (mIsDirty)
	{
		// Update wearable check boxes
		for (S32 i = 0; i < WT_COUNT; i++)
		{
			std::string name = "checkbox_" + LLWearable::typeToTypeLabel((EWearableType)i);
			bool was_enabled = childIsEnabled(name);
			bool enabled = (gAgent.getWearable((EWearableType)i) != NULL);
			BOOL selected = childGetValue(name).asBoolean();
			if (was_enabled != enabled)
			{
				selected = (BOOL)enabled;
			}
			if (gAgent.isTeen() && ((EWearableType)i == WT_UNDERSHIRT ||
									(EWearableType)i == WT_UNDERPANTS))
			{
				enabled = false;
				selected = FALSE;
				// hide wearable check boxes that don't apply to teen accounts
				childSetVisible(name, false);
			}
			childSetEnabled(name, enabled);
			childSetValue(name, selected);
		}

		// Update attachment check boxes
		LLVOAvatar* avatar = gAgent.getAvatarObject();
		if (avatar)
		{
			LLVOAvatar::attachment_map_t::iterator iter;
			for (iter = avatar->mAttachmentPoints.begin();
				 iter != avatar->mAttachmentPoints.end(); )
			{
				LLVOAvatar::attachment_map_t::iterator curiter = iter++;
				LLViewerJointAttachment* attachment = curiter->second;
				std::string name = "checkbox_" + attachment->getName();
				bool was_enabled = childIsEnabled(name);
				bool enabled = (attachment->getNumObjects() > 0);
				BOOL selected = childGetValue(name).asBoolean();
				if (was_enabled != enabled)
				{
					selected = (BOOL)enabled;
				}
				childSetEnabled(name, enabled);
				childSetValue(name, selected);
			}
		}

		// Force a refresh of the Save button status
		onCommitCheckBox(NULL, (void*)this);

		mIsDirty = false;
	}

	LLFloater::draw();
}

void LLFloaterMakeNewOutfit::getIncludedItems(LLDynamicArray<S32> &wearables_to_include,
											  LLDynamicArray<S32> &attachments_to_include)
{
	for (S32 i = 0; i < (S32)mCheckBoxList.size(); i++)
	{
		std::string name = mCheckBoxList[i].first;
		BOOL checked = childGetValue(name).asBoolean();
		if (i < WT_COUNT)
		{
			if (checked)
			{
				wearables_to_include.put(i);
			}
		}
		else
		{
			if (checked)
			{
				S32 attachment_pt = mCheckBoxList[i].second;
				attachments_to_include.put(attachment_pt);
			}
		}
	}
}

//static
void LLFloaterMakeNewOutfit::showInstance()
{
	if (!sInstance)
	{
		sInstance = new LLFloaterMakeNewOutfit();
 	}
	sInstance->open();
}

//static
void LLFloaterMakeNewOutfit::setDirty()
{
    if (sInstance)
	{
		sInstance->mIsDirty = true;
	}
}

//static
void LLFloaterMakeNewOutfit::onCommitCheckBox(LLUICtrl* ctrl, void* data)
{
	LLFloaterMakeNewOutfit* self = (LLFloaterMakeNewOutfit*)data;
	if (!self) return;

	// Refresh the Save button status (enabled if and only if at least one
	// check box is checked).
	bool enable_save = false;
	for (S32 i = 0; i < (S32)self->mCheckBoxList.size(); i++)
	{
		if (self->childGetValue(self->mCheckBoxList[i].first).asBoolean())
		{
			enable_save = true;
			break;
		}
	}
	self->childSetEnabled("Save", enable_save);
}

//static
void LLFloaterMakeNewOutfit::onButtonSave(void* data)
{
	LLFloaterMakeNewOutfit* self = (LLFloaterMakeNewOutfit*)data;
	if (self)
	{
		std::string folder = self->childGetValue("name ed").asString();
		BOOL rename_clothing = self->childGetValue("rename").asBoolean();
		LLDynamicArray<S32> wearables, attachments;
		self->getIncludedItems(wearables, attachments);
		gAgent.makeNewOutfit(folder, wearables, attachments, rename_clothing);
		self->close();
	}
}

//static
void LLFloaterMakeNewOutfit::onButtonCancel(void* data)
{
	LLFloaterMakeNewOutfit* self = (LLFloaterMakeNewOutfit*)data;
	if (self)
	{
		self->close();
	}
}
