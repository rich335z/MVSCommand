#ifndef __MVSDATASET__
	#define __MVSDATASET__ 1
	
	#define ASSIGN_CHAR '='
	#define DOT_CHAR '.'
	#define OPEN_PAREN_CHAR '('
	#define CLOSE_PAREN_CHAR ')'
	#define SEPARATOR_CHAR ':'
	
	#define MAX_DATASET_LEN (44)
	#define MAX_DATASET_LEN_STR "44"
	#define MAX_QUALIFIER_LEN   (8)
	#define MAX_MEMBER_LEN      (8)
	#define MAX_DDNAME_LEN      (8)
	#define MAX_NAME_LEN (8)
	#define MAX_NAME_LEN_STR "8"	
	
	#define STEPLIB_DDNAME "STEPLIB"
	#define DUMMY_NAME "DUMMY"
	#define CONSOLE_NAME "*"	
	
	struct DSNameNode;
	typedef struct DSNameNode {
		struct DSNameNode* next;
		char tempDDName[MAX_NAME_LEN+1];
		char dsName[MAX_DATASET_LEN+1]; 
		char memName[MAX_MEMBER_LEN+1];
	} DSNameNode_T;
	
	struct DSNameList;
	typedef struct DSNameList {
		DSNameNode_T* head;
		DSNameNode_T* tail;
	} DSNameList_T;
	
	static int hasMemberName(DSNameNode_T* dsNameNode) {
		return (dsNameNode->memName[0] != '\0');
	}
	static void setNoMemberName(DSNameNode_T* dsNameNode) {
		dsNameNode->memName[0] = '\0';
	}
	static void setMemberName(DSNameNode_T* dsNameNode, const char* memName, int len) {
		memcpy(dsNameNode->memName, memName, len);
		dsNameNode->memName[len] = '\0';	
	}
	
	struct DDNameList;
	typedef struct DDNameList {
		struct DDNameList* next;
		DSNameList_T* dsName;
		char ddName[MAX_DDNAME_LEN+1];
		int isConsole:1;
		int isDummy:1;		
	} DDNameList_T;
	
	#include "mvsargs.h"

	ProgramFailure_T removeConsoleFiles(OptInfo_T* optInfo);
	ProgramFailure_T printToConsole(OptInfo_T* optInfo);
	ProgramFailure_T establishDDNames(OptInfo_T* optInfo);
	ProgramFailure_T validMVSName(const char* name);
	ProgramFailure_T addDDName(const char* option, OptInfo_T* optInfo);
#endif