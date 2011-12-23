/** 
 * @file RRInterface.h
 * @author Marine Kelley
 * @brief The header for all RLV features
 *
 * RLV Source Code
 * The source code in this file("Source Code") is provided by Marine Kelley
 * to you under the terms of the GNU General Public License, version 2.0
 *("GPL"), unless you have obtained a separate licensing agreement
 *("Other License"), formally executed by you and Marine Kelley.  Terms of
 * the GPL can be found in doc/GPL-license.txt in the distribution of the
 * original source of the Second Life Viewer, or online at 
 * http://secondlifegrid.net/programs/open_source/licensing/gplv2
 * 
 * By copying, modifying or distributing this software, you acknowledge
 * that you have read and understood your obligations described above,
 * and agree to abide by those obligations.
 * 
 * ALL SOURCE CODE FROM MARINE KELLEY IS PROVIDED "AS IS." MARINE KELLEY 
 * MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING 
 * ITS ACCURACY, COMPLETENESS OR PERFORMANCE.
 */

#ifndef LL_RRINTERFACE_H
#define LL_RRINTERFACE_H

#define RR_VIEWER_NAME "RestrainedLife"
#define RR_VIEWER_NAME_NEW "RestrainedLove"
#define RR_VERSION_NUM "2070420"
#define RR_VERSION "2.07.04.20"
#define RR_SLV_VERSION "Cool VL Viewer v1.26.2"

#define RR_PREFIX "@"
#define RR_SHARED_FOLDER "#RLV"
#define RR_RLV_REDIR_FOLDER_PREFIX "#RLV/~"
// Length of the "#RLV/" string constant in characters.
#define RR_HRLVS_LENGTH 5
#define RR_ADD_FOLDER_PREFIX "+"

// Define to 0 if you wish @getcommand to return @behav=force commands as
// "behav%f" in excess to their @behave=y/n version (returned as "behav").
#define STRIP_FORCE_FLAG 1

// Set this to the name of the variable used to store the avatar height offset
#define AVATARHEIGHTOFFSET "AvatarOffsetZ"

// wearable types as strings
#define WS_ALL "all"
#define WS_EYES "eyes"
#define WS_SKIN "skin"
#define WS_SHAPE "shape"
#define WS_HAIR "hair"
#define WS_GLOVES "gloves"
#define WS_JACKET "jacket"
#define WS_PANTS "pants"
#define WS_SHIRT "shirt"
#define WS_SHOES "shoes"
#define WS_SKIRT "skirt"
#define WS_SOCKS "socks"
#define WS_UNDERPANTS "underpants"
#define WS_UNDERSHIRT "undershirt"
#define WS_ALPHA "alpha"
#define WS_TATTOO "tattoo"
#define WS_PHYSICS "physics"

//#include <set>
#include <deque>
#include <map>
#include <string>

#include "lluuid.h"
#include "llframetimer.h"

#include "llchat.h"
#include "llchatbar.h"
#include "llinventorymodel.h"
#include "llviewerjointattachment.h"
#include "llviewermenu.h"
#include "llwearable.h"

extern BOOL gRRenabled;

typedef std::multimap<std::string, std::string> RRMAP;
typedef struct Command {
	LLUUID uuid;
	std::string command;
} Command;

typedef struct AssetAndTarget {
	LLUUID uuid;
	std::string attachpt;
} AssetAndTarget;

// How to call @attach:outfit=force(useful for multi-attachments and multi-wearables
typedef enum AttachHow {
	AttachHow_replace			= 0, // default behavior
	AttachHow_under				= 1, // unusued for now
	AttachHow_over				= 2, // add on top
	AttachHow_over_or_replace	= 3, // stack if the name of the outfit begins with a special sign, otherwise replace
	AttachHow_count				= 4
} AttachHow;

// Type of the lock of a folder
typedef enum FolderLock {
	FolderLock_unlocked					= 0, // not locked
	FolderLock_locked_with_except		= 1, // locked but with exception(i.e. will be treated as unlocked)
	FolderLock_locked_without_except	= 2, // locked without exception
	FolderLock_count					= 3
} FolderLock;

class RRInterface
{
public:
	RRInterface();
	~RRInterface();

	static void init();

