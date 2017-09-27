#
# To be called from createTests and runTests to set up the environment
# Modify this file to match your z/OS system
#
export MVSCOMMAND_ROOT=/u/ibmuser/MVSCommand   # Location where mvscmd installed
export TESTHLQ=IBMUSER                         # High Level qualifier test datasets created under
export LEHLQ=CEE                               # High Level qualifier for LE datasets (SCEELKED, SCEELKEX, SCEERUN, SCEERUN2)
export CHLQ=CBC                                # High Level qualifier for C/C++ compiler datasets (SCCNCMP)
export COBOLHLQ=IGY610                         # High Level qualifier for COBOL compiler datasets (SIGYCOMP)
export PLIHLQ=IEL510                           # High Level qualifier for PL/I compiler datasets (SIBMZCMP)
export DBGHLQ=EQAE00                           # High Level qualifier for Code Coverage (optional)
export AUTHHLQ=IBMUSER                         # High Level qualifier where authorized version of MVSCMD written
export AUTHSFX=MVSCMD.LOAD                     # Dataset suffix where authorized version of MVSCMD written

export PATH=${MVSCOMMAND_ROOT}/bin:$PATH
export DBGLIB=${DBGHLQ}.SEQAMOD:${TESTHLQ}.CEEV2R2.CEEBINIT
export STEPLIB=${AUTHHLQ}.${AUTHSFX}:$STEPLIB
