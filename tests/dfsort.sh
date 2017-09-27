#
# Merge 2 sequential datasets (master and new) and write them out to a new dataset (merge)
#

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.DFSORT.MERGE'"\) recfm\(f,b\) lrecl\(80\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(10,10\) >/dev/null 2>&1

. setcc dfSort;
mvscmd --pgm=sort --args='MSGPRT=CRITICAL,LIST' --sortin01=${TESTHLQ}.mvscmd.dfsort.master --sortin02=${TESTHLQ}.mvscmd.dfsort.new --sysin=${TESTHLQ}.mvscmd.dfsort.cmd --sortout=${TESTHLQ}.mvscmd.dfsort.merge --sysout=*
. unsetcc;
(tsocmd delete "'"${TESTHLQ}".MVSCMD.DFSORT.MERGE'") >/dev/null 2>&1
