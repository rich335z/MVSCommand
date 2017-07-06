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
export AUTHHLQ=CEE                            # High Level qualifier where authorized version of MVSCMD written
export AUTHSFX=SCEERUN2                         # Dataset suffix where authorized version of MVSCMD written

export PATH=$MVSCOMMAND_ROOT/bin:$PATH
export STEPLIB=${CHLQ}.SCCNCMP:$STEPLIB
