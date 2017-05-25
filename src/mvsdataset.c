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
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <dynit.h>
#include <env.h>
#include <string.h>

#include "mvsmsgs.h"
#include "mvsdataset.h"
#include "mvsargs.h"
#include "mvsutil.h"

typedef struct {
	unsigned short len;
	char str[1];
} VarStr_T;

typedef struct {
	unsigned short key;
	unsigned short numDatasets;
} SVC99ConcatenatedDatasetAllocation_T;
	
static const char* prtStrPointer(const char* p) {
	if (p == NULL) {
		return "null";
	} else {
		return p;
	}
}

static void SteplibAllocationError(OptInfo_T* optInfo, const char* steplib) {
	printError(ErrorAllocatingSTEPLIB, steplib);
}

static void ConsoleAllocationError(OptInfo_T* optInfo, __dyn_t* ip) {
	char* ddName = ip->__ddname;
	char* dsName = ip->__dsname;
	printError(ErrorAllocatingCONSOLE, ddName, dsName);	
}

static void ConcatenationAllocationError(OptInfo_T* optInfo, DDNameList_T* ddNameList, struct __S99struc* parmlist) {
	char* ddName = ddNameList->ddName;
	printError(ErrorAllocatingConcatenation, ddName);	
}

static void DatasetAllocationError(OptInfo_T* optInfo, __dyn_t* ip) {
	char* ddName = ip->__ddname;
	char* dsName = ip->__dsname;
	/*
	 * Error Codes:
	 * https://www.ibm.com/support/knowledgecenter/SSLTBW_2.1.0/com.ibm.zos.v2r1.ieaa800/erc.htm
	 */
	if (optInfo->verbose) {
		printError(ErrorCodesAllocatingDataset, ip->__errcode, ip->__infocode);
	}
	if (ip->__errcode == DATASET_PROBABLY_DOES_NOT_EXIST) {
		printError(ErrorAllocatingNonExistantDataset, ddName, dsName);
	} else {
		printError(ErrorAllocatingDataset, ddName, dsName);	
	}
}

static void PDSMemberAllocationError(OptInfo_T* optInfo, __dyn_t* ip) {
	char* ddName = ip->__ddname;
	char* dsName = ip->__dsname;
	char* memName= ip->__member;
	printError(ErrorAllocatingPDSMember, ddName, dsName, memName);	}

static void DummyAllocationError(OptInfo_T* optInfo, __dyn_t* ip) {
	char* ddName = ip->__ddname;
	printError(ErrorAllocatingDUMMY, ddName); 	
}

static ProgramFailureMsg_T allocDDNode(DDNameList_T** ddNameList) {
	DDNameList_T* newDDNode = malloc(sizeof(DDNameList_T));
	if (newDDNode == NULL) {
		return InternalOutOfMemory;
	}

	newDDNode->next = *ddNameList;
	*ddNameList = newDDNode;
	
	return NoError;
}

static void freeDDNode(DDNameList_T* node) {
	free(node);
}

static ProgramFailureMsg_T allocDSNode(DSNodeList_T* dsNodeList) {
	DSNode_T* newNode = malloc(sizeof(DSNode_T));
	if (newNode == NULL) {
		return InternalOutOfMemory;
	}
	if (dsNodeList->head == NULL) {
		dsNodeList->head = newNode;
		dsNodeList->tail = newNode;
		newNode->next = NULL;
	} else {
		dsNodeList->tail->next = newNode;
		dsNodeList->tail = newNode;
	}
	
	return NoError;
}

static void freeDSNode(DSNode_T* node) {
	free(node);
}

/*
  From: https://www.ibm.com/support/knowledgecenter/SSJJ9R_4.0.4/com.ibm.team.scm.doc/topics/r_RTCz_datasetparms.html
  PDS member naming rules
   A member name cannot be longer than eight characters.
   The first member character must be either a letter or one of the following three special characters: #, @, $.
   The remaining seven characters can be letters, numbers, or one of the following special characters: #, @, or $.
   A PDS member name cannot contain a hyphen (-).
   A PDS member name cannot contain accented characters (à, é, è, and so on).
*/
static int isMVSLetter(int c) {
	return (isalpha(c) || (c == '#' || c == '@' || c == '$'));
}
static int isMVSNumber(int c) {
	return isdigit(c);
}

