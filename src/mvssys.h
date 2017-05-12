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
#ifndef __MVSSYS__
	#define __MVSSYS__ 1
	
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
	
	ProgramFailure_T loadProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo);
	ProgramFailure_T callProgram(OptInfo_T* optInfo, ProgramInfo_T* progInfo);
#endif