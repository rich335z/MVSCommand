#
# simple program that searches all the PDS members for a string, e.g. dgrep IEFPROC sys1.proclib
#
if [ -z "$1" ]; then
	str="SYSIN";
	datasetpattern=${TESTHLQ}.MVSCMD.*
else
	str=$1;
	datasetpattern=$2;
fi
datasets=`echo " LISTCAT -\n ENTRIES("${datasetpattern}")\n" | mvscmdauth --pgm=idcams --sysprint=* --sysin=stdin | awk '/0NONVSAM/ { print $3 }' `
for dataset in ${datasets}; do
	testid=`echo ${dataset} | tr '.' '_'`
	. setcc "search"${testid};
	echo "SRCHFOR '"${str}"'\n" | mvscmd --pgm=isrsupc --args='SRCHCMP,ANYC,IDPRFX,NOSUMS,LONGLN,NOPRTCC' --newdd=${dataset} --outdd=* --sysin=stdin | awk '!/  ISRSUPC/';
	. unsetcc
done
