**********************************************************************
*Copyright (c) 2017 IBM.
*All rights reserved. This program and the accompanying materials
*are made available under the terms of the Eclipse Public License v1.0
*which accompanies this distribution, and is available at
*http://www.eclipse.org/legal/epl-v10.html
*
*Contributors:
*   Mike Fulton - initial implementation and documentation
**********************************************************************
LOAD     CSECT
LOAD     AMODE 31
LOAD     RMODE ANY
PROLOG   EDCPRLG
*
         LR  4,1
         L   2,0(,4)
         LA  5,ERR_SVC
         LOAD EPLOC=0(,2),ERRET=ERR_SVC
POSTLOAD DS 0H
         L   6,4(,4)
         ST  1,0(,6)
         L   7,8(,4)
         STG 0,0(,7)
         LR  0,15
         LR  1,4
*         ST  0,0(,0)
*        
EPILOG  EDCEPIL
*
ERR_SVC DS 0F
        B  POSTLOAD
*        
ISAPFAUT CSECT
ISAPFAUT AMODE 31
ISAPFAUT RMODE ANY
         EDCPRLG
*
*   Issue TESTAUT to see if current task is running authorized
*
         TESTAUTH FCTN=1       see if running apf authorized
*                              store in R15 (0 authorized, 4 not)
*
         EDCEPIL
         DS    0D
         LTORG       
*
*
*
SETDUBDF CSECT
SETDUBDF AMODE 31
SETDUBDF RMODE ANY
SETDUBDP EDCPRLG
         CALL BPX1SDD
SETDUBDE EDCEPIL
*
*
*
ATTMVS   CSECT
ATTMVS   AMODE 31
ATTMVS   RMODE ANY
ATTMVSP  EDCPRLG
         CALL BPX1ATM        
ATTMVSE  EDCEPIL
*
* (all parms are full words)
*CALL BPX1SDD,(Dub_setting,
*              Return_value,
*              Return_code,
*              Reason_code)
*              
*DUBTHREAD            EQU   0    Dub a thread default setting     @L1A
*DUBPROCESS           EQU   1    Dub a process default setting    @L1A
*DUBTASKACEE          EQU   2    Dub a task ACEE setting          @02A
*DUBPROCESSDEFER      EQU   4    Dub a process - but defer dub    @04A
*DUBNOSIGNALS         EQU   8    Dub a process - no signals       @DWA
*DUBJOBPERM           EQU   16   Dub as a permanent Job           @DYA
*DUBNOJSTUNDUB        EQU   32   Dub process such that jobstep does not
*                 
*              
*Dub_setting is: DUBPROCESSDEFER
*
        CEECAA
        BPXYCONS
        END        