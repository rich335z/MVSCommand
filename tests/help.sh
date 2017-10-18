#
# test that help comes out using -? -h --help
#
. setcc HelpAbbrev
mvscmd -h
. unsetcc 
. setcc HelpLong
mvscmd --help
. unsetcc
. setcc infoAbbrev
mvscmd -?
. unsetcc
. setcc infoLong
mvscmd --info
. unsetcc
