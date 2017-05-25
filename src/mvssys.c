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
#include <dynit.h>
#include <stdlib.h>
#include <errno.h>
#include <env.h>
#include <string.h>
#include <stdio.h>

#define _POSIX_SOURCE
#include <sys/wait.h>

#include "mvsmsgs.h"
#include "mvsdataset.h"
#include "mvssys.h"
#include "mvsargs.h"
#include "mvsutil.h"

static const char* prtAMode(AddressingMode_T amode) {
	switch(amode) {
		case AMODE24: return "AMODE24";
		case AMODE31: return "AMODE31";
		case AMODE64: return "AMODE64";
	}
	return "<unk>";
}

static void setprogInfo(OptInfo_T* optInfo, ProgramInfo_T* progInfo, unsigned long long fp, unsigned int info) {
	if ((info >> 24) != 0) {
		progInfo->APFAuthorized = 1;
	}
	progInfo->programSize = (info & 0x00FFFFFF);
	if (fp & 0x1) {
		progInfo->amode = AMODE64;
		progInfo->fp = ((fp >> 1) << 1);
	} else if (fp & 0x80000000) {
		progInfo->amode = AMODE31;
		progInfo->fp = (fp & 0x7FFFFFFF);
	} else {
		progInfo->amode = AMODE24;
		progInfo->fp = fp;
	}
	 
	if (optInfo->verbose) {
		if (progInfo->APFAuthorized) {
			printInfo(InfoAPFAuthorized); 
		}
		printInfo(InfoAddressingMode, prtAMode(progInfo->amode));
	}
	if (optInfo->debug) {	
		if (progInfo->programSize == 0) {
			printInfo(InfoProgramSizeUnknown);
		} else {
			printInfo(InfoProgramSize, (progInfo->programSize << 3));
		}
		printInfo(InfoProgramLoadAddress, progInfo->fp);
	}
}

typedef int (OS_FP)();
#pragma linkage(OS_FP, OS)
typedef _Packed struct { short length; char arguments[MAX_ARGS_LENGTH]; } OSOpts_T;

static int call24BitProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
    OS_FP* fp = (OS_FP*) (progInfo->fp & 0x7FFFFFFF);
    OSOpts_T opts = { strlen(optInfo->arguments) };
    int rc;

    memcpy(opts.arguments, optInfo->arguments, opts.length);

    rc = fp(&opts);
    progInfo->rc = rc;

    if (optInfo->verbose) {
		printInfo(InfoProgramReturnCode, optInfo->programName, progInfo->rc);
    }
	
	return rc;
}

#pragma linkage(ATTMVS, OS)
#pragma map(ATTMVS, "ATTMVS")
void ATTMVS(int* pgmNameLen, const char* pgmName, int* argsLen, const char* args, void** exitAddr, void** exitParm, int* retVal, int* retCode, int* reasonCode);

static int call31BitOr64BitProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	char pgmName[MAX_NAME_LEN+1];
	int pgmNameLen;
	int argsLen = strlen(optInfo->arguments);
	const char* args = optInfo->arguments;
	void* exitAddr = NULL;
	void* exitParm = NULL;
	int retVal = 0;
	int retCode = 0;
	int reasonCode = 0;

	strcpy(pgmName, optInfo->programName);
	uppercase(pgmName);
	pgmNameLen = strlen(pgmName);
	ATTMVS(&pgmNameLen, pgmName, &argsLen, args, &exitAddr, &exitParm, &retVal, &retCode, &reasonCode);
	if (optInfo->debug) {
		printInfo(InfoProgramAttachInfo, pgmName, retVal, retCode, reasonCode);
	}
	if (retVal == -1) {
		progInfo->rc = retCode;
	} else {
		int pid = retVal;
		int status = 0;
		if (optInfo->debug) {
			printInfo(InfoWaitingOnPID, pid);
		}
		if ((pid = waitpid(pid, &status, 0)) == -1) {
			perror("");
			printError(ErrorWaitingForPID, retVal);
		} else {
			if (WIFEXITED(status)) {
				if (optInfo->verbose) {
					printInfo(InfoAttachExitCode, WEXITSTATUS(status), pgmName);
				}
				progInfo->rc = WEXITSTATUS(status);
			} else {
				printError(ErrorChildCompletion, pgmName);
				progInfo->rc = -1;
		    }	
		}
	}
	return progInfo->rc;
}

static int call64BitProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	int rc;
	return rc;
}

static int IAm64Bit() {
	return sizeof(void*) == 8;
}

#pragma linkage(SETDUBDF, OS)
#pragma map(SETDUBDF, "SETDUBDF")
void SETDUBDF(int* dubVal, int* retVal, int* retCode, int* reasonCode);

ProgramFailureMsg_T callProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	ProgramFailureMsg_T rc = NoError;
	if (progInfo->amode == AMODE24) {
		progInfo->rc = call24BitProgram(optInfo, progInfo);
		rc = NoError;
	} else {
		if (IAm64Bit() && (progInfo->amode == AMODE64)) {
			progInfo->rc = call31BitOr64BitProgram(optInfo, progInfo);
			rc = NoError;
		} else if (!IAm64Bit() && (progInfo->amode == AMODE31)) {
			int dub=4; /* dub-process-defer */
			int retVal=6;
			int retCode=6;
			int reasonCode=6;
			SETDUBDF(&dub, &retVal, &retCode, &reasonCode);
			if (retVal != 0) {
				rc = ErrorDubbingProgram;
			} else {
				progInfo->rc = call31BitOr64BitProgram(optInfo, progInfo);	
				rc = NoError;
			}
		} else if (IAm64Bit() && (progInfo->amode == AMODE31)) {
			rc = ErrorRunning31BitModuleWith64BitDriver;
		} else if (!IAm64Bit() && (progInfo->amode == AMODE64)) {
			rc = ErrorRunning64BitModuleWith31BitDriver;
		}
	}	
	if (rc != NoError) {
		printError(rc, optInfo->programName);
	}
	return rc;			 
}

#pragma linkage(LOAD, OS)
#pragma map(LOAD, "LOAD")
int LOAD(const char* module, unsigned int* info, unsigned long long* fp);

ProgramFailureMsg_T loadProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	unsigned long long fp = 0xFFFFFFFFFFFFFFFFLL;
	char program[MAX_NAME_LEN+1];
	int rc;
	unsigned int info = 0xFFFFFFFF;	

	uppercaseAndPad(program, optInfo->programName, MAX_NAME_LEN);
	if (optInfo->verbose) {
		printInfo(InfoLoadProgram, program);
	}
	rc = LOAD(program, &info, &fp);
	if (rc == 0) {
		setprogInfo(optInfo, progInfo, fp, info);	
		return NoError;
	} else {
		if (optInfo->verbose) {
			printInfo(InfoLoadProgramDetails, program, rc);
		}
		printError(UnableToFetchProgram, program);
		
		return UnableToFetchProgram;
	}
}