	void refreshTPflag(bool save);
	std::string getVersion(); // returns "RestrainedLife Viewer blah blah"
	std::string getVersion2(); // returns "RestrainedLove Viewer blah blah"
	BOOL isAllowed(LLUUID object_uuid, std::string action, BOOL log_it = TRUE);
	BOOL contains(std::string action); // return TRUE if the action is contained
	BOOL containsSubstr(std::string action);
	BOOL containsWithoutException(std::string action, std::string except = ""); // return TRUE if the action or action+"_sec" is contained, and either there is no global exception, or there is no local exception if we found action+"_sec"
	bool isFolderLocked(LLInventoryCategory* cat); // return true if cat has a lock specified for it or one of its parents, or not shared and @unshared is active
	FolderLock isFolderLockedWithoutException(LLInventoryCategory* cat, std::string attach_or_detach); // attach_or_detach must be equal to either "attach" or "detach"
	FolderLock isFolderLockedWithoutExceptionAux(LLInventoryCategory* cat, std::string attach_or_detach, std::deque<std::string> list_of_restrictions); // auxiliary function to isFolderLockedWithoutException

	bool isBlacklisted(std::string command, bool force = false);
	BOOL add(LLUUID object_uuid, std::string action, std::string option);
	BOOL remove(LLUUID object_uuid, std::string action, std::string option);
	BOOL clear(LLUUID object_uuid, std::string command="");
	void replace(LLUUID what, LLUUID by);
	BOOL garbageCollector(BOOL all=TRUE); // if false, don't clear rules attached to NULL_KEY as they are issued from external objects (only cleared when changing parcel)
	std::deque<std::string> parse(std::string str, std::string sep); // utility function
	void notify(LLUUID object_uuid, std::string action, std::string suffix); // scan the list of restrictions, when finding "notify" say the action on the specified channel

	BOOL parseCommand(std::string command, std::string& behaviour, std::string& option, std::string& param);
	BOOL handleCommand(LLUUID uuid, std::string command);
	BOOL fireCommands(); // execute commands buffered while the viewer was initializing (mostly useful for force-sit as when the command is sent the object is not necessarily rezzed yet)
	BOOL force(LLUUID object_uuid, std::string command, std::string option);
	void removeWearableFromAvatar(EWearableType type);

	BOOL answerOnChat(std::string channel, std::string msg);
	std::string crunchEmote(std::string msg, U32 truncateTo = 0);

	std::string getOutfitLayerAsString(EWearableType layer);
	EWearableType getOutfitLayerAsType(std::string layer);
	std::string getOutfit(std::string layer);
	std::string getAttachments(std::string attachpt);

	std::string getStatus(LLUUID object_uuid, std::string rule); // if object_uuid is null, return all
	std::string getCommand(std::string match, bool blacklist = false);
	std::string getCommandsByType(S32 type, bool blacklist = false);
	BOOL forceDetach(std::string attachpt);
	BOOL forceDetachByUuid(std::string object_uuid);

	BOOL hasLockedHuds();
	std::deque<LLInventoryItem*> getListOfLockedItems(LLInventoryCategory* root);
	std::deque<std::string> getListOfRestrictions(LLUUID object_uuid, std::string rule = "");
	std::string getInventoryList(std::string path, BOOL withWornInfo = FALSE);
	std::string getWornItems(LLInventoryCategory* cat);
	LLInventoryCategory* getRlvShare(); // return pointer to #RLV folder or null if does not exist
	BOOL isUnderRlvShare(LLInventoryItem* item);
	BOOL isUnderRlvShare(LLInventoryCategory* cat);
	BOOL isUnderFolder(LLInventoryCategory* cat_parent, LLInventoryCategory* cat_child); // true if cat_child is a child of cat_parent
//	void renameAttachment(LLInventoryItem* item, LLViewerJointAttachment* attachment); // DEPRECATED
	LLInventoryCategory* getCategoryUnderRlvShare(std::string catName, LLInventoryCategory* root = NULL);
	LLInventoryCategory* findCategoryUnderRlvShare(std::string catName, LLInventoryCategory* root = NULL);
	std::string findAttachmentNameFromPoint(LLViewerJointAttachment* attachpt);
	LLViewerJointAttachment* findAttachmentPointFromName(std::string objectName, bool exactName = false);
	LLViewerJointAttachment* findAttachmentPointFromParentName(LLInventoryItem* item);
	S32 findAttachmentPointNumber(LLViewerJointAttachment* attachment);
	void detachObject(LLViewerObject* object);
	void detachAllObjectsFromAttachment(LLViewerJointAttachment* attachment);
	bool canDetachAllObjectsFromAttachment(LLViewerJointAttachment* attachment, BOOL handle_nostrip = TRUE);
	void fetchInventory(LLInventoryCategory* root = NULL);

	BOOL forceAttach(std::string category, BOOL recursive, AttachHow how);
	BOOL forceDetachByName(std::string category, BOOL recursive, BOOL handle_nostrip);

