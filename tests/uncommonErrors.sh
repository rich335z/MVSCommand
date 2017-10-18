. setcc ConcatenationError
mvscmd -v --joblib=SYS1.PARMLIB
. unsetcc
. setcc SteplibError # Note - we do not let you set up STEPLIB - it is forced off right now
mvscmd -v --steplib=${LEHLQ}.SCEERUN
. unsetcc

