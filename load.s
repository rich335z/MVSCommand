LOAD    CSECT
LOAD    AMODE 31
LOAD    RMODE ANY
PROLOG  EDCPRLG
*
        LR  4,1
        L   2,0(,4)
        LOAD EPLOC=0(,2)
        L   4,4(,4)
        ST  0,0(,4)
        LR  0,15
        LR  1,4
*        
EPILOG  EDCEPIL
        END
