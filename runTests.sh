. ./setenv.sh 
cd tests
rm -f *.actual

if [ -z $1 ] ; then
	testEnv.sh >testEnv.actual 2>&1
	diff -w testEnv.actual testEnv.expected >/dev/null
	if [[ $? -ne 0 ]]; then
		echo 'Basic environment test for mvscmd and mvscmdauth failed. Tests not run'
		exit $?
	fi
	tests=*.sh
else
	tests=${1}.sh
fi

for test in ${tests}; do
	echo ${test}
	name="${test%.*}"
	${test} >${name}.actual 2>&1
	diff -w ${name}.actual ${name}.expected
done
