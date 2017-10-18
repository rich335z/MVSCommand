#
# ensure that configuration is all working correctly
#
. setcc Unauthorized
mvscmd --help | grep Syntax # should give message about unauthorized syntax
. unsetcc
. setcc Authorization
mvscmdauth --help | grep Syntax # should give message about authorized syntax
. unsetcc
