
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <dynit.h>
#include <stdlib.h>
#include <errno.h>
#include <env.h>
#include <string.h>

#define _POSIX_SOURCE
#include <sys/wait.h>

#define MIN_ARGS (0)
#define MAX_ARGS (INT_MAX)
#define MAX_NAME_LEN (8)
#define MAX_NAME_LEN_STR "8"
#define MAX_ARGS_LENGTH (100)
#define MAX_ARGS_LENGTH_STR "100"

#define PROGNAME_ARG (1)
#define FIRST_PROG_ARG (PROGNAME_ARG+1)

#define PROG_NAME "mvsjobs"

#define LONG_OPT_PREFIX "--"
#define LONG_OPT_PREFIX_LEN (sizeof(LONG_OPT_PREFIX)-1)

#define SHORT_OPT_PREFIX "-"
#define SHORT_OPT_PREFIX_LEN (sizeof(SHORT_OPT_PREFIX)-1)

#define CONSOLE_NAME "*"

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

#define SYSTEM_FORMAT_SPECIFIER "PGM=%s,PARM='%s'"
#define MAX_SYSTEM_CMDLINE_LENGTH (MAX_ARGS_LENGTH + MAX_MEMBER_LEN + sizeof(SYSTEM_FORMAT_SPECIFIER))

#define USE_ATTACH_MVS 1

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

int hasMemberName(DSNameNode_T* dsNameNode) {
	return (dsNameNode->memName[0] != '\0');
}
void setNoMemberName(DSNameNode_T* dsNameNode) {
	dsNameNode->memName[0] = '\0';
}
void setMemberName(DSNameNode_T* dsNameNode, const char* memName, int len) {
	memcpy(dsNameNode->memName, memName, len);
	dsNameNode->memName[len] = '\0';	
}

struct DDNameList;
typedef struct DDNameList {
	struct DDNameList* next;
	DSNameList_T* dsName;
	char ddName[MAX_DDNAME_LEN+1];
	int isConsole:1;
} DDNameList_T;

typedef struct {
	const char* programName;
	const char* arguments;
	DDNameList_T* ddNameList;
	char* substitution;
	
	int debug:1;
	int verbose:1;
	int help:1;
} OptInfo_T;

typedef enum {
	AMODE24=1,
	AMODE31=2,
	AMODE64=3
} AddressingMode_T;

typedef struct {
	unsigned long long fp;
	unsigned int programSize;
	int APFAuthorized:1;
	AddressingMode_T amode;
	int rc;
} ProgramInfo_T;

typedef struct {
	unsigned short len;
	char str[1];
} VarStr_T;

typedef struct {
	unsigned short key;
	unsigned short numDatasets;
} SVC99ConcatenatedDatasetAllocation_T;

/*
 * The ProgramFailure_T enum and ProgramFailureMessage must be kept in sync.
 */
typedef enum {
	NoError=0,
	TooFewArgsSingular=1,
	TooFewArgsPlural=2,
	TooManyArgs=3,
	MVSNameTooLong=4,
	MVSNameInvalid=5,
	NoMVSName=6,
	ArgLengthTooLong=7,
	NoArgSpecified=8,
	UnrecognizedOption=9,
	IssueHelp=10,
	InternalOutOfMemory=11,
	NoDatasetName=12,
	DatasetNameTooLong=13,
	InvalidDatasetName=14,
	UnableToFetchProgram=15,
	UnableToEstablishEnvironment=16,
	DDNameAllocationFailure=17,
	ErrorPrintingDatasetToConsole=18,
	ErrorCallingProgram=19,
	ErrorDeletingTemporaryDataset=20,
	ErrorRunning64BitModuleWith31BitDriver=21,
	ErrorRunning31BitModuleWith64BitDriver=22,
	ErrorDubbingProgram=23
} ProgramFailure_T;

