/*
   Stand-alone MAPI testsuite

   OpenChange Project - E-MAIL OBJECT PROTOCOL operations

   Copyright (C) Julien Kerihuel 2008

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <libmapi/libmapi.h>
#include "utils/mapitest/mapitest.h"
#include "utils/mapitest/proto.h"

/**
   \file module_oxomsg.c

   \brief E-Mail Object Protocol test suite
*/


/**
   \details Test the AddressTypes (0x49) operation

   This function:
   -# Log on the user private mailbox
   -# Call the AddressTypes operation

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_AddressTypes(struct mapitest *mt)
{
	enum MAPISTATUS		retval;
	mapi_object_t		obj_store;
	uint16_t		cValues;
	struct mapi_LPSTR	*transport = NULL;
	uint32_t		i;

	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step2. AddressTypes operation */
	retval = AddressTypes(&obj_store, &cValues, &transport);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}
	
	for (i = 0; i < cValues; i++) {
		mapitest_print(mt, "* Recipient Type: %s\n", transport[i].lppszA);
	}

	/* Release */
	mapi_object_release(&obj_store);

	return true;
}


/**
   \details Test the SubmitMessage (0x32) operation

   This function:
   -# Log on the user private mailbox
   -# Open the Outbox folder
   -# Create a sample message
   -# Submit the message
   -# Delete the message
	

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_SubmitMessage(struct mapitest *mt)
{
	enum MAPISTATUS		retval;
	mapi_object_t		obj_store;
	mapi_object_t		obj_folder;
	mapi_object_t		obj_message;
	mapi_id_t		id_folder;
	mapi_id_t		id_msgs[1];
	bool			ret;
	
	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 2. Open Outbox folder */
	retval = GetDefaultFolder(&obj_store, &id_folder, olFolderOutbox);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	mapi_object_init(&obj_folder);
	retval = OpenFolder(&obj_folder, id_folder, &obj_folder);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 3. Create the sample message */
	mapi_object_init(&obj_message);
	ret = mapitest_common_message_create(mt, &obj_folder, &obj_message, MT_MAIL_SUBJECT);
	if (ret == false) {
		return ret;
	}

	/* Step 4. Submit Message */
	retval = SubmitMessage(&obj_message);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "SubmitMessage", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 5. Delete Message */
	id_msgs[0] = mapi_object_get_id(&obj_message);
	retval = DeleteMessage(&obj_folder, id_msgs, 1);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "DeleteMessage", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Release */
	mapi_object_release(&obj_message);
	mapi_object_release(&obj_folder);
	mapi_object_release(&obj_store);

	return true;
}


/**
   \details Test the AbortSubmit (0x34) operation

   This function:
   -# Log on the user private mailbox
   -# Open the Outbox folder
   -# Create a sample message
   -# Submit the message
   -# Abort the submit operation
   -# Delete the message
	
   Note: This operation may fail since it depends on how busy the
   server is when we submit the message. It is possible the message
   gets already processed before we have time to abort the message.

   From preliminary tests, AbortSubmit returns MAPI_E_SUCCESS when we
   call SubmitMessage with SubmitFlags set to 0x2.

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_AbortSubmit(struct mapitest *mt)
{
	enum MAPISTATUS		retval;
	mapi_object_t		obj_store;
	mapi_object_t		obj_folder;
	mapi_object_t		obj_message;
	mapi_id_t		id_folder;
	mapi_id_t		id_msgs[1];
	bool			ret = true;
	
	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 2. Open Outbox folder */
	retval = GetDefaultFolder(&obj_store, &id_folder, olFolderOutbox);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	mapi_object_init(&obj_folder);
	retval = OpenFolder(&obj_folder, id_folder, &obj_folder);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 3. Create the sample message */
	mapi_object_init(&obj_message);
	ret = mapitest_common_message_create(mt, &obj_folder, &obj_message, MT_MAIL_SUBJECT);
	if (ret == false) {
		return ret;
	}

	retval = SaveChangesMessage(&obj_folder, &obj_message);
	if (GetLastError() != MAPI_E_SUCCESS) {
		ret = false;
	}

	/* Step 4. Submit Message */
	retval = SubmitMessage(&obj_message);
	retval = AbortSubmit(&obj_store, &obj_folder, &obj_message);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "AbortMessage", GetLastError());
	if ((GetLastError() != MAPI_E_SUCCESS) && 
	    (GetLastError() != 0x80040114) &&
	    (GetLastError() != 0x80040601)) {
		ret = false;
	}

	/* Step 5. Delete Message */
	id_msgs[0] = mapi_object_get_id(&obj_message);
	retval = DeleteMessage(&obj_folder, id_msgs, 1);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "DeleteMessage", GetLastError());

	/* Release */
	mapi_object_release(&obj_message);
	mapi_object_release(&obj_folder);
	mapi_object_release(&obj_store);

	return ret;
}


/**
   \details Test the SetSpooler (0x47) operation

   This function:
   -# Log on the user private mailbox
   -# Informs the server it will acts as an email spooler

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_SetSpooler(struct mapitest *mt)
{
	enum MAPISTATUS		retval;
	mapi_object_t		obj_store;
	
	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 2. SetSpooler */
	retval = SetSpooler(&obj_store);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "SetSpooler", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Release */
	mapi_object_release(&obj_store);

	return true;
}


