#
# Test binding
#
#set -x

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.BIND.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
mvscmd -v --pgm="HEWL" --args="MAP" --syslin=${TESTHLQ}.mvscmd.bind.obj\(bind\) --syslmod=${TESTHLQ}.mvscmd.bind.load\(bind\) --sysprint=* --syslib=${LEHLQ}.SCEELKED:${LEHLQ}.SCEELKEX:${LEHLQ}.SCEECPP | awk '!/^1z\/OS V2 R2 BINDER|BATCH EMULATOR|TIME OF SAVE/'
(tsocmd delete "'"${TESTHLQ}".MVSCMD.BIND.LOAD'") >/dev/null 2>&1
