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
* CEEPLOD2 - load a program with CEE service
*        
CEEPLOD2 CSECT
CEEPLOD2 AMODE 31
CEEPLOD2 RMODE ANY
CEEPLODP EDCPRLG
*
         L   15,CEECAACELV-CEECAA(,12)
         L   15,3948(,15)
         BALR 14,15
*        
CEEPLODE EDCEPIL
*
        CEECAA
        END        