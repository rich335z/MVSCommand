#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <dynit.h>
#include <stdlib.h>
#include <errno.h>

#define MIN_ARGS (0)
#define MAX_ARGS (INT_MAX)
#define MAX_NAME_LENGTH (8)
#define MAX_NAME_LENGTH_STR "8"
#define MAX_ARGS_LENGTH (100)
#define MAX_ARGS_LENGTH_STR "100"

#define PROGNAME_ARG (1)
#define FIRST_PROG_ARG (PROGNAME_ARG+1)

#define PROG_NAME "mvsjobs"

#define LONG_OPT_PREFIX "--"
#define LONG_OPT_PREFIX_LEN (sizeof(LONG_OPT_PREFIX)-1)

#define SHORT_OPT_PREFIX "-"
#define SHORT_OPT_PREFIX_LEN (sizeof(SHORT_OPT_PREFIX)-1)

#define ASSIGN_CHAR '='
#define DOT_CHAR '.'
#define OPEN_PAREN_CHAR '('
#define CLOSE_PAREN_CHAR ')'

#define MAX_DATASET_LEN (44)
#define MAX_DATASET_LEN_STR "44"
#define MAX_QUALIFIER_LEN   (8)
#define MAX_MEMBER_LEN      (8)

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
	UnableToFetchProgram=15
} ProgramFailure_T;

struct DDNameList;
typedef struct DDNameList {
	struct DDNameList* next;
	char* ddName;
	char* dsName;
	char* memName;
	char backingStorage[1];
} DDNameList_T;

#define DDNAME_LIST_SIZE(backingStore) (sizeof(DDNameList_T)+backingStore-1)

typedef struct {
	const char* programName;
	const char* arguments;
	DDNameList_T* ddNameList;
	char* substitution;
	
	int verbose:1;
	int help:1;
} OptInfo_T;

struct Option;
typedef struct Option {
	ProgramFailure_T (*fn)(const char* argument, struct Option* opt, OptInfo_T* optInfo);
	const char* shortName;
	const char* longName;
	const char* defaultValue;
	const char* specifiedValue;
} Option_T;

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

static const char* ProgramFailureMessage[] = {
	"",
	"At least %s argument must be specified.\n", 
	"At least %s arguments must be specified.\n", 
	"No more than %s arguments can be specified.\n",
	"Name %s is more than " MAX_NAME_LENGTH_STR " characters long.\n",
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
	"Unable to load program %s.\n"
};



static ProgramFailure_T allocNode(DDNameList_T** ddNameList, size_t length) {
	DDNameList_T* newNode = malloc(DDNAME_LIST_SIZE(length));
	if (newNode == NULL) {
		return InternalOutOfMemory;
	}
	newNode->next = *ddNameList;
	*ddNameList = newNode;
	
	return NoError;
}

