/**
 * @file llfloatermodeluploadbase.cpp
 * @brief LLFloaterUploadModelBase class definition
 *
 * $LicenseInfo:firstyear=2011&license=viewergpl$
 * 
 * Copyright (c) 2011, Linden Research, Inc.
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

#include "llfloatermodeluploadbase.h"

#include "llnotifications.h"

#include "llagent.h"
#include "llviewerregion.h"

LLFloaterModelUploadBase::LLFloaterModelUploadBase(const std::string& name)
:	LLFloater(name),
	mHasUploadPerm(false)
{
}

void LLFloaterModelUploadBase::requestAgentUploadPermissions()
{
	std::string capability = "MeshUploadFlag";
	std::string url = gAgent.getRegion()->getCapability(capability);

	if (!url.empty())
	{
		llinfos<< typeid(*this).name() <<"::requestAgentUploadPermissions() requesting for upload model permissions from: "<< url <<llendl;
		LLHTTPClient::get(url, new LLUploadModelPremissionsResponder(getPermObserverHandle()));
	}
	else
	{
		LLSD args;
		args["CAPABILITY"] = capability;
		LLNotifications::instance().add("RegionCapabilityRequestError", args);
		// BAP HACK avoid being blocked by broken server side stuff
		mHasUploadPerm = true;
	}
}
