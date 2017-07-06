. ./setenv.sh 
. ./setoptenv.sh
cd opttests
rm -f *.actual
tests=*.sh
for test in ${tests}; do
	echo ${test}
	name="${test%.*}"
	${test} >${name}.actual 2>&1
	diff -w ${name}.actual ${name}.expected
done
