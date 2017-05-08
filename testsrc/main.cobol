       Identification Division.
      *
       PROGRAM-ID.    PROGT.
      *
       Installation.  IBM - Silicon Valley Laboratory.
       Security.      Property of IBM Corporation.
      *LANGUAGE.      COBOL for OS/390
      *
      *****************************************************************
      ***                                                            **
      *** Function:                                                  **
      ***    Sample COBOL program used for DEBUG TOOL testing        **
      ***    (Single compile unit).                                  **
      ***                                                            **
      *****************************************************************
      *
       ENVIRONMENT DIVISION.
         CONFIGURATION SECTION.
          SOURCE-COMPUTER.  IBM-370.
          OBJECT-COMPUTER.  IBM-370.
         INPUT-OUTPUT SECTION.
          FILE-CONTROL.
      *
      *****************************************************************
      *
       DATA DIVISION.
         FILE SECTION.
         WORKING-STORAGE SECTION.
         01     VARBL1          PIC 99    VALUE 10.
         01     VARBL2          PIC 99    VALUE 20.
         01     R               PIC 9     VALUE 1.
         01     STR1            PIC X(5).
         01     STR2            PIC X(5).
         01     STR3            PIC X(5).
         01     STR4            PIC X(5).
         01     STR5            PIC X(5).
         LINKAGE SECTION.
      *
      *****************************************************************
      *
       PROCEDURE DIVISION.
      *  MAIN.
           MOVE "ONE"   TO STR1.
           MOVE "TWO"   TO STR2.
           MOVE "THREE" TO STR3.
           MOVE "FOUR"  TO STR4.
           MOVE "FIVE"  TO STR5.
           PERFORM UNTIL R = 5
             ADD 1 TO VARBL1
             SUBTRACT 2 FROM VARBL2
             MOVE "TOP" TO STR1
             MOVE "BEG" TO STR2
             MOVE "UP"  TO STR3
             ADD 1 TO R
             MOVE "BOT" TO STR1
             MOVE "END" TO STR2
             MOVE "DOW" TO STR3
           END-PERFORM.
           MOVE "DONE" TO STR1.
           MOVE "END"  TO STR2.
           MOVE "FIN"  TO STR3.
           STOP RUN.
