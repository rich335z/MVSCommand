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
#ifndef __MVSARGS__
	#define __MVSARGS__ 1
	
	#define MIN_ARGS (0)
	#define MAX_ARGS (INT_MAX)
	#define MAX_ARGS_LENGTH (100)
	
	#define PROGNAME_ARG (1)
	#define FIRST_PROG_ARG (PROGNAME_ARG+1)
	
	#define PROG_NAME(isAuth) ((isAuth) ? "mvscmdauth" : "mvscmd")
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
		
		int debug:1;
		int verbose:1;
		int help:1;
	} OptInfo_T;
	
	struct Option;
	typedef struct Option {
		ProgramFailureMsg_T (*fn)(const char* argument, struct Option* opt, OptInfo_T* optInfo);
		const char* shortName;
		const char* longName;
		const char* defaultValue;
		const char* specifiedValue;
	} Option_T;	

#endif