/*******************************************************************************
 * Copyright (c) 2017 IBM.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Mike Fulton - initial implentation and documentation
 *******************************************************************************/
#ifndef __MVSDATASET__
	#define __MVSDATASET__ 1
	
	#define ASSIGN_CHAR '='
	#define DOT_CHAR '.'
	#define OPEN_PAREN_CHAR '('
	#define CLOSE_PAREN_CHAR ')'
	#define SEPARATOR_CHAR ':'
	#define OPTION_CHAR ','
	
	#define MAX_DATASET_LEN (44)
	#define MAX_QUALIFIER_LEN   (8)
	#define MAX_MEMBER_LEN      (8)
	#define MAX_DDNAME_LEN      (8)
	#define MAX_NAME_LEN (8)
	
	#define STEPLIB_DDNAME "STEPLIB"
	#define DUMMY_NAME "DUMMY"
	#define CONSOLE_NAME "*"	
	#define DISP_EXCL "excl"
	#define DISP_OLD  "old"
	
	struct DSNode;
	typedef struct DSNode {
		struct DSNode* next;
		char tempDDName[MAX_NAME_LEN+1];
		char dsName[MAX_DATASET_LEN+1]; 
		char memName[MAX_MEMBER_LEN+1];
		int isExclusive:1;
	} DSNode_T;
	
	struct DSNodeList;
	typedef struct DSNodeList {
		DSNode_T* head;
		DSNode_T* tail;
	} DSNodeList_T;
	
	static int hasMemberName(DSNode_T* dsNode) {
		return (dsNode->memName[0] != '\0');
	}
	static void setNoMemberName(DSNode_T* dsNode) {
		dsNode->memName[0] = '\0';
	}
	static void setMemberName(DSNode_T* dsNode, const char* memName, int len) {
		memcpy(dsNode->memName, memName, len);
		dsNode->memName[len] = '\0';	
	}
	
	struct DDNameList;
	typedef struct DDNameList {
		struct DDNameList* next;
		DSNodeList_T dsNodeList;
		char ddName[MAX_DDNAME_LEN+1];
		int isConsole:1;
		int isDummy:1;	
	} DDNameList_T;
	
	#include "mvsargs.h"

	ProgramFailureMsg_T removeConsoleFiles(OptInfo_T* optInfo);
	ProgramFailureMsg_T printToConsole(OptInfo_T* optInfo);
	ProgramFailureMsg_T establishDDNames(OptInfo_T* optInfo);
	ProgramFailureMsg_T validMVSName(const char* name);
	ProgramFailureMsg_T addDDName(const char* option, OptInfo_T* optInfo);
#endif