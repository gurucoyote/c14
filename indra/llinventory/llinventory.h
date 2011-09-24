/** 
 * @file llinventory.h
 * @brief LLInventoryItem and LLInventoryCategory class declaration.
 *
 * $LicenseInfo:firstyear=2001&license=viewergpl$
 * 
 * Copyright (c) 2001-2009, Linden Research, Inc.
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

#ifndef LL_LLINVENTORY_H
#define LL_LLINVENTORY_H

#include <functional>

#include "llassetstorage.h"
#include "lldarray.h"
#include "llfoldertype.h"
#include "llinventorytype.h"
#include "llmemtype.h"
#include "llpermissions.h"
#include "llrefcount.h"
#include "llsaleinfo.h"
#include "llsd.h"
#include "lluuid.h"
#include "llxmlnode.h"

class LLMessageSystem;

// consts for Key field in the task inventory update message
extern const U8 TASK_INVENTORY_ITEM_KEY;
extern const U8 TASK_INVENTORY_ASSET_KEY;

// anonymous enumeration to specify a max inventory buffer size for
// use in packBinaryBucket()
enum
{
	MAX_INVENTORY_BUFFER_SIZE = 1024
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLInventoryObject
//
//   Base class for anything in the user's inventory.   Handles the common code 
//   between items and categories. 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class LLInventoryObject : public LLRefCount
{
public:
	typedef std::list<LLPointer<LLInventoryObject> > object_list_t;

	//--------------------------------------------------------------------
	// Initialization
	//--------------------------------------------------------------------
public:
	MEM_TYPE_NEW(LLMemType::MTYPE_INVENTORY);
	LLInventoryObject();
	LLInventoryObject(const LLUUID& uuid, 
					  const LLUUID& parent_uuid,
					  LLAssetType::EType type, 
					  const std::string& name);
	void copyObject(const LLInventoryObject* other); // LLRefCount requires custom copy
protected:
	virtual ~LLInventoryObject();
	
	//--------------------------------------------------------------------
	// Accessors
	//--------------------------------------------------------------------
public:
	virtual const LLUUID& getUUID() const; // inventoryID that this item points to
	virtual const LLUUID& getLinkedUUID() const; // inventoryID that this item points to, else this item's inventoryID
	const LLUUID& getParentUUID() const;
	virtual const std::string& getName() const;
	virtual LLAssetType::EType getType() const;
	LLAssetType::EType getActualType() const; // bypasses indirection for linked items
	BOOL getIsLinkType() const;
	
	//--------------------------------------------------------------------
	// Mutators
	//   Will not call updateServer
	//--------------------------------------------------------------------
public:
	void setUUID(const LLUUID& new_uuid);
	virtual void rename(const std::string& new_name);
	void setParent(const LLUUID& new_parent);
	void setType(LLAssetType::EType type);

private:
	// in place correction for inventory name string
	void correctInventoryName(std::string& name);

	//--------------------------------------------------------------------
	// File Support
	//   Implemented here so that a minimal information set can be transmitted
	//   between simulator and viewer.
	//--------------------------------------------------------------------
public:
	// virtual BOOL importFile(LLFILE* fp);
	virtual BOOL exportFile(LLFILE* fp, BOOL include_asset_key = TRUE) const;
	virtual BOOL importLegacyStream(std::istream& input_stream);
	virtual BOOL exportLegacyStream(std::ostream& output_stream, BOOL include_asset_key = TRUE) const;

	virtual void removeFromServer();
	virtual void updateParentOnServer(BOOL) const;
	virtual void updateServer(BOOL) const;

	//--------------------------------------------------------------------
	// Member Variables
	//--------------------------------------------------------------------
protected:
	LLUUID mUUID;
	LLUUID mParentUUID; // Parent category.  Root categories have LLUUID::NULL.
	LLAssetType::EType mType;
	std::string mName;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLInventoryItem
//
//   An item in the current user's inventory.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class LLInventoryItem : public LLInventoryObject
{
public:
	typedef LLDynamicArray<LLPointer<LLInventoryItem> > item_array_t;
	
	/** TODO: move to llinventorydefines
	 * Anonymous enumeration for specifying the inventory item flags.
	 */
	enum
	{
		// The shared flags at the top are shared among all inventory
		// types. After that section, all values of flags are type
		// dependent.  The shared flags will start at 2^30 and work
		// down while item type specific flags will start at 2^0 and
		// work up.
		II_FLAGS_NONE = 0,


		//
		// Shared flags
		//
		//

		// This value means that the asset has only one reference in
		// the system. If the inventory item is deleted, or the asset
		// id updated, then we can remove the old reference.
		II_FLAGS_SHARED_SINGLE_REFERENCE = 0x40000000,


		//
		// Landmark flags
		//
		II_FLAGS_LANDMARK_VISITED = 1,

		//
		// Object flags
		//

		// flag to indicate that object permissions should have next
		// owner perm be more restrictive on rez. We bump this into
		// the second byte of the flags since the low byte is used to
		// track attachment points.
		II_FLAGS_OBJECT_SLAM_PERM = 0x100,

		// flag to indicate that the object sale information has been changed.
		II_FLAGS_OBJECT_SLAM_SALE = 0x1000,

		// These flags specify which permissions masks to overwrite
		// upon rez.  Normally, if no permissions slam (above) or
		// overwrite flags are set, the asset's permissions are
		// used and the inventory's permissions are ignored.  If
		// any of these flags are set, the inventory's permissions
		// take precedence.
		II_FLAGS_OBJECT_PERM_OVERWRITE_BASE			= 0x010000,
		II_FLAGS_OBJECT_PERM_OVERWRITE_OWNER		= 0x020000,
		II_FLAGS_OBJECT_PERM_OVERWRITE_GROUP		= 0x040000,
		II_FLAGS_OBJECT_PERM_OVERWRITE_EVERYONE		= 0x080000,
		II_FLAGS_OBJECT_PERM_OVERWRITE_NEXT_OWNER	= 0x100000,

 		// flag to indicate whether an object that is returned is composed 
		// of muiltiple items or not.
		II_FLAGS_OBJECT_HAS_MULTIPLE_ITEMS			= 0x200000,

		//
		// wearables use the low order byte of flags to store the
		// EWearableType enumeration found in newview/llwearable.h
		//
		II_FLAGS_WEARABLES_MASK = 0xff,

		II_FLAGS_PERM_OVERWRITE_MASK = 				(II_FLAGS_OBJECT_SLAM_PERM |
													 II_FLAGS_OBJECT_SLAM_SALE |
													 II_FLAGS_OBJECT_PERM_OVERWRITE_BASE |
													 II_FLAGS_OBJECT_PERM_OVERWRITE_OWNER |
													 II_FLAGS_OBJECT_PERM_OVERWRITE_GROUP |
													 II_FLAGS_OBJECT_PERM_OVERWRITE_EVERYONE |
													 II_FLAGS_OBJECT_PERM_OVERWRITE_NEXT_OWNER),
			// These bits need to be cleared whenever the asset_id is updated
			// on a pre-existing inventory item (DEV-28098 and DEV-30997)
	};


	//--------------------------------------------------------------------
	// Initialization
	//--------------------------------------------------------------------
