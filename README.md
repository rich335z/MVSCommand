## Synopsis

This project enables z/OS developers to access MVS commands like IEBCOPY and IDCAMS from the Unix System Services environment.

## Code Example

Here is an example of **mvscmd** being used to copy dataset IBMUSER.TEST.C to IBMUSER.TEST.COPY, writing output from IEBCOPY to the screen

mvscmd --pgm=iebcopy --sysprint=* --sysin=dummy --sysut1=ibmuser.test.c --sysut2=ibmuser.test.copy

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
