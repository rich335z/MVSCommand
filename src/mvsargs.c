/*******************************************************************************
 * Copyright (c) 2017 IBM.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Mike Fulton - initial implementation and documentation
 *******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <string.h>
#include "mvsmsgs.h"
#include "mvsdataset.h"
#include "mvsargs.h"

static ProgramFailureMsg_T processDebug(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	optInfo->debug = 1;
	return NoError;
}

static ProgramFailureMsg_T processHelp(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	optInfo->help = 1;
	return NoError;
}

static ProgramFailureMsg_T processPgm(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	ProgramFailureMsg_T pf;
	
	if (value[0] != ASSIGN_CHAR) {
		return NoMVSName;
	}
	++value;
	pf = validMVSName(value);
	if (pf == NoError) {
		optInfo->programName = value;
	} else {
		printError(pf, strlen(value), value, MAX_NAME_LEN);
	}
	return pf;
}

static ProgramFailureMsg_T processVerbose(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	optInfo->verbose = 1;
	return NoError;
}

static ProgramFailureMsg_T processArgument(const char* value, Option_T* opt, OptInfo_T* optInfo) {
	int valLen;
	if (value[0] != ASSIGN_CHAR) {
		printError(NoArgSpecified);
		return NoArgSpecified;
	}
	++value;
	
	valLen = strlen(value);
	
	if (valLen > MAX_ARGS_LENGTH) {
		printError(ArgLengthTooLong, MAX_ARGS_LENGTH);
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

static ProgramFailureMsg_T processArg(const char* argument, Option_T* opts, OptInfo_T* optInfo) {
	int argLen = strlen(argument);
	const char* val;
	ProgramFailureMsg_T rc;
	
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
		printError(UnrecognizedOption, argument);
		rc = UnrecognizedOption;
	}
	return rc;
}

ProgramFailureMsg_T processArgs(int argc, char* argv[], OptInfo_T* optInfo) {
	Option_T options[] = {
		{ &processHelp, "?", "help" }, 
		{ &processHelp, "h", "info" }, 		
		{ &processDebug, "d", "debug" }, 
		{ &processPgm, "p", "pgm" }, 
		{ &processArgument, "a", "args" }, 
		{ &processVerbose, "v", "verbose"},
		{ NULL, NULL, NULL }
	};
	ProgramFailureMsg_T rc;
	int i;
	
#if MIN_ARGS > 0	
	if (argc < MIN_ARGS+1) {
		if (MIN_ARGS == 1) {
			printError(TooFewArgsSingular);
		} else {
			printError(TooFewArgsPlural);
		}
		return TooFewArgsPlural;
	}
#endif
#if MAX_ARGS < INT_MAX
	if (argc > MAX_ARGS) {
		printError(TooManyArgs);
		return TooManyArgs;
	}	
#endif
	for (i=1; i<argc; ++i) {
		rc = processArg(argv[i], options, optInfo);
		if (rc != NoError) {
			return rc;
		}
		if (optInfo->help) {
			return IssueHelp;
		}
	}
	return NoError;
}