static void freeNode(DDNameList_T* node) {
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
	if (len > MAX_NAME_LENGTH) {
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

static ProgramFailure_T validDatasetName(const char* name, OptInfo_T* optInfo) {
	char qualifier[MAX_QUALIFIER_LEN+1];
	char member[MAX_MEMBER_LEN+1];
	const int len = strlen(name);
	int prevDot=-1;
	int openParen=-1;
	int i;
	ProgramFailure_T rc = NoError;

	if (len == 0) {
		return NoDatasetName;
	}

	for (i=0; i<=len; ++i) {
		if (name[i] == DOT_CHAR || name[i] == OPEN_PAREN_CHAR || name[i] == '\0') {
			const char* qualifierName = &name[prevDot+1];
			if (i-prevDot-1 > MAX_QUALIFIER_LEN) {
				allocSubstitutionStr(optInfo, qualifierName, i-prevDot-1);
				return MVSNameTooLong;
			}
			if (i-prevDot == 1) {
				allocSubstitutionStr(optInfo, name, len);
				return InvalidDatasetName;
			}
			memcpy(qualifier, qualifierName, i-prevDot-1);
			qualifier[i-prevDot-1] = '\0';
			
			rc = validQualifierName(qualifier);
			if (rc != NoError) {
				allocSubstitutionStr(optInfo, qualifier, i-prevDot-1);
				return rc;
			}
			if (name[i] == DOT_CHAR) {
				prevDot = i;
			} else if (name[i] == OPEN_PAREN_CHAR) {
				openParen = i;
				break;
			}
		}
	}
	if (openParen >= 0) {
		if (openParen == 0) {
			allocSubstitutionStr(optInfo, name, len);
			return InvalidDatasetName;
		}
		if (openParen > MAX_DATASET_LEN) {
			allocSubstitutionStr(optInfo, name, openParen);
			return DatasetNameTooLong;
		}
		if (name[len-1] != CLOSE_PAREN_CHAR) {
			allocSubstitutionStr(optInfo, name, len);
			return InvalidDatasetName;
		}
		if (len-openParen-2 > MAX_MEMBER_LEN) {
			allocSubstitutionStr(optInfo, &name[openParen+1], len-openParen-2);
			return MVSNameTooLong;			
		}
		memcpy(member, &name[openParen+1], len-openParen-2);
		member[len-openParen-2] = '\0';
		rc = validMemberName(member);
		if (rc != NoError) {
			allocSubstitutionStr(optInfo, member, len-openParen-1);
			return rc;
		}
	} else {
		if (len > MAX_DATASET_LEN) {
			allocSubstitutionStr(optInfo, name, len);
			return DatasetNameTooLong;
		}		
	}	

	return NoError;
}	

static void establishMemNameIfRequired(DDNameList_T* entry) {
	int i;
	if (entry && entry->dsName) {
		for (i=0; entry->dsName[i] != '\0'; ++i) {
			if (entry->dsName[i] == OPEN_PAREN_CHAR) {
				entry->memName = &entry->dsName[i+1];          /* establish memname starting after the open-paren                     */
				entry->dsName[strlen(entry->dsName)-1] = '\0'; /* convert close-paren to a NULL so memname is a proper name           */
				entry->dsName[i] = '\0';                       /* convert open-paren to a NULL so dsname and memname separate strings */
				return;
			}
		}
		entry->memName = NULL;
	}
	return;
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

static ProgramFailure_T processArgs(const char* value, Option_T* opt, OptInfo_T* optInfo) {
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

static ProgramFailure_T addDDName(const char* option, OptInfo_T* optInfo) {
	int optLen = strlen(option);
	ProgramFailure_T rc = allocNode(&optInfo->ddNameList, optLen);
	int assignPos=-1, i;
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

	entry->ddName = entry->backingStorage;
	memcpy(entry->ddName, option, optLen+1);
	entry->ddName[assignPos] = '\0';
	entry->dsName = &entry->ddName[assignPos+1];
	
	rc = validDatasetName(entry->dsName, optInfo);

	if (rc == NoError) {
		establishMemNameIfRequired(entry);
		uppercase(entry->ddName);
		uppercase(entry->dsName);
		uppercase(entry->memName);
	} else {
		if (rc == NoDatasetName) {
			allocSubstitutionStr(optInfo, entry->ddName, assignPos);
			optInfo->substitution = entry->ddName;
		} 
		freeNode(entry);
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
		if (progInfo->programSize == 0) {
			fprintf(stdout, "Program is multi-segment program object or very large. Size unknown\n");
		} else {
			fprintf(stdout, "Program size is %d bytes\n", (progInfo->programSize << 3));
		}
		fprintf(stdout, "Program loaded at 0x%llX\n", progInfo->fp);
		fprintf(stdout, "Addressing mode: %s\n", prtAMode(progInfo->amode));
	}
}


#pragma linkage(LOAD, OS)
#pragma map(LOAD, "LOAD")
int LOAD(const char* module, unsigned int* info, unsigned long long* fp);
static ProgramFailure_T loadProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	unsigned long long fp = 0xFFFFFFFFFFFFFFFFLL;
	unsigned int info = 0xFFFFFFFF;
	char program[MAX_NAME_LENGTH+1];
	int i, rc;
	
	uppercaseAndPad(program, optInfo->programName, MAX_NAME_LENGTH);

	if (optInfo->verbose) {
		fprintf(stdout, "Load program %s\n", program);
	}
	rc = LOAD(program, &info, &fp);
	if (rc != 0) {
		if (optInfo->verbose) {
			fprintf(stdout, "Program load of %s failed with 0x%x\n", program, rc);
		}
		allocSubstitutionStr(optInfo, program, i);
		return UnableToFetchProgram;
	} else if (optInfo->verbose) {
		setprogInfo(optInfo, progInfo, fp, info);
	}
	return NoError;
}

typedef int (OS_FP)();
#pragma linkage(OS_FP, OS)
typedef _Packed struct { short length; char arguments[MAX_ARGS_LENGTH]; } OSOpts_T;

static ProgramFailure_T callProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo) {
	if (progInfo->amode != AMODE64) {
		OS_FP* fp = (OS_FP*) (progInfo->fp);
		OSOpts_T opts = { strlen(optInfo->arguments) };
		
		memcpy(opts.arguments, optInfo->arguments, opts.length);
		
		progInfo->rc = fp(&opts);
		
		if (optInfo->verbose) {
			fprintf(stdout, "%s run. Return code:%d\n", optInfo->programName, progInfo->rc);
		}
		
	} else {
		if (optInfo->verbose) {
			fprintf(stdout, "Only programs that are not 64-bit are supported. %s not run\n", optInfo->programName);
		}
	}
	return NoError;
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
   			fprintf(stdout, "Dynalloc failed with errno %d error code %d, info code %d\n", errno, ip.__errcode, ip.__infocode);
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
			fprintf(stdout, "Dynalloc failed with errno %d error code %d, info code %d\n", errno, ip.__errcode, ip.__infocode);
		}
	} else {
   		if (optInfo->verbose) {
   			fprintf(stdout, "Dynalloc succeeded for %s=%s\n", ddName, dsName);
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
		if (ddNameList->memName) {
			fprintf(stdout, "  %s=%s(%s)\n", ddNameList->ddName, ddNameList->dsName, ddNameList->memName);
		} else {
			fprintf(stdout, "  %s=%s\n", ddNameList->ddName, ddNameList->dsName);
		}				
		ddNameList = ddNameList->next;
	}
}

int main(int argc, char* argv[]) {
	Option_T options[] = {
		{ &processHelp, "?", "help" }, 
		{ &processPgm, "p", "pgm" }, 
		{ &processArgs, "a", "args" }, 
		{ &processVerbose, "v", "verbose"},
		{ NULL, NULL, NULL }
	};
	OptInfo_T optInfo = { "IEFBR14", NULL, NULL, NULL, 0, 0 };
	ProgramInfo_T progInfo = { 0, 0, 0, 0, 0 };
	DDNameList_T* ddNameList;

	int i, pf;
	ProgramFailure_T rc;
	
	if (argc < MIN_ARGS+1) {
		if (MIN_ARGS == 1) {
			syntax(TooFewArgsSingular, &optInfo);
		} else {
			syntax(TooFewArgsPlural, &optInfo);
		}
		return 16;
	}
	if (argc > MAX_ARGS) {
		syntax(TooManyArgs, &optInfo);
		return 16;
	}	
	for (i=1; i<argc; ++i) {
		pf = processArg(argv[i], options, &optInfo);
		if (pf != NoError) {
			syntax(pf, &optInfo);
			return 16;
		}
		if (optInfo.help) {
			syntax(IssueHelp, &optInfo);
			return 8;
		}
	}
	
	ddNameList = optInfo.ddNameList;
	if (optInfo.verbose) {
		printProgramInfo(optInfo.programName, optInfo.arguments);
		printDDNames(ddNameList);
	}

	while (ddNameList != NULL) {
		if (ddNameList->memName) {
			allocPDSMemberReadOnly(&optInfo, ddNameList->ddName, ddNameList->dsName, ddNameList->memName);
		} else {
			allocPDSReadOnly(&optInfo, ddNameList->ddName, ddNameList->dsName);
		}				
		ddNameList = ddNameList->next;
	}
	
	rc = loadProgram(&optInfo, &progInfo);
	if (rc != NoError) {
		syntax(rc, &optInfo);
		return rc;
	}
	
	rc = callProgram(&optInfo, &progInfo);
	if (rc == NoError) {
		return progInfo.rc;
	} else {
		syntax(rc, &optInfo);
		return rc;
	}	
}
	

