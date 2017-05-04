export PATH=$MVSCOMMAND_ROOT:$PATH
cd tests
rm -f *.actual
tests=*.sh
for test in ${tests}; do
	echo ${test}
	name="${test%.*}"
	${test} >${name}.actual 2>&1
	diff ${name}.actual ${name}.expected
done