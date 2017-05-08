#include <dynit.h>
#include <stdlib.h>
#include <errno.h>
#include <env.h>
#include <string.h>
#include <stdio.h>

#define _POSIX_SOURCE
#include <sys/wait.h>

#include "mvsdataset.h"
#include "mvssys.h"
#include "mvsargs.h"
#include "mvsutil.h"

#define USE_ATTACH_MVS 1

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
			fprintf(stdout, "Program is APF authorized\n"); 
		}
#if 0
		if (progInfo->programSize == 0) {
			fprintf(stdout, "Program is multi-segment program object or very large. Size unknown\n");
		} else {
			fprintf(stdout, "Program size is %d bytes\n", (progInfo->programSize << 3));
		}
		fprintf(stdout, "Program loaded at 0x%llX\n", progInfo->fp);
#endif
		fprintf(stdout, "Addressing mode: %s\n", prtAMode(progInfo->amode));
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
		fprintf(stdout, "%s run. Return code:%d\n", optInfo->programName, progInfo->rc);
    }
	
	return rc;
}

#if USE_ATTACH_MVS
	
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
			fprintf(stdout, "%s attached. Return val:%d code:%d reason:%d\n", pgmName, retVal, retCode, reasonCode);
		}
		if (retVal == -1) {
			progInfo->rc = retCode;
		} else {
			int pid = retVal;
			int status = 0;
			if (optInfo->debug) {
				fprintf(stdout, "Waiting on pid:%d\n", pid);
			}
			if ((pid = waitpid(pid, &status, 0)) == -1) {
				perror("Error waiting for program to complete");
			} else {
				if (WIFEXITED(status)) {
					if (optInfo->verbose) {
						fprintf(stdout, "Exit code: %d from %s\n", WEXITSTATUS(status), pgmName);
					}
					progInfo->rc = WEXITSTATUS(status);
				} else {
					fprintf(stderr, "child did not exit successfully");
					progInfo->rc = -1;
			    }	
			}
		}
	
		return progInfo->rc;
	}
#else
	static int call31BitOr64BitProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
		char cmdLine[MAX_SYSTEM_CMDLINE_LENGTH+1];
		sprintf(cmdLine, SYSTEM_FORMAT_SPECIFIER, optInfo->programName, optInfo->arguments);
		int rc = system(cmdLine);
		progInfo->rc = rc;
		
		if (optInfo->verbose) {
			fprintf(stdout, "%s run. Return code:%d\n", cmdLine, progInfo->rc);
		}
		
		return rc;
	}
#endif

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

ProgramFailure_T callProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	ProgramFailure_T rc = NoError;
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
		allocSubstitutionStr(optInfo, optInfo->programName, strlen(optInfo->programName));
		syntax(rc, optInfo);		
	}
	return rc;			 
}

#pragma linkage(LOAD, OS)
#pragma map(LOAD, "LOAD")
int LOAD(const char* module, unsigned int* info, unsigned long long* fp);

ProgramFailure_T loadProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	unsigned long long fp = 0xFFFFFFFFFFFFFFFFLL;
	char program[MAX_NAME_LEN+1];
	int rc;
	unsigned int info = 0xFFFFFFFF;	

	uppercaseAndPad(program, optInfo->programName, MAX_NAME_LEN);
	if (optInfo->verbose) {
		fprintf(stdout, "OS Load program %s\n", program);
	}
	rc = LOAD(program, &info, &fp);
	if (rc == 0) {
		setprogInfo(optInfo, progInfo, fp, info);	
		return NoError;
	} else {
		if (optInfo->verbose) {
			fprintf(stdout, "OS Load of %s failed with 0x%x\n", program, rc);
		}
		allocSubstitutionStr(optInfo, program, strlen(program));
		syntax(UnableToFetchProgram, optInfo);
		
		return UnableToFetchProgram;
	}
}
