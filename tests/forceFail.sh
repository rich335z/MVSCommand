#
# Run paths that force a failure that could not otherwise be tested (set an environment variable to force the failure)
#
(export FAIL_SetEnvPOSIXTmpNam=1; . setcc ffPOSIXTmpNam; mvscmd; echo $? ; . unsetcc;)
(export FAIL_SetEnvPOSIXSystem=1; . setcc ffPOSIXSystem; mvscmd; echo $? ; . unsetcc;)
(export FAIL_ConsoleAllocation=1; . setcc ffConsoleAllocation; mvscmd --systerm=* 2>/tmp/$?.out; echo $?; cat /tmp/$?.out | sed "s/dataset ${TESTHLQ}.*)/dataset xxx)/g"; . unsetcc;) 
(export FAIL_PDSMemberAllocation=1; . setcc ffPDSMemberAllocation; mvscmd --mydd=${LEHLQ}.SCEERUN\(SOMEMEM\); echo $?)
(export FAIL_HFSAllocation=1; . setcc ffHFSAllocation; mvscmd --mydd=${MVSCOMMAND_ROOT}; echo $?)
(export FAIL_StdinAllocation=1; . setcc ffStdinAllocation; echo 'hi there' | mvscmd --mydd=stdin; echo $?)
(export FAIL_PrintDatasetToConsole=1; . setcc ffPrtToStdout;  mvscmd --mydd=*; echo $?)
(export FAIL_ConcatenationAllocation=1; . setcc ffConcatenationAllocation; mvscmd --mydd=${LEHLQ}.SCEERUN:${LEHLQ}.SCEERUN2; echo $?)
(export FAIL_DummyAllocation=1; . setcc ffDummyAllocation; mvscmd --mydd=dummy; echo $?)