	BOOL getAllowCancelTp() { return mAllowCancelTp; }
	void setAllowCancelTp(BOOL newval) { mAllowCancelTp = newval; }

	std::string getParcelName() { return mParcelName; }
	void setParcelName(std::string newval) { mParcelName = newval; }

	BOOL forceTeleport(std::string location);

	std::string stringReplace(std::string s, std::string what, std::string by, BOOL caseSensitive = FALSE);

	std::string getDummyName(std::string name, EChatAudible audible = CHAT_AUDIBLE_FULLY); // return "someone", "unknown" etc according to the length of the name(when shownames is on)
	std::string getCensoredMessage(std::string str); // replace names by dummy names

	LLUUID getSitTargetId() { return mSitTargetId; }
	void setSitTargetId(LLUUID newval) { mSitTargetId = newval; }

	BOOL forceEnvironment(std::string command, std::string option); // command is "setenv_<something>", option is a list of floats(separated by "/")
	std::string getEnvironment(std::string command); // command is "getenv_<something>"
	
	std::string getLastLoadedPreset() { return mLastLoadedPreset; }
	void setLastLoadedPreset(std::string newval) { mLastLoadedPreset = newval; }

	BOOL forceDebugSetting(std::string command, std::string option); // command is "setdebug_<something>", option is a list of values(separated by "/")
	std::string getDebugSetting(std::string command); // command is "getdebug_<something>"

	std::string getFullPath(LLInventoryCategory* cat);
	std::string getFullPath(LLInventoryItem* item, std::string option = "", bool full_list = true);
	LLInventoryItem* getItemAux(LLViewerObject* attached_object, LLInventoryCategory* root);
	LLInventoryItem* getItem(LLUUID wornObjectUuidInWorld);
	void attachObjectByUUID(LLUUID assetUUID, S32 attachPtNumber = 0);

	bool canDetachAllSelectedObjects();
	bool isSittingOnAnySelectedObject();

	bool canAttachCategory(LLInventoryCategory* folder, bool with_exceptions = true);
	bool canAttachCategoryAux(LLInventoryCategory* folder, bool in_parent, bool in_no_mod, bool with_exceptions = true);
	bool canDetachCategory(LLInventoryCategory* folder, bool with_exceptions = true);
	bool canDetachCategoryAux(LLInventoryCategory* folder, bool in_parent, bool in_no_mod, bool with_exceptions = true);
	bool canUnwear(LLViewerInventoryItem* item, BOOL handle_nostrip = TRUE);
	bool canUnwear(EWearableType type, BOOL handle_nostrip = TRUE);
	bool canWear(LLViewerInventoryItem* item, BOOL handle_nostrip = TRUE);
	bool canWear(EWearableType type, bool from_server = false);
	bool canDetach(LLViewerInventoryItem* item, BOOL handle_nostrip = TRUE);
	bool canDetach(LLViewerObject* attached_object, BOOL handle_nostrip = TRUE);
	bool canDetach(std::string attachpt, BOOL handle_nostrip = TRUE);
	bool canAttach(LLViewerObject* object_to_attach, std::string attachpt, bool from_server = false);
	bool canAttach(LLViewerInventoryItem* item);
	bool canEdit(LLViewerObject* object);
	bool canTouch(LLViewerObject* object, LLVector3 pick_intersection = LLVector3::zero); // set pick_intersection to force the check on this position
	bool canTouchFar(LLViewerObject* object, LLVector3 pick_intersection = LLVector3::zero); // set pick_intersection to force the check on this position

public:
	enum RRBehaviourType {
		RR_INFO,				// Information commands, not-blacklistable.
		RR_MISCELLANEOUS,		// Miscellaneous not-blacklistable commands.
		RR_INSTANTMESSAGE,		// Instant Messaging commands.
		RR_SENDCHAT,			// Chat sending commands.
		RR_RECEIVECHAT,			// Chat receiving commands.
		RR_CHANNEL,				// Chat on private channels commands.
		RR_EMOTE,				// Emote/pose commands.
		RR_REDIRECTION,			// Emote/pose redirection commands.
		RR_MOVE,				// Movement commands.
		RR_SIT,					// Sitting/unsitting commands.
		RR_TELEPORT,			// Teleportation commands.
		RR_TOUCH,				// Touch commands.
		RR_LOCK,				// Locking/unlocking commands.
		RR_ATTACH,				// Attach/wear commands.
		RR_DETACH,				// Detach/remove commands.
		RR_INVENTORY,			// Inventory commands.
		RR_INVENTORYLOCK,		// Inventory locking commands.
		RR_BUILD,				// Rezing/editing commands.
		RR_LOCATION,			// Location commands.
		RR_NAME,				// Name commands.
		RR_GROUP,				// Group commands.
		RR_PERM,				// Permissions/extra-restriction commands.
		RR_DEBUG,				// Debug settings commands.
		RR_ENVIRONMENT,			// Environment/rendering commands.
	};

