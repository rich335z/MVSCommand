#
# This will create the test datasets and copy contents in from testsrc
# used by runTests.sh
#
. ./setenv.sh
(tsocmd delete "'"${TESTHLQ}".MVSCMD.BIND.OBJ'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IDCAMS.CMD'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.CMD'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IDCAMS.IN'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.IN'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.C'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.PLI'") >/dev/null 2>@1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL'") >/dev/null 2>@1

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.BIND.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IDCAMS.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IDCAMS.IN'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.IN'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.C'"\) recfm\(v,b\) lrecl\(255\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.PLI'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1

cd testsrc 

# Update the templates and cmd files to have the correct HLQ's

extension="expected"
tgtdir="../tests"
for f in *.template; do
  xx=$(basename ${f}  .template); sed -e "s/@@HLQ@@/${TESTHLQ}/" ${f} >${tgtdir}/${xx}.${extension}
done
sed "s/@@HLQ@@/${TESTHLQ}/" delete.template >delete.cmd
sed "s/@@HLQ@@/${TESTHLQ}/" define.template >define.cmd

# Copy the files from zFS into their respective datasets

cp bind.o "//'"${TESTHLQ}".MVSCMD.BIND.OBJ(BIND)'"
cp main.pli "//'"${TESTHLQ}".MVSCMD.PLI(MAIN)'"
cp main.cobol "//'"${TESTHLQ}".MVSCMD.COBOL(MAIN)'"
cp main.c "//'"${TESTHLQ}".MVSCMD.C(MAIN)'"
cp err.c "//'"${TESTHLQ}".MVSCMD.C(ERR)'"

cp iebcopy1.in "//'"${TESTHLQ}".MVSCMD.IEBCOPY.IN(IEBCOPY1)'"
cp iebcopy2.in "//'"${TESTHLQ}".MVSCMD.IEBCOPY.IN(IEBCOPY2)'"
cp iebcopy3.in "//'"${TESTHLQ}".MVSCMD.IEBCOPY.IN(IEBCOPY3)'"
cp idcams.in   "//'"${TESTHLQ}".MVSCMD.IDCAMS.IN(IDCAMS)'"

cp delete.cmd    "//'"${TESTHLQ}".MVSCMD.IDCAMS.CMD(DELETE)'"
cp define.cmd    "//'"${TESTHLQ}".MVSCMD.IDCAMS.CMD(DEFINE)'"
cp copysome.cmd  "//'"${TESTHLQ}".MVSCMD.IEBCOPY.CMD(COPYSOME)'"