static const char* ProgramFailureMessage[] = {
	"",
	"At least %s argument must be specified.\n", 
	"At least %s arguments must be specified.\n", 
	"No more than %s arguments can be specified.\n",
	"Name %s is more than " MAX_NAME_LEN_STR " characters long.\n",
	"Name %s contains invalid characters.\n",
	"No Program name specified with program option.\n",
	"Program arguments more than " MAX_ARGS_LENGTH_STR " characters.\n",
	"No program arguments specified with argument option.\n",
	"Option not recognized.\n",
	"",
	"Out of Memory. Program ended abnormally.\n",
	"No dataset name specified for DDName %s.\n",
	"Dataset name %s is more than " MAX_DATASET_LEN_STR " characters long.\n", 
	"Invalid dataset name %s specified.\n", 
	"Unable to load program %s.\n",
	"Unable to establish environment.\n",
	"Unable to allocate all DDNames.\n",
	"Error printing dataset %s to console.\n",
	"Error calling program %s.\n",
	"Error deleting temporary dataset %s.\n",
	"Error running 64 bit module %s with mvscmd (use mvscmd64).\n",
	"Error running 31 bit module %s with mvscmd64 (use mvscmd).\n",	
	"Error dubbing module %s as process.\n"
};

struct Option;
typedef struct Option {
	ProgramFailure_T (*fn)(const char* argument, struct Option* opt, OptInfo_T* optInfo);
	const char* shortName;
	const char* longName;
	const char* defaultValue;
	const char* specifiedValue;
} Option_T;

static ProgramFailure_T allocDDNode(DDNameList_T** ddNameList) {
	DDNameList_T* newDDNode = malloc(sizeof(DDNameList_T));
	DSNameList_T* newDSNode = malloc(sizeof(DSNameList_T));
	if (newDDNode == NULL || newDSNode == NULL) {
		return InternalOutOfMemory;
	}
	newDDNode->dsName = newDSNode; 
	newDDNode->next = *ddNameList;
	*ddNameList = newDDNode;
	
	return NoError;
}

static void freeDDNode(DDNameList_T* node) {
	free(node);
}

static ProgramFailure_T allocDSNode(DSNameList_T* dsNameList) {
	DSNameNode_T* newNode = malloc(sizeof(DSNameNode_T));
	if (newNode == NULL) {
		return InternalOutOfMemory;
	}
	if (dsNameList->head == NULL) {
		dsNameList->head = newNode;
		dsNameList->tail = newNode;
		newNode->next = NULL;
	} else {
		dsNameList->tail->next = newNode;
		dsNameList->tail = newNode;
	}
	
	return NoError;
}

static void freeDSNode(DSNameNode_T* node) {
	free(node);
}

static ProgramFailure_T allocSubstitutionStr(OptInfo_T* optInfo, const char* value, size_t length) {
	char* newValue = malloc(length+1);
	if (newValue == NULL) {
		return InternalOutOfMemory;
	}
	memcpy(newValue, value, length);
	newValue[length] = '\0';
	optInfo->substitution = newValue;

	
	return NoError;
}

static void freeSubstitutionStr(OptInfo_T* optInfo) {
	free(optInfo->substitution);
	optInfo->substitution = NULL;
}

/*
 * Overly simplistic error message processing
 */
static void syntax(ProgramFailure_T reason, OptInfo_T* optInfo) {
	if (optInfo->substitution) {
		fprintf(stderr, ProgramFailureMessage[reason], optInfo->substitution);
		freeSubstitutionStr(optInfo);
	} else {
		fprintf(stderr, ProgramFailureMessage[reason]);
	}
	/*
	fprintf(stderr, "Syntax: %s <program> [<args>]\n", PROG_NAME);
	fprintf(stderr, " where <program> is the OS-program to run and <args> are the arguments to pass to the program\n");
	*/
	return;
}