public:
	MEM_TYPE_NEW(LLMemType::MTYPE_INVENTORY);
	LLInventoryItem(const LLUUID& uuid,
					const LLUUID& parent_uuid,
					const LLPermissions& permissions,
					const LLUUID& asset_uuid,
					LLAssetType::EType type,
					LLInventoryType::EType inv_type,
					const std::string& name, 
					const std::string& desc,
					const LLSaleInfo& sale_info,
					U32 flags,
					S32 creation_date_utc);
	LLInventoryItem();
	// Create a copy of an inventory item from a pointer to another item
	// Note: Because InventoryItems are ref counted, reference copy (a = b)
	// is prohibited
	LLInventoryItem(const LLInventoryItem* other);
	virtual void copyItem(const LLInventoryItem* other); // LLRefCount requires custom copy
	void generateUUID() { mUUID.generate(); }
protected:
	~LLInventoryItem(); // ref counted

	//--------------------------------------------------------------------
	// Accessors
	//--------------------------------------------------------------------
public:
	virtual const LLUUID& getLinkedUUID() const;
	virtual const LLPermissions& getPermissions() const;
	virtual const LLUUID& getCreatorUUID() const;
	virtual const LLUUID& getAssetUUID() const;
	virtual const std::string& getDescription() const;
	virtual const LLSaleInfo& getSaleInfo() const;
	virtual LLInventoryType::EType getInventoryType() const;
	virtual U32 getFlags() const;
	virtual time_t getCreationDate() const;
	virtual U32 getCRC32() const; // really more of a checksum.
	
	//--------------------------------------------------------------------
	// Mutators
	//   Will not call updateServer and will never fail
	//   (though it may correct to sane values)
	//--------------------------------------------------------------------