ProgramFailureMsg_T validMVSName(const char* name) {
	const int len = strlen(name);
	int i;
	if (len == 0) {
		return NoMVSName;
	}
	if (len > MAX_NAME_LEN) {
		return MVSNameTooLong;
	}
	if (!isMVSLetter(name[0])) {
		return MVSNameInvalid;
	}
	for (i=1; i<len; ++i) {
		int c = name[i];
		if (!isMVSLetter(c) && !isMVSNumber(c)) {
			return MVSNameInvalid;
		}
	}
	return NoError;
}

static ProgramFailureMsg_T validProgramName(const char* name) {
	return validMVSName(name);
}

static ProgramFailureMsg_T validDDName(const char* name) {
	return validMVSName(name);
}
static ProgramFailureMsg_T validQualifierName(const char* name) {
	return validMVSName(name);
}
static ProgramFailureMsg_T validMemberName(const char* name) {
	return validMVSName(name);
}

static ProgramFailureMsg_T validDatasetName(const char* buffer, int start, int end, OptInfo_T* optInfo) {
	char qualifier[MAX_QUALIFIER_LEN+1];
	char member[MAX_MEMBER_LEN+1];
	int prevDot=start-1;
	int openParen=-1;
	int i;
	ProgramFailureMsg_T rc = NoError;
	int len = end-start;

	if (len == 0) {
		return NoDatasetName;
	}

	for (i=start; i<=end; ++i) {
		if (buffer[i] == DOT_CHAR || buffer[i] == OPEN_PAREN_CHAR || i == end) {
			const char* qualifierName = &buffer[prevDot+1];
			if (i-prevDot-1 > MAX_QUALIFIER_LEN) {
				printError(MVSNameTooLong, i-prevDot-1, qualifierName, MAX_NAME_LEN);
				return MVSNameTooLong;
			}
			if (i-prevDot == 1) {
				printError(InvalidDatasetName, len, &buffer[start]);
				return InvalidDatasetName;
			}
			memcpy(qualifier, qualifierName, i-prevDot-1);
			qualifier[i-prevDot-1] = '\0';
			if (optInfo->debug) {
				printInfo(InfoQualifierValidity, qualifier);
			}
			
			rc = validQualifierName(qualifier);
			if (rc != NoError) {
				printError(rc, i-prevDot-1, qualifier);
				return rc;
			}
			if (buffer[i] == DOT_CHAR) {
				prevDot = i;
			} else if (buffer[i] == OPEN_PAREN_CHAR) {
				openParen = i;
				break;
			}
		}
	}
	if (openParen >= 0) {
		if (openParen == 0) {
			printError(InvalidDatasetName, len, &buffer[start]);
			return InvalidDatasetName;
		}
		if (openParen > MAX_DATASET_LEN) {
			printError(DatasetNameTooLong, openParen-start, &buffer[start], MAX_DATASET_LEN);
			return DatasetNameTooLong;
		}
		if (buffer[end-1] != CLOSE_PAREN_CHAR) {
			printError(InvalidDatasetName, len, &buffer[start]);
			return InvalidDatasetName;
		}
		if (end-openParen-2 > MAX_MEMBER_LEN) {
			printError(MVSNameTooLong, end-openParen-2, &buffer[openParen+1], MAX_NAME_LEN);
			return MVSNameTooLong;			
		}
		memcpy(member, &buffer[openParen+1], end-openParen-2);
		member[end-openParen-2] = '\0';
		if (optInfo->debug) {
			printInfo(InfoMemberValidity, member);
		}		
		rc = validMemberName(member);
		if (rc != NoError) {
			printError(rc, end-openParen-1, member);
			return rc;
		}
	} else {
		if (len > MAX_DATASET_LEN) {
			printError(DatasetNameTooLong, len, buffer, MAX_DATASET_LEN);
			return DatasetNameTooLong;
		}		
	}	

	return NoError;
}	

