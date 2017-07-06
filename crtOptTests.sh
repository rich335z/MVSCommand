#
# This will create the test datasets and copy contents in from testsrc
# used by runTests.sh
#
. ./setenv.sh
. ./setoptenv.sh

(tsocmd delete "'"${TESTHLQ}".MVSCMD.${DB2SSID}.COBOL'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.${DB2SSID}.OBJ'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.${DB2SSID}.LOAD'") >/dev/null 2>&1

(tsocmd delete "'"${TESTHLQ}".MVSCMD.${DB2SSID}.DB2PCGEN.COBOL'") >/dev/null 2>&1
(tsocmd delete "'"${TESTHLQ}".MVSCMD.${DB2SSID}.DBRMLIB.DATA'") >/dev/null 2>&1

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.${DB2SSID}.COBOL'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.${DB2SSID}.CMD'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.${DB2SSID}.OBJ'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.${DB2SSID}.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.${DB2SSID}.DB2PCGEN.COBOL'"\) recfm\(f,b\) lrecl\(80\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
tso alloc dsn\("'"${TESTHLQ}".MVSCMD.${DB2SSID}.DBRMLIB.DATA'"\) recfm\(f,b\) lrecl\(80\) blksize\(4000\) dsorg\(po\) dsntype\(library\) catalog tracks space\(40,40\) >/dev/null 2>&1

cd opttestsrc 

# Update the templates and cmd files to have the correct HLQ's

extension="expected"
tgtdir="../opttests"
for f in *.template; do
  xx=$(basename ${f}  .template); sed -e "s/@@HLQ@@/${TESTHLQ}/g" -e "s/@@DB2SSID@@/${DB2SSID}/g" ${f} >${tgtdir}/${xx}.${extension}
done

extension="cmd"
tgtdir="./"
for f in *.cmdtemplate; do
  xx=$(basename ${f}  .cmdtemplate); sed -e "s/@@HLQ@@/${TESTHLQ}/g" -e "s/@@DB2SSID@@/${DB2SSID}/g" ${f} >${tgtdir}/${xx}.${extension}
done

# Copy the files from zFS into their respective datasets

cp db2photo.cobol "//'"${TESTHLQ}".MVSCMD.${DB2SSID}.COBOL(DB2PHOTO)'"
cp db2bndphoto.cmd "//'"${TESTHLQ}".MVSCMD.${DB2SSID}.CMD(BNDPHOTO)'"
cp db2runphoto.cmd "//'"${TESTHLQ}".MVSCMD.${DB2SSID}.CMD(RUNPHOTO)'"