static void uppercaseAndPad(char* output, const char* input, size_t pad) {
	int i;
	for (i=0; input[i] != '\0'; ++i) {
		if (islower(input[i])) {
			output[i] = toupper(input[i]);
		} else {
			output[i] = input[i];
		}
	}
	memset(&output[i], ' ', pad-i);
	output[pad] = '\0';
}	
static void uppercase(char* str) {
	int i;
	if (!str) {
		return;
	}
	for (i=0; str[i] != '\0'; ++i) {
		if (islower(str[i])) {
			str[i] = toupper(str[i]);
		} else {
			str[i] = str[i];
		}
	}
	str[i] = '\0';
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

static ProgramFailure_T validMVSName(const char* name) {
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

static ProgramFailure_T validProgramName(const char* name) {
	return validMVSName(name);
}

static ProgramFailure_T validDDName(const char* name) {
	return validMVSName(name);
}
static ProgramFailure_T validQualifierName(const char* name) {
	return validMVSName(name);
}
static ProgramFailure_T validMemberName(const char* name) {
	return validMVSName(name);
}

static ProgramFailure_T validDatasetName(const char* buffer, int start, int end, OptInfo_T* optInfo) {
	char qualifier[MAX_QUALIFIER_LEN+1];
	char member[MAX_MEMBER_LEN+1];
	int prevDot=start-1;
	int openParen=-1;
	int i;
	ProgramFailure_T rc = NoError;
	int len = end-start;

	if (len == 0) {
		return NoDatasetName;
	}

	for (i=start; i<=end; ++i) {
		if (buffer[i] == DOT_CHAR || buffer[i] == OPEN_PAREN_CHAR || buffer[i] == '\0' || buffer[i] == SEPARATOR_CHAR) {
			const char* qualifierName = &buffer[prevDot+1];
			if (i-prevDot-1 > MAX_QUALIFIER_LEN) {
				allocSubstitutionStr(optInfo, qualifierName, i-prevDot-1);
				return MVSNameTooLong;
			}
			if (i-prevDot == 1) {
				allocSubstitutionStr(optInfo, &buffer[start], len);
				return InvalidDatasetName;
			}
			memcpy(qualifier, qualifierName, i-prevDot-1);
			qualifier[i-prevDot-1] = '\0';
			
			rc = validQualifierName(qualifier);
			if (rc != NoError) {
				allocSubstitutionStr(optInfo, qualifier, i-prevDot-1);
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
			allocSubstitutionStr(optInfo, &buffer[start], len);
			return InvalidDatasetName;
		}
		if (openParen > MAX_DATASET_LEN) {
			allocSubstitutionStr(optInfo, &buffer[start], openParen-start);
			return DatasetNameTooLong;
		}
		if (buffer[end-1] != CLOSE_PAREN_CHAR) {
			allocSubstitutionStr(optInfo, &buffer[start], len);
			return InvalidDatasetName;
		}
		if (end-openParen-2 > MAX_MEMBER_LEN) {
			allocSubstitutionStr(optInfo, &buffer[openParen+1], end-openParen-2);
			return MVSNameTooLong;			
		}
		memcpy(member, &buffer[openParen+1], end-openParen-2);
		member[end-openParen-2] = '\0';
		rc = validMemberName(member);
		if (rc != NoError) {
			allocSubstitutionStr(optInfo, member, end-openParen-1);
			return rc;
		}
	} else {
		if (len > MAX_DATASET_LEN) {
			allocSubstitutionStr(optInfo, buffer, len);
			return DatasetNameTooLong;
		}		
	}	

	return NoError;
}	

static void copyDSNameAndMember(DSNameNode_T* entry, const char* buffer, int start, int end) {
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

static ProgramFailure_T processDebug(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	optInfo->debug = 1;
	return NoError;
}

static ProgramFailure_T processHelp(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	optInfo->help = 1;
	return NoError;
}

static ProgramFailure_T processPgm(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	ProgramFailure_T pf;
	
	if (value[0] != ASSIGN_CHAR) {
		return NoMVSName;
	}
	++value;
	pf = validMVSName(value);
	if (pf == NoError) {
		optInfo->programName = value;
	} else {
		allocSubstitutionStr(optInfo, value, strlen(value));
	}
	return pf;
}

static ProgramFailure_T processVerbose(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	optInfo->verbose = 1;
	return NoError;
}

static ProgramFailure_T processArgument(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	int valLen;
	if (value[0] != ASSIGN_CHAR) {
		return NoArgSpecified;
	}
	++value;
	
	valLen = strlen(value);
	
	if (valLen > MAX_ARGS_LENGTH) {
		return ArgLengthTooLong;
	}
	
	optInfo->arguments = value;
	return NoError;
}

static const char* generalOptCmp(const char* prefix, int prefixLen, const char* optName, const char* argument) {
	int argLen;
	int optLen;
	const char* nextChar;
	if (memcmp(prefix, argument, prefixLen)) {
		return NULL;
	}
	argLen = strlen(argument);
	optLen = strlen(optName);
	if (argLen < prefixLen + optLen) {
		return NULL;
	}
	if (memcmp(&argument[prefixLen], optName, optLen)) {
		return NULL;
	}
	nextChar = &argument[prefixLen+optLen];
	if (nextChar[0] == ASSIGN_CHAR || nextChar[0] == '\0') {
		return nextChar;
	}
	
	return NULL; 
}

static const char* shortOptCmp(const char* optName, const char* argument) {
	return generalOptCmp(SHORT_OPT_PREFIX, SHORT_OPT_PREFIX_LEN, optName, argument);
}

static const char* longOptCmp(const char* optName, const char* argument) {
	return generalOptCmp(LONG_OPT_PREFIX, LONG_OPT_PREFIX_LEN, optName, argument);
}


static ProgramFailure_T addDDName(const char* option, OptInfo_T* optInfo) {
	int optLen = strlen(option);
	ProgramFailure_T rc = allocDDNode(&optInfo->ddNameList);
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
		allocSubstitutionStr(optInfo, option, optLen);
		return UnrecognizedOption;
	}
	if (assignPos > MAX_DDNAME_LEN) {
		allocSubstitutionStr(optInfo, option, assignPos);
		return MVSNameTooLong;
	}

	memcpy(entry->ddName, option, assignPos);
	entry->ddName[assignPos] = '\0';
	rc = validDDName(entry->ddName);
	if (rc != NoError) {
		allocSubstitutionStr(optInfo, option, assignPos);
		return rc;
	}
	uppercase(entry->ddName);
	
	if (!strcmp(&option[assignPos+1], CONSOLE_NAME)) {
		rc = allocDSNode(entry->dsName);
		if (rc == NoError) {
			entry->isConsole = 1;
		}
		return rc;
	}

	dsNameStartPos=assignPos+1;
	while (1) {
		if (option[i] == '\0' || option[i] == ':') {
			rc = validDatasetName(option, dsNameStartPos, i, optInfo);
			if (rc == NoError) {
				rc = allocDSNode(entry->dsName);
				if (rc != NoError) {
					return rc;
				}
				copyDSNameAndMember(entry->dsName->tail, option, dsNameStartPos, i);
				if (option[i] == '\0') {
					break;
				}
				dsNameStartPos=i+1;
			} else {
				if (rc == NoDatasetName) {
					allocSubstitutionStr(optInfo, option, assignPos);
				} 
				freeDDNode(entry);
				break;
			}	
		}	
		++i;
	}
	return rc;
}

static ProgramFailure_T processArg(const char* argument, Option_T* opts, OptInfo_T* optInfo) {
	int argLen = strlen(argument);
	const char* val;
	ProgramFailure_T rc;
	
	while (opts->shortName != NULL) {
		if (argLen > SHORT_OPT_PREFIX_LEN && (val = shortOptCmp(opts->shortName, argument)) != NULL) {
			rc = opts->fn(val, opts, optInfo);
			return rc;
		} else if (argLen > LONG_OPT_PREFIX_LEN && (val = longOptCmp(opts->longName, argument)) != NULL) {
			rc = opts->fn(val, opts, optInfo);
			return rc;
		}		
		++opts;
	}
	if (argLen > LONG_OPT_PREFIX_LEN && !memcmp(argument, LONG_OPT_PREFIX, LONG_OPT_PREFIX_LEN)) {
		rc = addDDName(&argument[LONG_OPT_PREFIX_LEN], optInfo);
	} else {
		allocSubstitutionStr(optInfo, argument, argLen);
		rc = UnrecognizedOption;
	}
	return rc;
}

static const char* prtStrPointer(const char* p) {
	if (p == NULL) {
		return "null";
	} else {
		return p;
	}
}

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

static ProgramFailure_T callProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
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

static ProgramFailure_T loadProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
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

static int allocPDSMemberReadOnly(OptInfo_T* optInfo, char* ddName, char* dsName, char* memName) {
   __dyn_t ip;
   int rc;

   dyninit(&ip);

   ip.__ddname = ddName; 
   ip.__dsname = dsName; 
   ip.__member = memName;
   ip.__dsorg  = __DSORG_PS;
   ip.__status = __DISP_SHR;   

   errno = 0;
   rc = dynalloc(&ip); 
   if (rc) {
   		if (optInfo->verbose) {
   			fprintf(stdout, "PDS Member %s(%s) dynalloc failed for DDName %s with errno %d error code 0x%x, info code %d\n", dsName, memName, ddName, errno, ip.__errcode, ip.__infocode);
   		}
	} else {
   		if (optInfo->verbose) {
   			fprintf(stdout, "Dynalloc succeeded for %s=%s(%s)\n", ddName, dsName, memName);
   		}
   	}
	return rc;
}

static int allocPDSReadOnly(OptInfo_T* optInfo, char* ddName, char* dsName) {
   __dyn_t ip;
   int rc;

   dyninit(&ip);

   ip.__ddname = ddName; 
   ip.__dsname = dsName; 
   ip.__dsorg  = __DSORG_PO;
   ip.__status = __DISP_SHR;   


	errno = 0;
	rc = dynalloc(&ip); 
	if (rc) {
		if (optInfo->verbose) {
			fprintf(stdout, "PDS dynalloc failed with errno %d error code %d, info code %d\n", errno, ip.__errcode, ip.__infocode);
		}
	} else {
   		if (optInfo->verbose) {
   			fprintf(stdout, "Dynalloc succeeded for %s=%s\n", ddName, dsName);
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

	DSNameNode_T* node = ddNameList->dsName->head;
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
		if (optInfo->verbose) {
			fprintf(stdout, "SYSOUT dynalloc failed for %s=%s (sysout) with errno %d error code %d, info code %d\n", ddNameList->ddName, node->dsName, errno, ip.__errcode, ip.__infocode);
		}
	} else {
   		if (optInfo->verbose) {
   			fprintf(stdout, "Dynalloc succeeded for %s (temporary dataset for console)\n", ddNameList->ddName);
   		}
   	}
	return rc;
}
	
static void printProgramInfo(const char* programName, const char* arguments) {
	fprintf(stdout, "Program: <%s> Arguments: <%s>\n", prtStrPointer(programName), prtStrPointer(arguments));
}

static void printDDNames(DDNameList_T* ddNameList) {	
	fprintf(stdout, "DDNames:\n");
	if (ddNameList == NULL) {
		fprintf(stdout, "  No DDNames specified\n");
	}
	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			fprintf(stdout, "  %s=*\n", ddNameList->ddName);	
		} else {
			DSNameNode_T* dsNameNode = ddNameList->dsName->head;
			fprintf(stdout, "  %s=", ddNameList->ddName);
			while (dsNameNode != NULL) {
				if (hasMemberName(dsNameNode)) {
					fprintf(stdout, "%s(%s)", dsNameNode->dsName, dsNameNode->memName);
				} else {
					fprintf(stdout, "%s", dsNameNode->dsName);
				}	
				dsNameNode = dsNameNode->next;
				if (dsNameNode != NULL) {
					fprintf(stdout, ":");
				} else {
					fprintf(stdout, "\n");
				}
			}
		}
		ddNameList = ddNameList->next;
	}
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

static ProgramFailure_T establishEnvironment() {
	if (setenv("__POSIX_TMPNAM", "NO", 1)) {
		return UnableToEstablishEnvironment;
	}
	if (setenv("__POSIX_SYSTEM", "NO", 1)) {
		return UnableToEstablishEnvironment;
	}		
	return NoError;
}

static ProgramFailure_T processArgs(int argc, char* argv[], OptInfo_T* optInfo) {
	Option_T options[] = {
		{ &processHelp, "?", "help" }, 
		{ &processDebug, "d", "debug" }, 
		{ &processPgm, "p", "pgm" }, 
		{ &processArgument, "a", "args" }, 
		{ &processVerbose, "v", "verbose"},
		{ NULL, NULL, NULL }
	};
	ProgramFailure_T rc;
	int i;
	
	if (argc < MIN_ARGS+1) {
		if (MIN_ARGS == 1) {
			syntax(TooFewArgsSingular, optInfo);
		} else {
			syntax(TooFewArgsPlural, optInfo);
		}
		return TooFewArgsPlural;
	}
	if (argc > MAX_ARGS) {
		syntax(TooManyArgs, optInfo);
		return TooManyArgs;
	}	
	for (i=1; i<argc; ++i) {
		rc = processArg(argv[i], options, optInfo);
		if (rc != NoError) {
			syntax(rc, optInfo);
			return rc;
		}
		if (optInfo->help) {
			syntax(IssueHelp, optInfo);
			return IssueHelp;
		}
	}
	return NoError;
}

static int allocConcatenationReadOnly(OptInfo_T* optInfo, const char* ddName, DSNameList_T* dsNameList) {
	DSNameNode_T* cur = dsNameList->head;
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
			strcpy(cur->tempDDName, ddName);
		} else {
			strcpy(cur->tempDDName, "????????"); 
		}
		if (hasMemberName(cur)) {
			rc = allocPDSMemberReadOnly(optInfo, cur->tempDDName, cur->dsName, cur->memName);
		} else {
			rc = allocPDSReadOnly(optInfo, cur->tempDDName, cur->dsName);
		}
		if (optInfo->verbose) {
			fprintf(stdout, "DDName %s allocated to Dataset %s\n", cur->tempDDName, cur->dsName);
		}
		if (rc) {
			if (rc > maxRC) {
				maxRC = rc;
			}
			fprintf(stderr, "Error allocating dataset %s\n", cur->dsName);
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
	
	cur = dsNameList->head;
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
		if (optInfo->verbose) {
			fprintf(stderr, "Error code = %d Information code = %d\n", parmlist.__S99ERROR, parmlist.__S99INFO);
		}
		fprintf(stderr, "Error allocating concation for DDName %s\n", ddName);		
	}
	return rc;
}

static ProgramFailure_T establishDDNames(OptInfo_T* optInfo) {
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
		} else {
			if (ddNameList->dsName->head->next == NULL) {
				if (hasMemberName(ddNameList->dsName->head)) {
					rc = allocPDSMemberReadOnly(optInfo, ddNameList->ddName, ddNameList->dsName->head->dsName, ddNameList->dsName->head->memName);
				} else {
					rc = allocPDSReadOnly(optInfo, ddNameList->ddName, ddNameList->dsName->head->dsName);
				}	
			} else {
				rc = allocConcatenationReadOnly(optInfo, ddNameList->ddName, ddNameList->dsName);
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

static ProgramFailure_T printToConsole(OptInfo_T* optInfo) {
	DDNameList_T* ddNameList = optInfo->ddNameList;
	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			if (printDatasetToConsole(ddNameList->dsName->head->dsName)) {
				optInfo->substitution = ddNameList->dsName->head->dsName;
				syntax(ErrorPrintingDatasetToConsole, optInfo); 
				return ErrorPrintingDatasetToConsole;
			}
		}
		ddNameList = ddNameList->next;
	}
	return NoError;
}

static ProgramFailure_T removeConsoleFiles(OptInfo_T* optInfo) {
	char posixName[MAX_DATASET_LEN+5];
	DDNameList_T* ddNameList = optInfo->ddNameList;
	while (ddNameList != NULL) {
		if (ddNameList->isConsole) {
			CIODatasetName(ddNameList->dsName->head->dsName, posixName);
			if (optInfo->debug) {
				fprintf(stdout, "Temporary Dataset %s retained for debug\n", ddNameList->dsName->head->dsName);
			} else {
				if (remove(posixName)) {
					optInfo->substitution = ddNameList->dsName->head->dsName;
					syntax(ErrorDeletingTemporaryDataset, optInfo); 
					return ErrorDeletingTemporaryDataset;
				}
			}
		}
		ddNameList = ddNameList->next;
	}
	return NoError;
}

int main(int argc, char* argv[]) {
	OptInfo_T optInfo = { "IEFBR14", "", NULL, NULL, 0, 0 };
	ProgramInfo_T progInfo = { 0, 0, 0, 0, 0 };

	ProgramFailure_T rc;
	
	rc = establishEnvironment();
	if (rc != NoError) {
		syntax(rc, &optInfo);
		return 16;
	}
	
	rc = processArgs(argc, argv, &optInfo);
	if (rc != NoError) {
		return 8;
	}
	
	rc = establishDDNames(&optInfo);
	if (rc != NoError) {
		return 8;
	}
	rc = loadProgram(&optInfo, &progInfo);
	if (rc != NoError) {
		return rc;
	}
	
	rc = callProgram(&optInfo, &progInfo);
	if (rc != NoError) {
		return 16;
	}	
	
	rc = printToConsole(&optInfo);
	if (rc != NoError) {
		return 8;
	}
	rc = removeConsoleFiles(&optInfo);
	if (rc != NoError) {
		return 8;
	}
	
	if (rc == NoError) {
		return progInfo.rc;
	} else {
		syntax(rc, &optInfo);
		return rc;
	}	
}
	
