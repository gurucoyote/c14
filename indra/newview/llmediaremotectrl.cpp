/** 
 * @file llmediaremotectrl.cpp
 * @brief A remote control for media (video and music)
 *
 * $LicenseInfo:firstyear=2005&license=viewergpl$
 * 
 * Copyright (c) 2005-2009, Linden Research, Inc.
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

#include "llmediaremotectrl.h"

#include "llaudioengine.h"
#include "lliconctrl.h"
#include "llmimetypes.h"
#include "lloverlaybar.h"
#include "llviewermedia.h"
#include "llviewerparcelmedia.h"
#include "llviewerparcelmgr.h"
#include "lluictrlfactory.h"
#include "llpanelaudiovolume.h"
#include "llparcel.h"
#include "llviewercontrol.h"
#include "llbutton.h"

////////////////////////////////////////////////////////////////////////////////
//
//

static LLRegisterWidget<LLMediaRemoteCtrl> r("media_remote");

LLMediaRemoteCtrl::LLMediaRemoteCtrl(const std::string& name,
									 const LLRect& rect,
									 const std::string& xml_file,
									 const ERemoteType type)
:	LLPanel(name, rect, FALSE),
	mType(type)
{
	setIsChrome(TRUE);
	setFocusRoot(TRUE);

	LLUICtrlFactory::getInstance()->buildPanel(this, xml_file);
}

BOOL LLMediaRemoteCtrl::postBuild()
{
	if (mType == REMOTE_MEDIA)
	{
		childSetAction("media_play", LLOverlayBar::mediaPlay, this);
		childSetAction("media_stop", LLOverlayBar::mediaStop, this);
		childSetAction("media_pause", LLOverlayBar::mediaPause, this);
	}
	else if (mType == REMOTE_MUSIC)
	{
		childSetAction("music_play", LLOverlayBar::musicPlay, this);
		childSetAction("music_stop", LLOverlayBar::musicStop, this);
		childSetAction("music_pause", LLOverlayBar::musicPause, this);
	}
	else if (mType == REMOTE_VOLUME)
	{
		childSetAction("volume", LLOverlayBar::toggleAudioVolumeFloater, this);
		//childSetControlName("volume", "ShowAudioVolume");	// Set in XML file
	}

	return TRUE;
}

void LLMediaRemoteCtrl::draw()
{
	LLParcel* parcel = LLViewerParcelMgr::getInstance()->getAgentParcel();

	if (mType == REMOTE_MEDIA)
	{
		bool media_play_enabled = false;
		bool media_stop_enabled = false;
		bool media_show_pause = false;

		LLColor4 media_icon_color = LLUI::sColorsGroup->getColor("IconDisabledColor");
		std::string media_type = "none/none";
		std::string media_url;

		static LLCachedControl<bool> audio_streaming_video(gSavedSettings, "AudioStreamingVideo");
		if (gOverlayBar && audio_streaming_video && parcel && parcel->getMediaURL()[0])
		{
			media_play_enabled = true;
			media_icon_color = LLUI::sColorsGroup->getColor("IconEnabledColor");
			media_type = parcel->getMediaType();
			media_url = parcel->getMediaURL();

			LLViewerMediaImpl::EMediaStatus status = LLViewerParcelMedia::getStatus();
			switch (status)
			{
				case LLViewerMediaImpl::MEDIA_NONE:
					media_show_pause = false;
					media_stop_enabled = false;
					break;
				case LLViewerMediaImpl::MEDIA_LOADING:
				case LLViewerMediaImpl::MEDIA_LOADED:
				case LLViewerMediaImpl::MEDIA_PLAYING:
					// HACK: only show the pause button for movie types
					media_show_pause = LLMIMETypes::widgetType(parcel->getMediaType()) == "movie" ? true : false;
					media_stop_enabled = true;
					media_play_enabled = false;
					break;
				case LLViewerMediaImpl::MEDIA_PAUSED:
					media_show_pause = false;
					media_stop_enabled = true;
					break;
				default:
					// inherit defaults above
					break;
			}
		}

		childSetEnabled("media_play", media_play_enabled);
		childSetEnabled("media_stop", media_stop_enabled);
		childSetEnabled("media_pause", media_show_pause);
		childSetVisible("media_pause", media_show_pause);
		childSetVisible("media_play", !media_show_pause);

		const std::string media_icon_name = LLMIMETypes::findIcon(media_type);
		LLIconCtrl* media_icon = getChild<LLIconCtrl>("media_icon");
		if (media_icon && !media_icon_name.empty())
		{
			media_icon->setImage(media_icon_name);
		}
		childSetColor("media_icon", media_icon_color);
		if (media_url.empty())
		{
			media_url = getString("play_label");
		}
		else
		{
			media_url = getString("play_label") + " (" + media_url + ")";
		}
		childSetToolTip("media_play", media_url);
	}
	else if (mType == REMOTE_MUSIC)
	{
		bool music_play_enabled = false;
		bool music_stop_enabled = false;
		bool music_show_pause = false;

		LLColor4 music_icon_color = LLUI::sColorsGroup->getColor("IconDisabledColor");
		std::string music_url;

		static LLCachedControl<bool> audio_streaming_music(gSavedSettings, "AudioStreamingMusic");
		if (gOverlayBar && gAudiop && audio_streaming_music &&
			parcel && !parcel->getMusicURL().empty())
		{
			music_url = parcel->getMusicURL();
			music_play_enabled = true;
			music_icon_color = LLUI::sColorsGroup->getColor("IconEnabledColor");

			if (gOverlayBar->musicPlaying())
			{
				music_show_pause = true;
				music_stop_enabled = true;
			}
			else
			{
				music_show_pause = false;
				music_stop_enabled = false;
			}
		}

		childSetEnabled("music_play", music_play_enabled);
		childSetEnabled("music_stop", music_stop_enabled);
		childSetEnabled("music_pause", music_show_pause);
		childSetVisible("music_pause", music_show_pause);
		childSetVisible("music_play", !music_show_pause);

		childSetColor("music_icon", music_icon_color);
		if (music_url.empty())
		{
			music_url = getString("play_label");
		}
		else
		{
			music_url = getString("play_label") + " (" + music_url + ")";
		}
		childSetToolTip("music_play", music_url);
	}

	LLPanel::draw();
}
