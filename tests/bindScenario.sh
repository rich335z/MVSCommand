#
# Program does a simple bind of an object file, using HEWL.
# Various timestamps are suppressed using awk !/.../
#
#set -x

tso alloc dsn\("'"${TESTHLQ}".MVSCMD.BIND.LOAD'"\) recfm\(u\) lrecl\(0\) blksize\(32760\) dsorg\(po\) dsntype\(library\) catalog tracks space\(10,10\) >/dev/null 2>&1
. setcc bindProgram;
mvscmd -v --pgm="HEWL" --args="MAP" --syslin=${TESTHLQ}.mvscmd.bind.obj\(bind\) --syslmod=${TESTHLQ}.mvscmd.bind.load\(bind\) --sysprint=* --syslib=${LEHLQ}.SCEELKED:${LEHLQ}.SCEELKEX:${LEHLQ}.SCEECPP | awk '!/^1z\/OS V2 R2 BINDER|BATCH EMULATOR|TIME OF SAVE/' | /bin/sed -e 's/ \+VOLUME SERIAL \+[^ ]\+/ VOLUME SERIAL ******/'
. unsetcc
(tsocmd delete "'"${TESTHLQ}".MVSCMD.BIND.LOAD'") >/dev/null 2>&1