static void copyDSNameAndMember(DSNode_T* entry, const char* buffer, int start, int end) {
	int dsNameLen;
	int memStart;
	if (entry && buffer) {
		for (memStart=start; memStart<end; ++memStart) {
			if (buffer[memStart] == OPEN_PAREN_CHAR) {
				break;
			}
		}
		if (memStart < end) {
			setMemberName(entry, &buffer[memStart+1], end-memStart-2);
		} else {
			setNoMemberName(entry);
		}
		dsNameLen = memStart-start;
		memcpy(entry->dsName, &buffer[start], dsNameLen);
		entry->dsName[dsNameLen] = '\0';

		uppercase(entry->dsName);
		uppercase(entry->memName);
	}
}

ProgramFailureMsg_T addDDName(const char* option, OptInfo_T* optInfo) {
	int optLen = strlen(option);
	ProgramFailureMsg_T rc = allocDDNode(&optInfo->ddNameList);
	int assignPos=-1, i;
	int dsNameStartPos;
	DDNameList_T* entry = optInfo->ddNameList;
	if (rc != NoError) {
		return rc;
	}
	for (i=0; option[i] != '\0'; ++i) {
		if (option[i] == ASSIGN_CHAR) {
			assignPos = i;
			break;
		}
	}
	if (assignPos < 0) {
		printError(UnrecognizedOption, optLen, option);
		return UnrecognizedOption;
	}
	if (assignPos > MAX_DDNAME_LEN) {
		printf("assignpos:%d option:<%s>, length:%d\n", assignPos, option, MAX_NAME_LEN);
		fflush(stdout);
		printError(MVSNameTooLong, assignPos, option, MAX_NAME_LEN);
		return MVSNameTooLong;
	}

	memcpy(entry->ddName, option, assignPos);
	entry->ddName[assignPos] = '\0';
	rc = validDDName(entry->ddName);
	if (rc != NoError) {
		printError(rc, assignPos, option);
		return rc;
	}
	uppercase(entry->ddName);
	
	if (!strcmp(&option[assignPos+1], CONSOLE_NAME)) {
		rc = allocDSNode(&entry->dsNodeList);
		if (rc == NoError) {
			entry->isConsole = 1;
		}
		return rc;
	}
	if (!strnocasecmp(&option[assignPos+1], DUMMY_NAME)) {
		rc = allocDSNode(&entry->dsNodeList);
		if (rc == NoError) {
			entry->isDummy = 1;
		}
		return rc;
	}	

	dsNameStartPos=assignPos+1;
	while (1) {
		if (option[i] == '\0' || option[i] == SEPARATOR_CHAR || option[i] == OPTION_CHAR) {
			rc = validDatasetName(option, dsNameStartPos, i, optInfo);
			if (rc == NoError) {
				rc = allocDSNode(&entry->dsNodeList);
				if (rc != NoError) {
					return rc;
				}
				copyDSNameAndMember(entry->dsNodeList.tail, option, dsNameStartPos, i);
				if (option[i] == '\0' || option[i] == OPTION_CHAR) {
					break;
				}
				dsNameStartPos=i+1;
			} else {
				if (rc == NoDatasetName) {
					printError(NoDatasetName, assignPos, option);
				} 
				freeDDNode(entry);
				break;
			}	
		}	
		++i;
	}
	if (option[i] == OPTION_CHAR) {
		const char* subOpt = &option[i+1]; 
		if (optInfo->debug) {
			printInfo(InfoCheckOption, subOpt);
		}		

		if (!strnocasecmp(subOpt, DISP_OLD) || !strnocasecmp(subOpt, DISP_EXCL)) {
			entry->dsNodeList.tail->isExclusive = 1;
		} else {
			printError(InvalidDatasetOption, optLen-i, &option[i+1]);	
			rc = InvalidDatasetOption;
		}
	}
	return rc;
}


static int allocPDSMember(OptInfo_T* optInfo, char* ddName, DSNode_T* dsNode) {
	__dyn_t ip;
	int rc;

	dyninit(&ip);

	ip.__ddname = ddName; 
	ip.__dsname = dsNode->dsName; 
	ip.__member = dsNode->memName;
	ip.__dsorg  = __DSORG_PS;
	if (dsNode->isExclusive) {
		ip.__status = __DISP_OLD;
	} else {
		ip.__status = __DISP_SHR; 
	}   

	errno = 0;
	rc = dynalloc(&ip); 
	if (rc) {
		PDSMemberAllocationError(optInfo, &ip);
	} else {
   		if (optInfo->verbose) {
   			printInfo(InfoPDSMemberAllocationSucceeded, ddName, dsNode->dsName, dsNode->memName);
   		}
   	}
	return rc;
}

