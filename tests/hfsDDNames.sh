#
# Run the C compiler to test that HFS DD names work properly
# Then run DFSORT to test that output DD names properly have the 'WRONLY' set
# Then run tests to ensure that if files that -should- be for output are not writable that DFSORT fails
#
(
 export STEPLIB=${CHLQ}.SCCNCMP;
 . setcc cHFSCompilation;
 cp ${MVSCOMMAND_ROOT}/testsrc/main.c /tmp/mvscmd_chfs_main.c;
 chmod u-w /tmp/mvscmd_chfs_main.c;
 touch /tmp/mvscmd_chfs_main.o;
 mvscmd -v --pgm="CCNDRVR" --args="/CXX" --syslin=/tmp/mvscmd_chfs_main.o --sysin=/tmp/mvscmd_chfs_main.c --sysprint=*
 . unsetcc
)
touch /tmp/mvscmd_dfsort_ok.merge;
. setcc sortHFSOk;
mvscmd -v --pgm=sort --args=MSGPRT=CRITICAL,LIST --sortin01=${TESTHLQ}.mvscmd.dfsort.master --sortin02=${TESTHLQ}.mvscmd.dfsort.new --sysin=${TESTHLQ}.mvscmd.dfsort.cmd --sortout=/tmp/mvscmd_dfsort_ok.merge --sysout=* # ok because the output file is writable
. unsetcc;

touch /tmp/mvscmd_dfsort_fail.merge;
chmod u-w /tmp/mvscmd_dfsort_fail.merge;
. setcc sortHFSNoRW;
mvscmd --pgm=sort --args=MSGPRT=CRITICAL,LIST --sortin01=${TESTHLQ}.mvscmd.dfsort.master --sortin02=${TESTHLQ}.mvscmd.dfsort.new --sysin=${TESTHLQ}.mvscmd.dfsort.cmd --sortout=/tmp/mvscmd_dfsort_fail.merge --sysout=* | grep S614 # Error because the output file is not writable
. unsetcc;

# This is the longest valid file in POSIX (1024) but the maximum for DDName allocation is 255
touch /tmp/mvscmd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
. setcc HFSNameTooLong;
mvscmd -v --syslong=/tmp/mvscmd000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
. unsetcc

# Make sure 255 works ok
touch /tmp/mvscmd0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
. setcc HFSDDNameOk;
mvscmd -v --syslong=/tmp/mvscmd0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
. unsetcc

