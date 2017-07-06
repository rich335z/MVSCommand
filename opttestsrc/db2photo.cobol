      *****************************************************************         
      * MODULE NAME = DSN8CLPV (DB2 SAMPLE PROGRAM)                   *         
      *                                                               *         
      * DESCRIPTIVE NAME = Display PSEG photo image of a specified    *         
      *                    employee                                   *         
      *                                                               *         
      *  LICENSED MATERIALS - PROPERTY OF IBM                         *         
      *  5615-DB2                                                     *         
      *  (C) COPYRIGHT 1982 2013 IBM CORP.  ALL RIGHTS RESERVED.      *         
      *                                                               *         
      *  STATUS = VERSION 11                                          *         
      *                                                               *         
      * Function:Prompts the user to choose an employee then retrieves*         
      *       the PSEG photo image for that employee from the PSEG -  *         
      *       PHOTO column of the EMP_PHOTO_RESUME table and passes it*         
      *       to GDDM for formatting and display.                     *         
      *                                                               *         
      * Notes:                                                        *         
      *   Dependencies:                                               *         
      *             Requires IBM Graphical Data Display Manager (GDDM)*         
      *                 V3R1 or higher                                *         
      *                                                               *         
      *   Restrictions:                                               *         
      * Module type: COBOL program                                    *         
      * Module size: See linkedit output                              *         
      *  Attributes: Re-entrant and re-usable                         *         
      *                                                               *         
      * Entry Point: CEESTART (Language Environment entry point)      *         
      *     Purpose: See Function                                     *         
      *     Linkage: Standard MVS program invocation no parameters    *         
      *                                                               *         
      * Normal Exit: Return Code = 0000                               *         
      *              - Message: none                                  *         
      *                                                               *         
      *  Error Exit: Return Code = 0008                               *         
      *              - Message: *** ERROR: DSN8CLPV DB2 Sample Program*         
      *                                 Unexpected SQLCODE encountered*         
      *                                    at location xxx            *         
      *                                    Error detailed below       *         
      *                                    Processing terminated      *         
      *                               (DSNTIAR-formatted message here)*         
      *                                                               *         
      *              - Message: *** ERROR: DSN8CLPV DB2 Sample Program*         
      *                                No entry in the Employee Photo/*         
      *                                Resume table for employee with *         
      *                                empno = xxxxxx                 *         
      *                                Processing terminated          *         
      *                                                               *         
      *              - Message: *** ERROR: DSN8CLPV DB2 Sample Program*         
      *                                No PSEG data exists in         *         
      *                                the Employee Photo/Resume table*         
      *                                for the employee with empno =  *         
      *                                xxxxxx.                        *         
      *                                Processing terminated          *         
      *                                                               *         
      *                                                               *         
      *    External References:                                       *         
      *             - Routines/Services: DSNTIAR, GDDM, ISPF          *         
      *             - Data areas       : DSNTIAR error_message        *         
      *             - CONTROL blocks  : None                         *          
      *                                                               *         
      *                                                               *         
      *  Pseudocode:                                                  *         
      *   DSN8CLPV:                                                   *         
      *   - Do until the user indicates termination                   *         
      *     - Call GETEMPLNUM to request an employee id               *         
      *     - Call GETEMPLPHOTO to retrieve the PSEG photo image      *         
      *     - Call SHOWEMPLPHOTO to display the photo                 *         
      *   End DSN8CLPV                                                *         
      *                                                               *         
      *                                                               *         
      *   GETEMPLNUM:                                                 *         
      *   -prompt user to select an employee whose photo image is to  *         
      *     be viewed                                                 *         
      *   End GETEMPLNUM                                              *         
      *                                                               *         
      *   GETEMPLPHOTO:                                               *         
      *   - Fetch the specified employee's PSEG photo image from DB2  *         
      *     - call D31100-CHECK-SQLCODE for unexpected SQLCODEs       *         
      *   End GETEMPLPHOTO                                            *         
      *                                                               *         
      *   SHOWEMPLPHOTO:                                              *         
      *   - Use GDDM calls to format and display the PSEG photo image *         
      *   End SHOWEMPLPHOTO                                           *         
      *                                                               *         
      *   D31100-CHECK-SQLCODE                                        *         
      *   - call DSNTIAR to format an unexpected SQLCODE.             *         
      *   End D31100-CHECK-SQLCODE                                    *         
      *                                                               *         
      *****************************************************************         
                                                                                
       IDENTIFICATION DIVISION.                                                 
       PROGRAM-ID. DSN8CLPV.                                                    
                                                                                
       ENVIRONMENT DIVISION.                                                    
       CONFIGURATION SECTION.                                                   
       SOURCE-COMPUTER.  IBM-370.                                               
       OBJECT-COMPUTER.  IBM-370.                                               
                                                                                
                                                                                
       DATA DIVISION.                                                           
                                                                                
                                                                                
       WORKING-STORAGE SECTION.                                                 
                                                                                
      *************************** ISPF Syntax **************************        
       01  CHAR         PIC X(8)   VALUE 'CHAR    '.                            
       01  VCONTROL     PIC X(8)   VALUE 'CONTROL '.                            
       01  VDISPLAY     PIC X(8)   VALUE 'DISPLAY '.                            
       01  VLINE        PIC X(8)   VALUE 'LINE    '.                            
       01  VDEFINE      PIC X(8)   VALUE 'VDEFINE '.                            
       01  VGET         PIC X(8)   VALUE 'VGET    '.                            
       01  VRESET       PIC X(8)   VALUE 'VRESET  '.                            
       01  DSN8SSE      PIC X(8)   VALUE 'DSN8SSE '.                            
       01  EMPLNUM      PIC X(8)   VALUE 'D8EMNUMB'.                            
       01  VZERO        PIC 9(06)  COMP VALUE 0.                                
       01  ONE          PIC 9(06)  COMP VALUE 1.                                
       01  TWO          PIC 9(06)  COMP VALUE 2.                                
       01  THREE        PIC 9(06)  COMP VALUE 3.                                
       01  SIX          PIC 9(06)  COMP VALUE 06.                               
       01  DB2IO-COMMAND    PIC X(35).                                          
       01  KEEPVIEWING  PIC XXX.                                                
       01  D8EMNUMB     PIC X(6).                                               
       01  IH-PIXELS    PIC S9(8)  COMP VALUE 800.                              
       01  IV-PIXELS    PIC S9(8)  COMP VALUE 750.                              
       01  IIM-TYPE     PIC S9(8)  COMP VALUE 1.                                
       01  IRES-TYPE    PIC S9(8)  COMP VALUE 1.                                
       01  IRES-UNIT    PIC S9(8)  COMP VALUE 0.                                
       01  IH-RES       COMP-1 VALUE 10.000E1.                                  
       01  IV-RES       COMP-1 VALUE 10.000E1.                                  
       01  PSEGFORMAT   PIC S9(8)  COMP VALUE -3.                               
       01  PSEGCOMPRESSION  PIC S9(8) COMP VALUE 4.                             
       01  ATTYPE      PIC S9(8) COMP VALUE +0.                                 
       01  ATTVAL      PIC S9(8) COMP VALUE +0.                                 
       01  VCOUNT      PIC S9(8) COMP VALUE +0.                                 
       01  APPL-ID     PIC S9(9) COMP.                                          
                                                                                
       01  AABSTR.                                                              
          02  SEVERITY-CODE   PIC S9 COMP VALUE +0.                             
          02  ERROR-CODE      PIC S9 COMP VALUE +0.                             
          02  GDDM-ANCHOR     PIC S9(9) COMP VALUE 0.                           
      *****************************************************************         
      * Job status indicator                                                    
      *****************************************************************         
       01 STATUS1              PIC X(4).                                        
          88 NOT-OK  VALUE 'BAD '.                                              
          88 OK      VALUE 'GOOD'.                                              
                                                                                
      *****************************************************************         
      * Buffer for receiving SQL error messages                                 
      *****************************************************************         
       01 ERROR-MESSAGE.                                                        
          02  ERROR-LEN            PIC S9(4)   COMP VALUE +960.                 
          02  ERROR-TEXT           PIC X(120)  OCCURS 10 TIMES                  
                                                INDEXED BY ERROR-INDEX.         
       77 ERROR-TEXT-LEN           PIC S9(9)   COMP VALUE +120.                 
                                                                                
                                                                                
      **************************** DB2 TABLES **************************        
            EXEC SQL DECLARE  EMP_PHOTO_RESUME  TABLE                           
               (        EMPNO  CHAR(06)     NOT NULL,                           
                        EMP_ROWID  ROWID,                                       
                        PSEG_PHOTO  BLOB( 500K ),                               
                        BMP_PHOTO  BLOB( 100K ),                                
                        RESUME  CLOB(   5K )         )                          
            END-EXEC.                                                           
                                                                                
                                                                                
      ********** DB2 HOST AND NULL INDICATOR VARIABLES ***************          
           EXEC SQL BEGIN DECLARE SECTION END-EXEC.                             
       77  SQLCODE                PIC S9(9) COMP-4.                             
       77  SQLSTATE               PIC X(5).                                     
       01  HVEMPNO     PIC X(6).                                                
       01  HVPSEG-PHOTO  USAGE IS SQL TYPE IS BLOB(500K).                       
       01  NIPSEG-PHOTO  PIC S9(4) COMP-4 VALUE  0.                             
           EXEC SQL END DECLARE SECTION END-EXEC.                               
                                                                                
       PROCEDURE DIVISION.                                                      
       A10000-LOBEXAMPLE.                                                       
                                                                                
            MOVE 'GOOD' TO STATUS1.                                             
            MOVE "YES" TO KEEPVIEWING.                                          
                                                                                
            PERFORM UNTIL KEEPVIEWING = "NO"                                    
                                                                                
      ***********************************************************               
      * extract the employee's PSEG photo image from BLOB storage*              
      ***********************************************************               
               PERFORM C10000-GETEMPLNUM                                        
                                                                                
      ***********************************************************               
      * if okay, convert PSEG image to GDDM format and display it*              
      ***********************************************************               
               IF OK AND KEEPVIEWING = "YES"                                    
                  PERFORM C10010-GETEMPLPHOTO                                   
                                                                                
                  IF OK                                                         
                     PERFORM C10010-SHOWEMPLPHOTO                               
                  ELSE                                                          
                     MOVE "NO" TO KEEPVIEWING                                   
                  END-IF                                                        
                                                                                
      * MSF hack to prevent infinite loop                                       
                  MOVE 'NO' TO KEEPVIEWING                                      
               END-IF                                                           
                                                                                
            END-PERFORM                                                         
                                                                                
            STOP RUN.                                                           
                                                                                
                                                                                
       C10000-GETEMPLNUM.                                                       
      **********************************************************                
      * Called by the main routine.  Displays an ISPF panels to*                
      * prompt the user to select an employee whose resume is  *                
      * to be displayed.                                       *                
      **********************************************************                
                                                                                
      *********************************************************                 
      * Share the ISPF var having the employee number         *                 
      *********************************************************                 
                                                                                
      *      CALL 'ISPLINK' USING VDEFINE EMPLNUM D8EMNUMB CHAR                 
      *.                                                                        
            MOVE SPACES TO D8EMNUMB.                                            
                                                                                
      ***** DISPLAY THE PROMPT PANEL **************************                 
      *      CALL 'ISPLINK' USING VDISPLAY DSN8SSE.                             
            IF RETURN-CODE NOT = 0                                              
               MOVE 'NO' TO KEEPVIEWING.                                        
                                                                                
      *** SAVE OFF THE VALUE OF THE ISPF SHARED VARIABLE ******                 
            MOVE '000130' TO D8EMNUMB.                                          
            MOVE D8EMNUMB TO HVEMPNO.                                           
                                                                                
      *** AND RELEASE IT **************************************                 
      *      CALL 'ISPLINK' USING VRESET.                                       
                                                                                
                                                                                
       C10010-GETEMPLPHOTO.                                                     
      **********************************************************                
      * CALLED BY THE MAIN ROUTINE. EXTRACTS A SPECIFIED       *                
      * EMPLOYEE'S PHOTO DATA FROM A BLOB COLUMN IN THE SAMPLE *                
      * EMP_PHOTO_RESUME. THIS IMAGE WILL BE CONVERTED TO      *                
      * GDDM FORMAT AND DISPLAYED BY THE ROUTINE SHOWEMPLPHOTO *                
      **********************************************************                
                                                                                
            EXEC SQL SELECT  PSEG_PHOTO                                         
                     INTO :HVPSEG-PHOTO                                         
                     FROM EMP_PHOTO_RESUME                                      
                     WHERE EMPNO = :HVEMPNO                                     
            END-EXEC.                                                           
                                                                                
            IF SQLCODE = 100                                                    
               MOVE 'BAD' TO STATUS1                                            
               DISPLAY '**************************************'                 
               DISPLAY '*** ERROR: DSN8CLPV DB2 SAMPLE PROGRAM'                 
               DISPLAY '***    NO ENTRY IN THE EMPLOYEE PHOTO/RESUME'           
               DISPLAY '***     TABLE FOR EMPLOYEE WITH EMPNO ' HVEMPNO         
               DISPLAY '***        PROCESSING TERMINATED'                       
               DISPLAY '***************************************'                
            ELSE                                                                
               IF SQLCODE = 305                                                 
                  MOVE 'BAD' TO STATUS1                                         
                  DISPLAY '************************************'                
                  DISPLAY '*** ERROR: DSN8CLPV DB2 SAMPLE PROGRAM'              
                  DISPLAY '***       NO PHOTO IMAGE EXISTS IN THE'              
                  DISPLAY '***       EMPLOYEE PHOTO/RESUME TABLE FOR '          
                  DISPLAY '***       EMPLOYEE WITH EMPNO = ' HVEMPNO            
                  DISPLAY '***       PROCESSING TERMINATED'                     
                  DISPLAY '************************************'                
               ELSE                                                             
                  IF SQLCODE NOT = 0                                            
                     MOVE 'BAD' TO STATUS1                                      
                     MOVE 'GETEMPLPHOTO @ SELECT' TO DB2IO-COMMAND              
                     PERFORM D31100-CHECK-SQLCODE                               
                  END-IF                                                        
               END-IF                                                           
            END-IF.                                                             
                                                                                
       C10010-SHOWEMPLPHOTO.                                                    
      ****************************************************************          
      * Called by the main routine.                                             
      *                                                                         
      * This is where you would put code to do an interesting                   
      * display of the photo                                                    
                                                                                
           DISPLAY 'Display photo for employee: ' HVEMPNO.                      
                                                                                
       D31100-CHECK-SQLCODE.                                                    
      ****************************************************************          
      * Verify that the prior SQL call completed successfully                   
      ****************************************************************          
            IF SQLCODE NOT = 0 THEN                                             
               MOVE 'BAD' TO STATUS1.                                           
               DISPLAY '*    UNEXPECTED SQLCODE FROM DSN8CLRV '                 
                            'DURING ' DB2IO-COMMAND ' REQUEST.'                 
               DISPLAY '*'                                                      
               PERFORM E31110-DETAIL-SQL-ERROR.                                 
                                                                                
                                                                                
       E31110-DETAIL-SQL-ERROR.                                                 
      ****************************************************************          
      * CALL DSNTIAR TO RETURN A TEXT MESSAGE FOR AN UNEXPECTED                 
      * SQLCODE.                                                                
      ****************************************************************          
            CALL 'DSNTIAR' USING SQLCA ERROR-MESSAGE ERROR-TEXT-LEN.            
            IF RETURN-CODE = ZERO                                               
               PERFORM F31111-PRINT-SQL-ERROR-MSG VARYING ERROR-INDEX           
                  FROM 1 BY 1 UNTIL ERROR-INDEX GREATER THAN 10.                
                                                                                
      *                                           **MESSAGE FORMAT              
      *                                           **ROUTINE ERROR               
      *                                           **PRINT ERROR MESSAG          
                                                                                
                                                                                
       F31111-PRINT-SQL-ERROR-MSG.                                              
      ****************************************************************          
      * PRINT MESSAGE TEXT                                                      
      ****************************************************************          
            DISPLAY ERROR-TEXT (ERROR-INDEX).                                   