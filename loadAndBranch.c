#include <stdio.h>
#include <limits.h>
#include <ctype.h>

#define _EXT
#include <stdlib.h>


#define MIN_ARGS (0)
#define MAX_ARGS (INT_MAX)
#define MAX_NAME_LENGTH (8)
#define MAX_NAME_LENGTH_STR "8"
#define MAX_ARGS_LENGTH (256)
#define MAX_ARGS_LENGTH_STR "256"

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
	char* value;
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
	"Unable to fetch program %s.\n"
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
			qualifier[i-prevDot] = '\0';
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
		member[len-openParen-1] = '\0';
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
	entry->value  = &entry->ddName[assignPos+1];
	
	rc = validDatasetName(entry->value, optInfo);
	if (rc != NoError) {
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

static const char* printStrPointer(const char* p) {
	if (p == NULL) {
		return "null";
	} else {
		return p;
	}
}

#pragma linkage(LOAD, OS)
#pragma map(LOAD, "LOAD")
int LOAD(const char* module, unsigned int* fp);
static ProgramFailure_T loadProgram(OptInfo_T* optInfo) {
	unsigned int fp = 0xFFFFFFFF;
	char program[MAX_NAME_LENGTH+1];
	int i, rc;
	
	for (i=0; optInfo->programName[i] != '\0'; ++i) {
		if (islower(optInfo->programName[i])) {
			program[i] = toupper(optInfo->programName[i]);
		} else {
			program[i] = optInfo->programName[i];
		}
	}
	program[i] = '\0';
	if (optInfo->verbose) {
		fprintf(stdout, "Load program %s\n", program);
	}
	rc = LOAD(program, &fp);
	if (rc != 0) {
		allocSubstitutionStr(optInfo, program, i);
		return UnableToFetchProgram;
	} else if (optInfo->verbose) {
		fprintf(stdout, "Program loaded at 0x%X\n", fp);
	}
	return NoError;
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
	if (optInfo.verbose) {
		DDNameList_T* ddNameList = optInfo.ddNameList;	
		fprintf(stdout, "Program: <%s> Arguments: <%s>\n", printStrPointer(optInfo.programName), printStrPointer(optInfo.arguments));
		fprintf(stdout, "DDNames:\n");
		if (ddNameList == NULL) {
			fprintf(stdout, "  No DDNames specified\n");
		}
		while (ddNameList != NULL) {
			fprintf(stdout, "  %s=%s\n", ddNameList->ddName, ddNameList->value);
			ddNameList = ddNameList->next;
		}
	}
	
	rc = loadProgram(&optInfo);
	if (rc == NoError) {
		return 0;
	} else {
		return rc;
	}
}
	