static int allocDataset(OptInfo_T* optInfo, char* ddName, DSNode_T* dsNode) {
	__dyn_t ip;
	int rc;

	dyninit(&ip);

	ip.__ddname = ddName; 
	ip.__dsname = dsNode->dsName; 
	
	if (dsNode->isExclusive) {
		ip.__status = __DISP_OLD;
	} else {
		ip.__status = __DISP_SHR; 
	}

	errno = 0;
	rc = dynalloc(&ip); 
	if (rc) {
		DatasetAllocationError(optInfo, &ip);
	} else {
   		if (optInfo->verbose) {
   			printInfo(InfoDatasetAllocationSucceeded, ddName, dsNode->dsName);
   		}
   	}
	return rc;
}

static char* temporaryMVSSequentialDataset(char* buffer) {
	char tmpNamBuffer[MAX_DATASET_LEN+3];
	char* result = tmpnam(tmpNamBuffer);
	int len = strlen(result);
	if (result == NULL) { return result; }
	
	/*
	 * remove the single quotes that surround the name
	 */
	memmove(buffer, &tmpNamBuffer[1], len-2);
	buffer[len-2] = '\0';
	
	return buffer;
}	

	
static int allocConsole(OptInfo_T* optInfo, DDNameList_T* ddNameList) {
	__dyn_t ip;
	int rc;

	DSNode_T* node = ddNameList->dsNodeList.head;
	temporaryMVSSequentialDataset(node->dsName);
	if (node->dsName == NULL) {
		return -1;
	}
	dyninit(&ip);

	ip.__ddname = ddNameList->ddName; 
	ip.__dsname = node->dsName;
	ip.__dsorg  = __DSORG_PS;
	ip.__normdisp = __DISP_CATLG;
	ip.__lrecl  = 137;
	ip.__recfm  = _VB_ + _A_;
	ip.__status = __DISP_NEW; 
	ip.__alcunit = __TRK;
	ip.__primary = 100;
	ip.__secondary = 100;

	errno = 0;
	rc = dynalloc(&ip); 
	if (rc) {
		ConsoleAllocationError(optInfo, &ip);
	} else {
   		if (optInfo->verbose) {
   			printInfo(InfoConsoleDatasetAllocationSucceeded, ddNameList->ddName);
   		}
   	}
	return rc;
}

/*
 * Convert a datasetname from HLQ.MLQ.....LLQ to:
 * //'HLQ.MLQ....LLQ'
 */
static char* CIODatasetName(const char* fullyQualifiedDataset, char* buffer) {
	int len = strlen(fullyQualifiedDataset);
	memcpy(buffer, "//'", 3);
	memcpy(&buffer[3], fullyQualifiedDataset, len);
	memcpy(&buffer[3+len], "'", 2);

	return buffer;
}

static int printDatasetToConsole(char* dataset) {
	char posixName[MAX_DATASET_LEN+5];
	FILE* fp = fopen(CIODatasetName(dataset, posixName), "rb,type=record");
	char line[138] = "";
	size_t numElements = 1;
	size_t size = 137;
	int readCount;

	while ((readCount = fread(line, numElements, size, fp)) > 0) {
		line[readCount] = '\0';
		printf("%s\n", line);
	}
	fclose(fp);

	return 0;
}

