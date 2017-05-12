#
# This will create the test datasets and copy contents in from testsrc
# used by runTests.sh
#
. ./setenv.sh
(tsocmd delete "'"${TESTHLQ}".MVSCMD.BIND.OBJ'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IDCAMS.CMD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.CMD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.SUPERCE.CMD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IDCAMS.IN'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.IEBCOPY.IN'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.SUPERCE.IN'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.C'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.PLI'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.COBOL'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.DFSORT.MASTER'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.DFSORT.NEW'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.DFSORT.CMD'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.ADRDSU.CMD'") >/dev/null 2>&1

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.BIND.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IDCAMS.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.SUPERCE.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IDCAMS.IN'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.IEBCOPY.IN'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.SUPERCE.IN'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.C'"\) recfm\(v,b\) lrecl\(255\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.PLI'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.COBOL'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.DFSORT.MASTER'"\) recfm\(f,b\) lrecl\(80\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.DFSORT.NEW'"\) recfm\(f,b\) lrecl\(80\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.DFSORT.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.ADRDSU.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(ps\) dsntype\(basic\) catalog tracks space\(10,10\) >/dev/null 2>&1

cd testsrc 

# Update the templates and cmd files to have the correct HLQ's

extension="expected"
tgtdir="../tests"
for f in *.template; do
  xx=$(basename ${f}  .template); sed -e "s/@@HLQ@@/${TESTHLQ}/g" ${f} >${tgtdir}/${xx}.${extension}
done
extension="cmd"
tgtdir="./"
for f in *.cmdtemplate; do
  xx=$(basename ${f}  .cmdtemplate); sed -e "s/@@HLQ@@/${TESTHLQ}/g" ${f} >${tgtdir}/${xx}.${extension}
done

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
cp oldfile.in  "//'"${TESTHLQ}".MVSCMD.SUPERCE.IN(OLDFILE)'"
cp newfile.in  "//'"${TESTHLQ}".MVSCMD.SUPERCE.IN(NEWFILE)'"

cp delete.cmd    "//'"${TESTHLQ}".MVSCMD.IDCAMS.CMD(DELETE)'"
cp define.cmd    "//'"${TESTHLQ}".MVSCMD.IDCAMS.CMD(DEFINE)'"
cp superce.cmd   "//'"${TESTHLQ}".MVSCMD.SUPERCE.CMD(SUPERCE)'"
cp copysome.cmd  "//'"${TESTHLQ}".MVSCMD.IEBCOPY.CMD(COPYSOME)'"

cp dfsort.cmd     "//'"${TESTHLQ}".MVSCMD.DFSORT.CMD'"
cp dfsort.master  "//'"${TESTHLQ}".MVSCMD.DFSORT.MASTER'"
cp dfsort.new     "//'"${TESTHLQ}".MVSCMD.DFSORT.NEW'"

cp adrdsu.cmd     "//'"${TESTHLQ}".MVSCMD.ADRDSU.CMD'"
