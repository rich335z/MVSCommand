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
#include <stdio.h>
#include <stdarg.h>
#include "mvsmsgs.h"




static const char* ProgramFailureMessage[] = {
	"",
	"At least %s argument must be specified.\n", 
	"At least %s arguments must be specified.\n", 
	"No more than %s arguments can be specified.\n",
	"Name %.*s is more than %d characters long.\n",
	"Name %.*s contains invalid characters.\n",
	"No Program name specified with program option.\n",
	"Program arguments more than %d characters.\n",
	"No program arguments specified with argument option.\n",
	"Option not recognized.\n",
	"",
	"Out of Memory. Program ended abnormally.\n",
	"No dataset name specified for DDName %.*s.\n",
	"Dataset name %.*s is more than %d characters long.\n", 
	"Invalid dataset name %.*s specified.\n", 
	"Unable to load program %s.\n",
	"Unable to establish environment.\n",
	"Unable to allocate all DDNames.\n",
	"Error printing dataset %s to console.\n",
	"Error calling program %s.\n",
	"Error deleting temporary dataset %s=%s.\n",
	"Unable to run 64 bit module %s with %s (use %s64).\n",
	"Unable to run 31 bit module %s with %s64 (use %s).\n",	
	"Error dubbing module %s as process.\n",
	"Invalid dataset option %s specified.\n", 
	"Error allocating STEPLIB %s.\n",
	"Error allocating DDName %s to CONSOLE (backing temporary dataset %s).\n", 
	"Error allocating DDName %s concatenation.\n",
	"Error allocating DDName %s to Dataset %s.\n",
	"Error allocating DDName %s to PDS Member %s(%s).\n",
	"Error allocating DDName %s to DUMMY.\n",
	"Error waiting for PID %d to complete.\n",
	"Child process did not complete correctly for %s.\n",
	"Error allocating DDName %s to (probably) non-existant dataset %s.\n", 
	"Error Codes allocating dataset: error:0x%X info:0x%X.\n",
	"Unable to call unauthorized program %s from %s. Use %s.\n",
	"Unable to call authorized program %s from %s. Use %s.\n",
	"HFS file %s can not be read.\n",
	"HFS file %.*s too long.\n",
	"Error reading from stdin to %s.\n",	
	"Error writing record %d to stdin temporary dataset %s.\n"
};

static const char* ProgramInfoMessage[] = {
	"",
	"Check validity of qualifier <%s>\n",
	"Check validity of member name <%s>\n",
	"PDS Member allocation succeeded for %s=%s(%s)\n",
	"Dataset allocation succeeded for %s=%s\n",
	"Dynamic allocation succeeded for %s (temporary dataset for console)\n",
	"DDName %s allocated to Concatenated Dataset %s (%d)\n",
	"DUMMY Dynalloc succeeded for %s=DUMMY\n",
	"Program: <%s> Arguments: <%s>\n",
	"DDNames:\n",
	"  No DDNames specified\n",
	"  %s=*\n",
	"  %s=DUMMY\n",
	"  %s=",
	"%s(%s)",
	"%s",
	",excl",
	":",
	"\n",
	"Temporary Dataset %s=%s retained for debug\n",
	"Program is APF authorized\n",
	"Addressing mode: %s\n",
	"Program is multi-segment program object or very large. Size unknown\n",
	"Program size is %d bytes\n",
	"Program loaded at 0x%llX\n",
	"%s run. Return code:%d\n",
	"%s attached. Return val:%d code:%d reason:%d\n",
	"Waiting on pid:%d\n",
	"Attach Exit code: %d from %s\n",
	"OS Load program %s\n",
	"OS Load of %s failed with 0x%x\n",
	"Syntax: %s [<args>]\n", 
	" where <args> is one or more of the following:\n",
	" --help | -? (this help)\n",
	" --info | -h (this help)\n",
	" --pgm=<program-name> | -p=<program-name>  (the program to run, e.g. -p=iebcopy). Default is IEFBR14\n",
	" --args=<program-arguments> | -a=<program-arguments> (arguments to pass to the program, e.g. -a='MARGINS(1,72)'. Default is ''\n",
	" --verbose | -v (verbose messages). Default is off\n",
	" --debug | -d (even more verbose messages). Default is off.\n",
	" --<ddname>=<value>[,excl|,old] (specify a dataset, concatenated dataset, PDS member, console or dummy for the given ddname).\n",
	"  Dataset example: --sysin=IBMUSER.TEST.OBJ: allocate the DDName SYSIN to the dataset IBMUSER.TEST.OBJ\n",
	"  Concatenated dataset example: --syslib=CEE.SCEELKED:CEE.SCEELKEX: allocate the ddname SYSLIB to the dataset concatenation CEE.SCEELKED:CEE.SCEELKEX\n",
	"  Console example: --sysprint=*: allocate the DDName SYSPRINT to stdout (which can then be piped to other processes).\n",
	"  Console example (alternate): --sysprint=stdout: allocate the DDName SYSPRINT to stdout (which can then be piped to other processes).\n",	
	"  stdin example: --sysin=stdin: allocate the DDName SYSIN to an FB 80 temporary sequential dataset that has stdin written to it (stdin can be piped in from other processes).\n",	
	"  Dummy Dataset example: --sysin=dummy: allocate the DDName SYSIN to DUMMY\n",
	"  Dataset allocated as 'exclusive' (i.e. DISP=OLD) example: --archive=IBMUSER.MVSCMD.DAR,EXCL\n",
	" Note: DD-names and the keywords DUMMY, EXCL and OLD are case-insensitive. All other options are case-sensitive\n",
	" Example: Compare 2 PDS members 'old' and 'new' and write the output to stdout\n",
	"  echo \"   CMPCOLM 1:72\" | mvscmd --pgm=isrsupc --args=\"DELTAL,LINECMP\" --newdd=ibmuser.in\\(new\\) --olddd=ibmuser.in\\(old\\) --sysin=stdin --outdd=stdout\n",
	"Check option <%s>\n",
	"APF Authorization Information. My APF authorization: %d. Program Authorization: %d\n",
	"Dynamic allocation succeeded for %s (temporary dataset for stdin)\n",
	"  %s=stdin\n",
	"Record %d read from stdin is more than 80 bytes. Record truncated\n",
};


void printError(ProgramFailureMsg_T reason, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, reason);
	vfprintf(stderr, ProgramFailureMessage[reason], arg_ptr);
	va_end(arg_ptr);
}

void printInfo(ProgramInfoMsg_T reason, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, reason);
	vfprintf(stdout, ProgramInfoMessage[reason], arg_ptr);
	va_end(arg_ptr);
}

void printHelp(const char* progName) {
	printInfo(InfoSyntax01, progName);
	printInfo(InfoSyntax02);
	printInfo(InfoSyntax03);
	printInfo(InfoSyntax04);
	printInfo(InfoSyntax05);
	printInfo(InfoSyntax06);
	printInfo(InfoSyntax07);
	printInfo(InfoSyntax08);
	printInfo(InfoSyntax09);
	printInfo(InfoSyntax10);
	printInfo(InfoSyntax11);
	printInfo(InfoSyntax12);
	printInfo(InfoSyntax13);
	printInfo(InfoSyntax14);
	printInfo(InfoSyntax15);
	printInfo(InfoSyntax16);
	printInfo(InfoSyntax17);	
	printInfo(InfoSyntax18);	
	printInfo(InfoSyntax19);		
}	