static int allocConcatenationReadOnly(OptInfo_T* optInfo, DDNameList_T* ddNameList) {
	DSNode_T* cur = ddNameList->dsNodeList.head;
	int rc;
	int maxRC = 0;
	int numDatasets = 0;
	struct __S99struc parmlist = {0};
	int totSize = sizeof(SVC99ConcatenatedDatasetAllocation_T);
	VarStr_T* curDD;
	SVC99ConcatenatedDatasetAllocation_T* svc99Parms;
	int svc99ParmsInt;
	SVC99ConcatenatedDatasetAllocation_T** svc99PParms;
	
	while (cur != NULL) {
		if (numDatasets == 0) { /* use the concatenation ddname for the first allocation */
			strcpy(cur->tempDDName, ddNameList->ddName);
		} else {
			strcpy(cur->tempDDName, "????????"); 
		}
		if (hasMemberName(cur)) {
			rc = allocPDSMember(optInfo, cur->tempDDName, cur);
		} else {
			rc = allocDataset(optInfo, cur->tempDDName, cur);
		}
		if (optInfo->verbose) {
			printInfo(InfoConcatenatedDatasetAllocationSucceeded, cur->tempDDName, cur->dsName, numDatasets);
		}
		if (rc) {
			if (rc > maxRC) {
				maxRC = rc;
			}
		}
		totSize += (sizeof(short) + strlen(cur->tempDDName));
		++numDatasets;
		cur = cur->next;
	}
	if (maxRC > 0) {
		return maxRC;
	}
	
	/*
	 * Example: To specify concatenation of SYSLIB to MYLIB, code: 
	 *  KEY    #      LEN    PARM         LEN    PARM
     *  0001   0002   0005   D4E8D3C9C2   0006   E2E8E2D3C9C2
	 */
	svc99Parms = malloc(totSize);
	if (svc99Parms == NULL) {
		return InternalOutOfMemory; /* msf - fix */
	}
	svc99Parms->key = 1;
	svc99Parms->numDatasets = numDatasets;
	
	cur = ddNameList->dsNodeList.head;
	curDD = (VarStr_T*) &svc99Parms[1];
	
	while (cur != NULL) {
		curDD->len = strlen(cur->tempDDName);
		memcpy(curDD->str, cur->tempDDName, curDD->len);
		curDD = (VarStr_T*) (((char*) curDD) + sizeof(short) + curDD->len);
		cur = cur->next;
	}
	
	parmlist.__S99VERB = 3;  /* Verb code for concatenation      */
	parmlist.__S99RBLN = 20; /* minimum length for request block */
	svc99ParmsInt = (int) ((void*) svc99Parms);
	svc99Parms = (void*) (svc99ParmsInt | 0x80000000); /* turn HOb on */
	svc99PParms = &svc99Parms;
	parmlist.__S99TXTPP = svc99PParms;
	
	rc = svc99(&parmlist);
	if (rc) {
		ConcatenationAllocationError(optInfo, ddNameList, &parmlist);
	}
	return rc;
}

static int allocDummy(OptInfo_T* optInfo, char* ddName) {
	__dyn_t ip;
	int rc;

	dyninit(&ip);

	ip.__ddname = ddName; 
	ip.__misc_flags = __DUMMY_DSN;

	errno = 0;
	rc = dynalloc(&ip); 
	if (rc) {
		DummyAllocationError(optInfo, &ip);
	} else {
   		if (optInfo->verbose) {
   			printInfo(InfoDummyDatasetAllocationSucceeded, ddName);
   		}
   	}
	return rc;
}	

static int allocSteplib(OptInfo_T* optInfo, DSNodeList_T* dsNodeList) {
/*
 * We can not set up STEPLIB at this point - it is too late. This would set up the 
 * STEPLIB for the program we are calling, but if the program itself needs the STEPLIB
 * (which is the typical case), then this will not help
 */
	char* steplib;
	char* pos;
	DSNode_T* cur = dsNodeList->head;
	int length = 0;
	
	while (cur != NULL) {
		length += (strlen(cur->dsName) + 1);
		cur = cur->next;
	}

	steplib = malloc(length);
	if (steplib == NULL) {
		return InternalOutOfMemory;
	}
	
	cur = dsNodeList->head;
	pos = steplib;
	while (cur != NULL) {
		int dsNameLen = strlen(cur->dsName);
		memcpy(pos, cur->dsName, dsNameLen);
		pos += dsNameLen;
		if (cur->next) {
			*pos = ':';
			++pos;
		}
		cur = cur->next;
	}
	
	SteplibAllocationError(optInfo, steplib);
	
	free(steplib);
	
	return 16;
	
#if 0
	if (setenv(STEPLIB_DDNAME, steplib, 1)) {
		perror("STEPLIB DDName allocation");
		fprintf(stderr, "Dataset allocation failed for %s=%s\n", STEPLIB_DDNAME, steplib);
		return 16;
	} else {
		if (optInfo->verbose) {
			printInfo(Info,"Dataset allocation succeeded for %s=%s\n", STEPLIB_DDNAME, steplib);
		}
	}
	return 0; 
#endif

}	
	
