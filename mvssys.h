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