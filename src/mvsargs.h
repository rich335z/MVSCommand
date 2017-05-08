#ifndef __MVSARGS__
	#define __MVSARGS__ 1
	
	#define MIN_ARGS (0)
	#define MAX_ARGS (INT_MAX)
	#define MAX_ARGS_LENGTH (100)
	#define MAX_ARGS_LENGTH_STR "100"
	
	#define PROGNAME_ARG (1)
	#define FIRST_PROG_ARG (PROGNAME_ARG+1)
	
	#define PROG_NAME "mvscmd"
	#define DEFAULT_MVSCMD "IEFBR14"
	
	#define LONG_OPT_PREFIX "--"
	#define LONG_OPT_PREFIX_LEN (sizeof(LONG_OPT_PREFIX)-1)
	
	#define SHORT_OPT_PREFIX "-"
	#define SHORT_OPT_PREFIX_LEN (sizeof(SHORT_OPT_PREFIX)-1)
	
	#define SYSTEM_FORMAT_SPECIFIER "PGM=%s,PARM='%s'"
	#define MAX_SYSTEM_CMDLINE_LENGTH (MAX_ARGS_LENGTH + MAX_MEMBER_LEN + sizeof(SYSTEM_FORMAT_SPECIFIER))
	
	#include "mvsdataset.h"
	
	typedef struct {
		const char* programName;
		const char* arguments;
		DDNameList_T* ddNameList;
		char* substitution;
		
		int debug:1;
		int verbose:1;
		int help:1;
	} OptInfo_T;
	
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
	
	struct Option;
	typedef struct Option {
		ProgramFailure_T (*fn)(const char* argument, struct Option* opt, OptInfo_T* optInfo);
		const char* shortName;
		const char* longName;
		const char* defaultValue;
		const char* specifiedValue;
	} Option_T;	

	ProgramFailure_T processArgs(int argc, char* argv[], OptInfo_T* optInfo);
	void syntax(ProgramFailure_T reason, OptInfo_T* optInfo);
	void printHelp();
	ProgramFailure_T allocSubstitutionStr(OptInfo_T* optInfo, const char* value, size_t length);
#endif