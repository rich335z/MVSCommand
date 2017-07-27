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
	
	#include "mvsutil.h"
	
	#define ASSIGN_CHAR '='
	#define HFS_PATH_CHAR '/'
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
	
	#define DATASET_PROBABLY_DOES_NOT_EXIST 0x1708
	
	typedef _Packed struct DSNode {
		char dsName[MAX_DATASET_LEN+1]; 
		char memName[MAX_MEMBER_LEN+1];
	} DSNode_T;
	typedef _Packed struct DSConcatenationNode {
		DSNode_T ds;
		char tempDDName[MAX_NAME_LEN+1];
	} DSConcatenationNode_T;
	typedef _Packed struct HFSNode {
		char fileName[FILENAME_MAX+1]; 
	} HFSNode_T;
	
	struct FileNode;
	typedef _Packed struct FileNode {
		_Packed struct FileNode* next;
		union {
			DSNode_T ds;
			DSConcatenationNode_T dsc;
			HFSNode_T hfs;
		} node;
	} FileNode_T;
	
	struct FileNodeList;
	typedef struct FileNodeList {
		FileNode_T* head;
		FileNode_T* tail;
	} FileNodeList_T;
	
	static int hasMemberName(FileNode_T* fileNode) {
		return (fileNode->node.ds.memName[0] != '\0');
	}
	static void setNoMemberName(FileNode_T* fileNode) {
		fileNode->node.ds.memName[0] = '\0';
	}
	static void setMemberName(FileNode_T* fileNode, const char* memName, int len) {
		memcpy(fileNode->node.ds.memName, memName, len);
		fileNode->node.ds.memName[len] = '\0';	
		uppercase(fileNode->node.ds.memName);
	}
	
	static void setDSName(FileNode_T* fileNode, const char* dsName, int len) {
		memcpy(fileNode->node.ds.dsName, dsName, len);
		fileNode->node.ds.dsName[len] = '\0';		
		uppercase(fileNode->node.ds.dsName);
	}
	
	struct DDNameList;
	typedef struct DDNameList {
		struct DDNameList* next;
		FileNodeList_T fileNodeList;
		char ddName[MAX_DDNAME_LEN+1];
		int isHFS:1;
		int isConsole:1;
		int isDummy:1;	
		int isConcatenation:1;
		int isExclusive:1;
	} DDNameList_T;
	
	#include "mvsargs.h"

	ProgramFailureMsg_T removeConsoleFiles(OptInfo_T* optInfo);
	ProgramFailureMsg_T printToConsole(OptInfo_T* optInfo);
	ProgramFailureMsg_T establishDDNames(OptInfo_T* optInfo);
	ProgramFailureMsg_T validMVSName(const char* name);
	ProgramFailureMsg_T addDDName(const char* option, OptInfo_T* optInfo);
#endif