public:
	void setAssetUUID(const LLUUID& asset_id);
	void setDescription(const std::string& new_desc);
	void setSaleInfo(const LLSaleInfo& sale_info);
	void setPermissions(const LLPermissions& perm);
	void setInventoryType(LLInventoryType::EType inv_type);
	void setFlags(U32 flags);
	void setCreationDate(time_t creation_date_utc);
	void setCreator(const LLUUID& creator); // only used for calling cards

	// Check for changes in permissions masks and sale info
	// and set the corresponding bits in mFlags.
	void accumulatePermissionSlamBits(const LLInventoryItem& old_item);

	// Put this inventory item onto the current outgoing mesage.
	// Assumes you have already called nextBlock().
	virtual void packMessage(LLMessageSystem* msg) const;

	// Returns TRUE if the inventory item came through the network correctly.
	// Uses a simple crc check which is defeatable, but we want to detect 
	// network mangling somehow.
	virtual BOOL unpackMessage(LLMessageSystem* msg, const char* block, S32 block_num = 0);

	//--------------------------------------------------------------------
	// File Support
	//--------------------------------------------------------------------
public:
	virtual BOOL importFile(LLFILE* fp);
	virtual BOOL exportFile(LLFILE* fp, BOOL include_asset_key = TRUE) const;
	virtual BOOL importLegacyStream(std::istream& input_stream);
	virtual BOOL exportLegacyStream(std::ostream& output_stream, BOOL include_asset_key = TRUE) const;

	//--------------------------------------------------------------------
	// Helper Functions
	//--------------------------------------------------------------------
public:
	// Pack all information needed to reconstruct this item into the given binary bucket.
	S32 packBinaryBucket(U8* bin_bucket, LLPermissions* perm_override = NULL) const;
	void unpackBinaryBucket(U8* bin_bucket, S32 bin_bucket_size);
	LLSD asLLSD() const;
	void asLLSD(LLSD& sd) const;
	bool fromLLSD(const LLSD& sd);

	//--------------------------------------------------------------------
	// Member Variables
	//--------------------------------------------------------------------
protected:
	LLPermissions mPermissions;
	LLUUID mAssetUUID;
	std::string mDescription;
	LLSaleInfo mSaleInfo;
	LLInventoryType::EType mInventoryType;
	U32 mFlags;
	time_t mCreationDate; // seconds from 1/1/1970, UTC
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Class LLInventoryCategory
//
//   A category/folder of inventory items. Users come with a set of default 
//   categories, and can create new ones as needed.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class LLInventoryCategory : public LLInventoryObject
{
public:
	typedef LLDynamicArray<LLPointer<LLInventoryCategory> > cat_array_t;

	//--------------------------------------------------------------------
	// Initialization
	//--------------------------------------------------------------------
public:
	MEM_TYPE_NEW(LLMemType::MTYPE_INVENTORY);
	LLInventoryCategory(const LLUUID& uuid, const LLUUID& parent_uuid,
						LLFolderType::EType preferred_type,
						const std::string& name);
	LLInventoryCategory();
	LLInventoryCategory(const LLInventoryCategory* other);
	void copyCategory(const LLInventoryCategory* other); // LLRefCount requires custom copy
protected:
	~LLInventoryCategory();
	
	//--------------------------------------------------------------------
	// Accessors And Mutators
	//--------------------------------------------------------------------
public:
	LLFolderType::EType getPreferredType() const;
	void setPreferredType(LLFolderType::EType type);
	LLSD asLLSD() const;
	bool fromLLSD(const LLSD& sd);

	//--------------------------------------------------------------------
	// Messaging
	//--------------------------------------------------------------------
public:
	virtual void packMessage(LLMessageSystem* msg) const;
	virtual void unpackMessage(LLMessageSystem* msg, const char* block, S32 block_num = 0);

	//--------------------------------------------------------------------
	// File Support
	//--------------------------------------------------------------------
public:
	virtual BOOL importFile(LLFILE* fp);
	virtual BOOL exportFile(LLFILE* fp, BOOL include_asset_key = TRUE) const;
	virtual BOOL importLegacyStream(std::istream& input_stream);
	virtual BOOL exportLegacyStream(std::ostream& output_stream, BOOL include_asset_key = TRUE) const;

	//--------------------------------------------------------------------
	// Member Variables
	//--------------------------------------------------------------------
protected:
	LLFolderType::EType	mPreferredType; // Type that this category was "meant" to hold (although it may hold any type).	
};


//-----------------------------------------------------------------------------
// Convertors
//
//   These functions convert between structured data and an inventory
//   item, appropriate for serialization.
//-----------------------------------------------------------------------------
LLSD ll_create_sd_from_inventory_item(LLPointer<LLInventoryItem> item);
LLSD ll_create_sd_from_inventory_category(LLPointer<LLInventoryCategory> cat);
LLPointer<LLInventoryCategory> ll_create_category_from_sd(const LLSD& sd_cat);

// TODO: replace InventoryObjectList with LLInventoryObject::object_list_t
// in the viewer code
typedef std::list<LLPointer<LLInventoryObject> > InventoryObjectList;

#endif // LL_LLINVENTORY_H