/**
   \details Test the SpoolerLockMessage (0x48) operation

   This function:
   -# Log on the user private mailbox
   -# Informs the server it will acts as an email spooler
   -# Create a message in the outbox folder
   -# Save message changes and Submit the message
   -# Lock the message
   -# Unlock-Finish the message

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_SpoolerLockMessage(struct mapitest *mt)
{
	enum MAPISTATUS		retval;
	bool			ret = true;
	mapi_object_t		obj_store;
	mapi_object_t		obj_folder;
	mapi_object_t		obj_message;

	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 2. SetSpooler */
	retval = SetSpooler(&obj_store);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "SetSpooler", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 3. Open the outbox folder */
	mapi_object_init(&obj_folder);
	ret = mapitest_common_folder_open(mt, &obj_store, &obj_folder, olFolderOutbox);
	if (ret == false) return ret;

	/* Step 4. Create the message */
	mapi_object_init(&obj_message);
	ret = mapitest_common_message_create(mt, &obj_folder, &obj_message, MT_MAIL_SUBJECT);
	mapitest_print(mt, "* %-35s: %s\n", "mapitest_common_message_create", 
		       ret == true ? "TRUE" : "FALSE");
	if (ret == false) return ret;

	/* Step 5. Save changes on message */
	retval = SaveChangesMessage(&obj_folder, &obj_message);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "SaveChangesMessage", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		ret = false;
	}

	/* Step 6. Submit the message */
	retval = SubmitMessage(&obj_message);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "SubmitMessage", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		ret = false;
	}

	/* Step 7. Lock the message */
	retval = SpoolerLockMessage(&obj_store, &obj_message, LockState_1stLock);
	mapitest_print(mt, "* %-35s: %s 0x%.8x\n", "SpoolerLockMessage", "1stLock", GetLastError());
	if (retval != MAPI_E_SUCCESS) {
		ret = false;
	}

	/* Step 8. finish locking the message */
	retval = SpoolerLockMessage(&obj_store, &obj_message, LockState_1stFinished);
	mapitest_print(mt, "* %-35s: %s 0x%.8x\n", "SpoolerLockMessage", "1stFinished", GetLastError());
	if (retval != MAPI_E_SUCCESS) {
		ret = false;
	}

	/* Release */
	mapi_object_release(&obj_message);
	mapi_object_release(&obj_folder);
	mapi_object_release(&obj_store);

	return ret;
}


/**
   \details Test the TransportSend (0x4a) operation

   This function:

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_TransportSend(struct mapitest *mt)
{
	enum MAPISTATUS			retval;
	bool				ret = true;
	mapi_object_t			obj_store;
	mapi_object_t			obj_folder;
	mapi_object_t			obj_message;
	struct mapi_SPropValue_array	lpProps;

	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 3. Open the outbox folder */
	mapi_object_init(&obj_folder);
	ret = mapitest_common_folder_open(mt, &obj_store, &obj_folder, olFolderOutbox);
	if (ret == false) return ret;

	/* Step 4. Create the message */
	mapi_object_init(&obj_message);
	ret = mapitest_common_message_create(mt, &obj_folder, &obj_message, MT_MAIL_SUBJECT);
	mapitest_print(mt, "* %-35s: %s\n", "mapitest_common_message_create", 
		       ret == true ? "TRUE" : "FALSE");
	if (ret == false) return ret;

	/* Step 5. Save changes on message */
	retval = SaveChangesMessage(&obj_folder, &obj_message);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "SaveChangesMessage", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		ret = false;
	}

	/* Step 6. TransportSend */
	retval = TransportSend(&obj_message, &lpProps);
	mapitest_print(mt, "* %-35s: 0x%.8x\n", "TransportSend", GetLastError());
	if (GetLastError() != MAPI_E_SUCCESS) {
		ret = false;
	}
	if (&lpProps) {
		uint32_t		i;
		struct SPropValue	lpProp;

		for (i = 0; i < lpProps.cValues; i++) {
			cast_SPropValue(&lpProps.lpProps[i], &lpProp);
			mapidump_SPropValue(lpProp, "\t* ");
		}
	}

	/* Release */
	mapi_object_release(&obj_message);
	mapi_object_release(&obj_folder);
	mapi_object_release(&obj_store);

	return ret;
}


/**
   \details Test the GetTransportFolder (0x6d) operation

   This function:
   -# Log on the user private mailbox
   -# Retrieves the folder ID of temporary transport folder

   \param mt pointer on the top-level mapitest structure

   \return true on success, otherwise false
 */
_PUBLIC_ bool mapitest_oxomsg_GetTransportFolder(struct mapitest *mt)
{
	enum MAPISTATUS		retval;
	mapi_object_t		obj_store;
	mapi_id_t		folder_id;

	/* Step 1. Logon */
	mapi_object_init(&obj_store);
	retval = OpenMsgStore(&obj_store);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	/* Step 2. Get the transport folder */
	retval = GetTransportFolder(&obj_store, &folder_id);
	mapitest_print(mt, "* %-35s: 0x%.8x (0x%llx)\n", "GetTransportFolder", 
		       GetLastError(), folder_id);
	if (GetLastError() != MAPI_E_SUCCESS) {
		return false;
	}

	return true;
}