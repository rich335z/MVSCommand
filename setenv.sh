#
# To be called from createTests and runTests to set up the environment
# Modify this file to match your z/OS system
#
export MVSCOMMAND_ROOT=/u/ibmuser/MVSCommand   # Location where mvscmd installed
export TESTHLQ=IBMUSER                         # High Level qualifier test datasets created under
export LEHLQ=CEE                               # High Level qualifier for LE datasets (SCEELKED, SCEELKEX, SCEERUN, SCEERUN2)
export CHLQ=CBC                                # High Level qualifier for C/C++ compiler datasets (SCCNCMP)
export COBOLHLQ=IGY520                         # High Level qualifier for COBOL compiler datasets (SIGYCOMP)
export PLIHLQ=IEL450                           # High Level qualifier for PL/I compiiler datasets (SIBMZCMP)

export PATH=$MVSCOMMAND_ROOT/bin:$PATH
