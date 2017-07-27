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

#ifndef __MVSMSG__
	#define __MVSMSG__ 1
	
	/*
	 * The ProgramFailureMsg_T enum and ProgramFailureMessage must be kept in sync.
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
		ErrorDubbingProgram=23,
		InvalidDatasetOption=24, 
		ErrorAllocatingSTEPLIB=25,
		ErrorAllocatingCONSOLE=26,
		ErrorAllocatingConcatenation=27,
		ErrorAllocatingDataset=28,
		ErrorAllocatingPDSMember=29,
		ErrorAllocatingDUMMY=30,
		ErrorWaitingForPID=31,
		ErrorChildCompletion=32,
		ErrorAllocatingNonExistantDataset=33,
		ErrorCodesAllocatingDataset=34,
		ErrorCallingUnauthorizedFromAuthorized=35,
		ErrorCallingAuthorizedFromUnauthorized=36,
		ErrorUnableToReadHFSFile=37,
		ErrorHFSFileTooLong=38
	} ProgramFailureMsg_T;
	
	/*
	 * The ProgramInfo_T enum and ProgramInfoMessage must be kept in sync.
	 */
	typedef enum {
		InfoNone=0,
		InfoQualifierValidity=1,
		InfoMemberValidity=2,
		InfoPDSMemberAllocationSucceeded=3,
		InfoDatasetAllocationSucceeded=4,
		InfoConsoleDatasetAllocationSucceeded=5,
		InfoConcatenatedDatasetAllocationSucceeded=6,	
		InfoDummyDatasetAllocationSucceeded=7,
		InfoProgramName=8,
		InfoDDNameHeader=9,
		InfoNoDDNames=10,
		InfoConsoleDDName=11,
		InfoDummyDDName=12,
		InfoDDName=13,
		InfoPDSMemberName=14,
		InfoDatasetName=15,
		InfoExclusive=16,
		InfoConcatenationSeparator=17,
		InfoNewline=18,
		InfoTemporaryDatasetRetained=19,
		InfoAPFAuthorized=20,
		InfoAddressingMode=21,
		InfoProgramSizeUnknown=22,
		InfoProgramSize=23,
		InfoProgramLoadAddress=24,		
		InfoProgramReturnCode=25,				
		InfoProgramAttachInfo=26,	
		InfoWaitingOnPID=27,	
		InfoAttachExitCode=28,
		InfoLoadProgram=29,
		InfoLoadProgramDetails=30,
		InfoSyntax01=31,
		InfoSyntax02=32,
		InfoSyntax03=33,
		InfoSyntax04=34,
		InfoSyntax05=35,
		InfoSyntax06=36,
		InfoSyntax07=37,
		InfoSyntax08=38,
		InfoSyntax09=39,
		InfoSyntax10=40,
		InfoSyntax11=41,
		InfoSyntax12=42,
		InfoSyntax13=43,
		InfoSyntax14=44,
		InfoSyntax15=45,
		InfoSyntax16=46,
		InfoSyntax17=47,
		InfoCheckOption=48,
		InfoAPFAuthorization=49
	} ProgramInfoMsg_T;
	
	void printHelp(const char* pgmName);

	void printError(ProgramFailureMsg_T pfm, ...);
	void printInfo(ProgramInfoMsg_T pim, ...);
#endif	