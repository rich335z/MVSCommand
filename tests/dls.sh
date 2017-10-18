#
# simple program that lists datasets that start with a prefix (enter the prefix in uppercase) 
#
if [ -z "$1" ]; then
	datasetprefix=${TESTHLQ}.MVSCMD.DLS
else
	datasetprefix=$1;
fi
(
 echo " LISTCAT -\n LEVEL(${datasetprefix}\n" | 
   mvscmdauth --pgm=idcams --sysprint=* --sysin=stdin | 
   awk '/0NONVSAM/ { print $3 }'
)
