## Synopsis

This project enables z/OS developers to access MVS commands like IEBCOPY and IDCAMS from the Unix System Services environment.

## Code Example

Here is an example of **mvscmd** being used to copy dataset IBMUSER.TEST.C to IBMUSER.TEST.COPY, writing output from IEBCOPY to the screen

mvscmd --pgm=iebcopy --sysprint=* --sysin=dummy --sysut1=ibmuser.test.c --sysut2=ibmuser.test.copy

Here is a short script I called `dgrep` to illustrate the value of **mvscmd** a bit better:

** dgrep: **
```
str=$1
datasetpattern=$2
datasets=`echo " LISTCAT -\n ENTRIES("${datasetpattern}")\n" | mvscmdauth --pgm=idcams --sysprint=* --sysin=stdin | awk '/0NONVSAM/ { print $3 }' `
for dataset in ${datasets}; do
	echo "SRCHFOR '"${str}"'\n" | mvscmd --pgm=isrsupc --args='SRCHCMP,ANYC,IDPRFX,NOSUMS,LONGLN,NOPRTCC' --newdd=${dataset} --outdd=* --sysin=stdin | awk '!/  ISRSUPC/'
done
```

**Sample Invocation:**
```
dgrep SYSIN “IBMUSER.DB2ADMIN.*”
```

**Sample Output:**
```
 LINE-#  SOURCE SECTION                    SRCH DSN: IBMUSER.DB2ADMIN.JCL
 BINDTEP2                    --------- STRING(S) FOUND -------------------
     24  //SYSIN    DD *                                                         00650000
 BLDTEP2                     --------- STRING(S) FOUND -------------------
     42  //PPLI.SYSIN   DD DISP=SHR,
 CRTPHOTO                    --------- STRING(S) FOUND -------------------
     22  //SYSIN    DD  *
 CRTTBL                      --------- STRING(S) FOUND -------------------
      7  //SYSIN   DD DSN=IBMUSER.DB2ADMIN.SYSIN(CRTTBL),DISP=SHR
 LINE-#  SOURCE SECTION                    SRCH DSN: IBMUSER.DB2ADMIN.PLI
 DSNTEP2                     --------- STRING(S) FOUND -------------------
    214    *      Get input from SYSIN.  Write results to SYSPRINT.           *  02140000
    397    *      THE FOLLOWING ARE CONSIDERED VALID INPUT TO SYSIN:          *  03950000
 LINE-#  SOURCE SECTION                    SRCH DSN: IBMUSER.DB2ADMIN.PROC
 DSNHICOB                    --------- STRING(S) FOUND -------------------
     10  //SYSIN    DD  DISP=SHR,DSN=&INDSN(&MEM)
 DSNHPLI                     --------- STRING(S) FOUND -------------------
     27  //SYSIN    DD  DSN=&&DSNHIN,DISP=(OLD,DELETE)
```
## Motivation

I am not a big fan of JCL, even though I have worked on z/OS forever. 
I also try to use Unix System Services as much as possible to develop software. 
Unfortunately, if you want to run an MVS command, such as IDCAMS to work with VSAM datasets, you need to create JCL, submit JCL, 
poll until the JCL completes, then interpret the results from USS. This is annoying.
This made me wonder why we couldn't just run the MVS program from USS directly. 

## Installation

To install:
- copy the files to z/OS Unix System Services directory. For this example, we assume it is /u/ibmuser/MVSCommand
- cd to the directory (/u/ibmuser/MVSCommand)
- edit setenv.sh to point to the various MVS programs that will be tested, and to specify where your code was copied to. 
- edit build.sh if required to point to your C compiler and assembler, then run the script to build the program.
- run build.sh: build.sh
- The assemble, compile and link should be 'clean' and will produce a file called 'mvscmd' and an authorized version called 'mvscmdauth'
-  NOTE: You do not need to create mvscmdauth - but it is required for running authorized programds like ADRDSU and IKJEFT01

## API Reference

To get started reading the code, begin in mvscmd.c, which has 'main' and drives all the functions in the other files.

## Tests

To run the tests:
- cd to the directory (e.g. /u/ibmuser/MVSCommand)
- runTests.sh

This will write results to the screen as it runs the testcases in the tests sub-directory. At this point, you may see messages about the
volume label being different and if you did not create mvscmdauth, you will see failures for authorized programs like ADRDSU

## Contributors

Mike Fulton (IBM Canada) is the sole contributor at this point. I am happy to change this :)

The code still needs work. Error messages can (always) be improved and there are missing features people may need.
If anyone wants to contribute, please reach out to fultonm@ca.ibm.com (Mike Fulton)

## License

The code uses the Eclipse Public License 1.0 ( https://opensource.org/licenses/eclipse-1.0.php )
