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
#include <ctype.h>
#include <string.h>
#include "mvsutil.h"

void uppercase(char* str) {
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

void uppercaseAndPad(char* output, const char* input, int pad) {
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

int strnocasecmp(const char* first, const char* second) {
	int i=0;
	while (first[i] != '\0' && second[i] != '\0') {
		int fc = first[i];
		int sc = second[i];
		if (isalpha(fc)) {
			fc = toupper(fc);
		}
		if (isalpha(sc)) {
			sc = toupper(sc);
		}
		if (fc != sc) {
			return sc - fc;
		}
		++i;
	}
	return (first[i] - second[i]);
}