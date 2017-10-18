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
#ifndef __MVS_UTIL__
	#define __MVS_UTIL__ 1
	
	void uppercase(char* str);
	void uppercaseAndPad(char* output, const char* input, int pad);
	int strnocasecmp(const char* first, const char* second);
#endif