static void printProgramInfo(const char* programName, const char* arguments) {
	printInfo(InfoProgramName, prtStrPointer(programName), prtStrPointer(arguments));
}

static void printDDNames(DDNameList_T* ddNameList) {	
	printInfo(InfoDDNameHeader);
	if (ddNameList == NULL) {
		printInfo(InfoNoDDNames);
	}
	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			printInfo(InfoConsoleDDName, ddNameList->ddName);	
		} else if (ddNameList->isDummy) {
			printInfo(InfoDummyDDName, ddNameList->ddName);				
		} else {
			DSNode_T* dsNode = ddNameList->dsNodeList.head;
			printInfo(InfoDDName, ddNameList->ddName);
			while (dsNode != NULL) {
				if (hasMemberName(dsNode)) {
					printInfo(InfoPDSMemberName, dsNode->dsName, dsNode->memName);
				} else {
					printInfo(InfoDatasetName, dsNode->dsName);
				}	
				if (dsNode->isExclusive) {
					printInfo(InfoExclusive);
				}
				dsNode = dsNode->next;
				if (dsNode != NULL) {
					printInfo(InfoConcatenationSeparator);
				} else {
					printInfo(InfoNewline);
				}
			}
		}
		ddNameList = ddNameList->next;
	}
}


ProgramFailureMsg_T establishDDNames(OptInfo_T* optInfo) {
	DDNameList_T* ddNameList = optInfo->ddNameList;
	int rc;
	int maxRC = 0;
	
	if (optInfo->verbose) {
		printProgramInfo(optInfo->programName, optInfo->arguments);
		printDDNames(ddNameList);
	}

	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			rc = allocConsole(optInfo, ddNameList);
		} else if (ddNameList->isDummy) {
			rc = allocDummy(optInfo, ddNameList->ddName);
		} else {
			if (!strcmp(ddNameList->ddName, STEPLIB_DDNAME)) {
				rc = allocSteplib(optInfo, &ddNameList->dsNodeList);
			} else if (ddNameList->dsNodeList.head->next == NULL) {
				if (hasMemberName(ddNameList->dsNodeList.head)) {
					rc = allocPDSMember(optInfo, ddNameList->ddName, ddNameList->dsNodeList.head);
				} else {
					rc = allocDataset(optInfo, ddNameList->ddName, ddNameList->dsNodeList.head);
				}	
			} else {
				rc = allocConcatenationReadOnly(optInfo, ddNameList);
			}
		}
		if (rc > maxRC) {
			maxRC = rc;
		}
		ddNameList = ddNameList->next;
	}
	
	if (maxRC > 0) {
		return DDNameAllocationFailure;
	} else {
		return NoError;
	}
}

ProgramFailureMsg_T printToConsole(OptInfo_T* optInfo) {
	DDNameList_T* ddNameList = optInfo->ddNameList;
	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			if (printDatasetToConsole(ddNameList->dsNodeList.head->dsName)) {
				printError(ErrorPrintingDatasetToConsole, ddNameList->dsNodeList.head->dsName); 
				return ErrorPrintingDatasetToConsole;
			}
		}
		ddNameList = ddNameList->next;
	}
	return NoError;
}

ProgramFailureMsg_T removeConsoleFiles(OptInfo_T* optInfo) {
	char posixName[MAX_DATASET_LEN+5];
	DDNameList_T* ddNameList = optInfo->ddNameList;
	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			CIODatasetName(ddNameList->dsNodeList.head->dsName, posixName);
			if (optInfo->debug) {
				printInfo(InfoTemporaryDatasetRetained, ddNameList->dsNodeList.head->dsName);
			} else {
				if (remove(posixName)) {
					printError(ErrorDeletingTemporaryDataset, ddNameList->dsNodeList.head->dsName); 
					return ErrorDeletingTemporaryDataset;
				}
			}
		}
		ddNameList = ddNameList->next;
	}
	return NoError;
}