	typedef std::pair<std::string, S32> rr_command_entry_t;
	typedef std::map<std::string, S32 > rr_command_map_t;
	static rr_command_map_t sCommandsMap;

	static BOOL sRRNoSetEnv;
	static BOOL sRestrainedLoveDebug;
	static BOOL sCanOoc; // when TRUE, the user can bypass a sendchat restriction by surrounding with (( and ))
	static BOOL sUntruncatedEmotes; // when TRUE, the user's emotes are never truncated.
	static std::string sBlackList;		// user-blacklisted RestrainedLove commands.
	static std::string sRecvimMessage;	// message to replace an incoming IM, when under recvim
	static std::string sSendimMessage;	// message to replace an outgoing IM, when under sendim
	static std::string sRolePlayBlackList;	// standard blacklist for role-players
	static std::string sVanillaBlackList;	// standard blacklist for non-BDSM folks

	// Some cache variables to accelerate common checks
	BOOL mHasLockedHuds;
	BOOL mContainsDetach;
	BOOL mContainsShowinv;
	BOOL mContainsUnsit;
	BOOL mContainsFartouch;
	BOOL mContainsShowworldmap;
	BOOL mContainsShowminimap;
	BOOL mContainsShowloc;
	BOOL mContainsShownames;
	BOOL mContainsSetenv;
	BOOL mContainsSetdebug;
	BOOL mContainsFly;
	BOOL mContainsEdit;
	BOOL mContainsRez;
	BOOL mContainsShowhovertextall;
	BOOL mContainsShowhovertexthud;
	BOOL mContainsShowhovertextworld;
	BOOL mContainsDefaultwear;
	BOOL mContainsPermissive;
	BOOL mContainsRun;
	BOOL mContainsAlwaysRun;

	// Allowed debug settings(initialized in the ctor)
	std::string mAllowedU32;
	std::string mAllowedS32;
	std::string mAllowedF32;
	std::string mAllowedBOOLEAN;
	std::string mAllowedSTRING;
	std::string mAllowedVEC3;
	std::string mAllowedVEC3D;
	std::string mAllowedRECT;
	std::string mAllowedCOL4;
	std::string mAllowedCOL3;
	std::string mAllowedCOL4U;

	// These should be private but we may want to browse them from the outside world, so let's keep them public
	RRMAP mSpecialObjectBehaviours;
	std::deque<Command> mRetainedCommands;

	// When a locked attachment is kicked off by another one with llAttachToAvatar() in a script, retain its UUID here, to reattach it later 
	std::deque<AssetAndTarget> mAssetsToReattach;
	LLFrameTimer mReattachTimer; // Reset each time a locked attachment is kicked by a "Wear", and on auto-reattachment timeout.
	BOOL mReattaching; // TRUE when llappviewer.cpp asked for a reattachment. FALSE when llviewerjointattachment.cpp detected a reattachment.
	BOOL mReattachTimeout; // TRUE when llappviewer.cpp detects a reattachment timeout, FALSE when llviewerjointattachment.cpp detected a reattachment.
	AssetAndTarget mJustDetached; // we need this to inhibit the removeObject event that occurs right after addObject in the case of a replacement
	AssetAndTarget mJustReattached; // we need this to inhibit the removeObject event that occurs right after addObject in the case of a replacement
	LLVector3d mLastStandingLocation; // this is the global position we had when we sat down on something, and we will be teleported back there when we stand up if we are prevented from "sit-tp by rezzing stuff"
	BOOL mSnappingBackToLastStandingLocation; // TRUE when we are teleporting back to the last standing location, in order to bypass the usual checks

private:
	std::string mParcelName; // for convenience (gAgent does not retain the name of the current parcel)
	BOOL mInventoryFetched; // FALSE at first, used to fetch RL Share inventory once upon login
	BOOL mAllowCancelTp; // TRUE unless forced to TP with @tpto (=> receive TP order from server, act like it is a lure from a Linden => don't show the cancel button)
	LLUUID mSitTargetId;
	std::string mLastLoadedPreset; // contains the name of the latest loaded Windlight preset
	U32 mLaunchTimestamp; // timestamp of the beginning of this session
};

#endif
