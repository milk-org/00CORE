/**
 * @file    00CORE.c
 * @brief   core functions
 *
 * Useful functions frequently used by modules
 *
 *
 */



/* ================================================================== */
/* ================================================================== */
/*            MODULE INFO                                             */
/* ================================================================== */
/* ================================================================== */

#define MODULE_NAME              "00CORE"

// module default short name
// all CLI calls to this module functions will be <shortname>.<funcname>
// if set to "", then calls use <funcname>
#define MODULE_SHORTNAME_DEFAULT ""

// Module short description
#define MODULE_DESCRIPTION       "Core functions"

// Application to which module belongs
#define MODULE_APPLICATION       "milk"








/* =============================================================================================== */
/* =============================================================================================== */
/*                                        HEADER FILES                                             */
/* =============================================================================================== */
/* =============================================================================================== */

#include <fitsio.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h> // needed for tid = syscall(SYS_gettid);

#ifdef __MACH__
#include <mach/mach_time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
int clock_gettime(int clk_id, struct mach_timespec *t)
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t time;
    time = mach_absolute_time();
    double nseconds = ((double)time * (double)timebase.numer) / ((
                          double)timebase.denom);
    double seconds = ((double)time * (double)timebase.numer) / ((
                         double)timebase.denom * 1e9);
    t->tv_sec = seconds;
    t->tv_nsec = nseconds;
    return 0;
}
#else
#include <time.h>
#endif



#include "CommandLineInterface/CLIcore.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "00CORE/00CORE.h"





/* =============================================================================================== */
/* =============================================================================================== */
/*                                  GLOBAL DATA DECLARATION                                        */
/* =============================================================================================== */
/* =============================================================================================== */


//extern DATA data;

static int INITSTATUS_00CORE = 0;






/* =============================================================================================== */
/* =============================================================================================== */
/*                                    MODULE INITIALIZATION                                        */
/* =============================================================================================== */
/* =============================================================================================== */
/** @name Module initialization */

// for internal use
static errno_t init_module_CLI()
{
    return RETURN_SUCCESS;
}

// for external use
// may perform additional initialization
errno_t init_00CORE()
{
    return init_module_CLI();
}


// Module initialization macro in CLIcore.h
//
INIT_MODULE_LIB();






/* =============================================================================================== */
/* =============================================================================================== */
/*                                    FUNCTIONS SOURCE CODE                                        */
/* =============================================================================================== */
/* =============================================================================================== */



/**
 * ## Purpose
 *
 * Print red string to stdout
 *
 * ## Arguments
 *
 * @param[in]
 * string	CHAR*
 */
int printRED(char *string)
{
    printf("%c[%d;%dm %s %c[%d;m\n", (char) 27, 1, 31, string, (char) 27, 0);

    return(0);
}





/**
 * ## Purpose
 *
 * Print warning string
 *
 * ## Arguments
 *
 * @param[in]
 * file		CHAR*
 * 			file name from which warning is issued
 *
 * @param[in]
 * func		CHAR*
 * 			function name from which warning is issued
 *
 * @param[in]
 * line		int
 * 			line number from which warning is issued
 *
 * @param[in]
 * warnmessage		CHAR*
 * 			warning message to be printed
 *
 *
 *  Color codes for printf
    	30	Black
    	31	Red
    	32	Green
    	33	Yellow
    	34	Blue
    	35	Magenta
    	36	Cyan
    	37	White

 */

errno_t printWARNING(
    const char *restrict file,
    const char *restrict func,
    int                  line,
    const char *restrict warnmessage
)
{
    fprintf(stderr,
            "%c[%d;%dm WARNING [ FILE: %s   FUNCTION: %s  LINE: %d ]  %c[%d;m\n",
            (char) 27, 1, 35, file, func, line, (char) 27, 0);
    if(C_ERRNO != 0)
    {
        char buff[256];
        if(strerror_r(errno, buff, 256) == 0)
        {
            fprintf(stderr, "C Error: %s\n", buff);
        }
        else
        {
            fprintf(stderr, "Unknown C Error\n");
        }
    }
    else
    {
        fprintf(stderr, "No C error (errno = 0)\n");
    }

    fprintf(stderr,
            "%c[%d;%dm %s  %c[%d;m\n",
            (char) 27, 1, 35, warnmessage, (char) 27, 0);
    C_ERRNO = 0;

    return RETURN_SUCCESS;
}





/**
 * ## Purpose
 *
 * Print error string
 *
 * ## Arguments
 *
 * @param[in]
 * file		CHAR*
 * 			file name from which error is issued
 *
 * @param[in]
 * func		CHAR*
 * 			function name from which error is issued
 *
 * @param[in]
 * line		int
 * 			line number from which error is issued
 *
 * @param[in]
 * warnmessage		CHAR*
 * 			error message to be printed
 *
 */

errno_t printERROR(
    const char *restrict file,
    const char *restrict func,
    int                  line,
    const char *restrict errmessage
)
{
    fprintf(stderr, "%c[%d;%dm ERROR [ %s:%d: %s ]  %c[%d;m\n", (char) 27, 1, 31,
            file, line, func, (char) 27, 0);
    if(C_ERRNO != 0)
    {
        char buff[256];
        if(strerror_r(errno, buff, 256) == 0)
        {
            fprintf(stderr, "C Error: %s\n", buff);
        }
        else
        {
            fprintf(stderr, "Unknown C Error\n");
        }
    }
    else
    {
        fprintf(stderr, "No C error (errno = 0)\n");
    }

    fprintf(stderr, "%c[%d;%dm %s  %c[%d;m\n", (char) 27, 1, 31, errmessage,
            (char) 27, 0);

    C_ERRNO = 0;

    return RETURN_SUCCESS;
}









/**
 * ## Purpose
 *
 * Log function call (for testing / debugging only).
 *
 * Function calls are logged if to file .FileName.funccalls.log
 *
 * Variable AOLOOPCONTROL_logfunc_level keeps track of function depth: \n
 * it is incremented when entering a function \n
 * decremented when exiting a function
 *
 * Variable AOLOOPCONTROL_logfunc_level_max sets the max depth of logging
 *
 *
 * At the beginning of each function, insert this code:
 * @code
 * #ifdef TEST
 * CORE_logFunctionCall( logfunc_level, logfunc_level_max, 1, __FILE__, __func__, __LINE__, "");
 * #endif
 * @endcode
 * and at the end of each function:
 * @code
 * #ifdef TEST
 * CORE_logFunctionCall( logfunc_level, logfunc_level_max, 1, __FILE__, __func__, __LINE__, "");
 * #endif
 * @endcode
 *
 *
 * ## Arguments
 *
 * @param[in]
 * funclevel		INT
 * 					Function level (0: top level, always log)
 *
 * @param[in]
 * loglevel			INT
 * 					Log level: log all function with level =< loglevel
 *
 * logfuncMODE		INT
 * 					Log mode, 0:entering function, 1:exiting function
 *
 * @param[in]
 * FileName			char*
 * 					Name of source file, usually __FILE__ so that preprocessor fills this parameter.
 *
 * @param[in]
 * FunctionName		char*
 * 					Name of function, usually __FUNCTION__ so that preprocessor fills this parameter.
 *
 * @param[in]
 * line				char*
 * 					Line in cource code, usually __LINE__ so that preprocessor fills this parameter.
 *
 * @param[in]
 * comments			char*
 * 					comment string
 *
 * @return void
 *
 * @note Carefully set depth value to avoid large output file.
 * @warning May slow down code. Only use for debugging. Output file may grow very quickly.
 */

void CORE_logFunctionCall(
    const int funclevel,
    const int loglevel,
    const int logfuncMODE,
    __attribute__((unused)) const char *FileName,
    const char *FunctionName,
    const long line,
    char *comments
)
{
    time_t tnow;
    struct timespec timenow;
    pid_t tid;
    char modechar;

    modechar = '?';

    if(logfuncMODE == 0)
    {
        modechar = '>';
    }
    else if(logfuncMODE == 1)
    {
        modechar = '<';
    }
    else
    {
        modechar = '?';
    }

    if(funclevel <= loglevel)
    {
        char  fname[500];

        FILE *fp;


        sprintf(fname, ".%s.funccalls.log", FunctionName);

        struct tm *uttime;
        tnow = time(NULL);
        uttime = gmtime(&tnow);
        clock_gettime(CLOCK_REALTIME, &timenow);
        tid = syscall(SYS_gettid);

        // add custom parameter into string (optional)

        fp = fopen(fname, "a");
        fprintf(fp, "%02d:%02d:%02ld.%09ld  %10d  %10d  %c %40s %6ld   %s\n",
                uttime->tm_hour, uttime->tm_min, timenow.tv_sec % 60, timenow.tv_nsec,
                getpid(), (int) tid,
                modechar, FunctionName, line, comments);
        fclose(fp);
    }


}










/**
 * ## Purpose
 *
 * Set default precision (single or double)
 *
 * ## Arguments
 *
 * @param[in]
 * vp		int
 * 			precision mode
 *
 *
 * @param[in]
 * warnmessage		CHAR*
 * 			error message to be printed
 * -		0: single precision
 * -		1: double precision
 *
 */
int set_precision(int vp)
{
    if(vp == 0)
    {
        data.precision = 0;
        printf("Default precision : single (float)\n");
    }
    else if(vp == 1)
    {
        data.precision = 1;
        printf("Default precision : double\n");
    }
    else
    {
        printf("Requested precision value (%d) not a valid choice\n", vp);
    }

    return(0);
}





int CLIinfoPrint()
{
    printf("Process ID   : %d\n", CLIPID);
    printf("CONFIGDIR = %s\n", data.configdir);
    printf("SOURCEDIR = %s\n", data.sourcedir);
    printf("Memory usage      : %ld Mb  (%ld images)\n",
           ((long)(compute_image_memory(data) / 1024 / 1024)),
           compute_nb_image(data));
    printf("Default precision : %d (0: single, 1: double)\n", data.precision);

    return(0);
}





int CLIWritePid()
{
    int pid;
    char command[200];
    int n;

    pid = getpid();
    n = snprintf(command, 200, "touch CLI_%d", pid);
    if(n >= 200)
    {
        printERROR(__FILE__, __func__, __LINE__,
                   "Attempted to write string buffer with too many characters");
    }

    if(system(command) != 0)
    {
        printERROR(__FILE__, __func__, __LINE__, "system() returns non-zero value");
    }

    return(0);
}





struct timespec timespec_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;

    if((end.tv_nsec - start.tv_nsec) < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return temp;
}




double timespec_diff_double(struct timespec start, struct timespec end)
{
    struct timespec temp;
    double val;

    if((end.tv_nsec - start.tv_nsec) < 0)
    {
        temp.tv_sec = end.tv_sec - start.tv_sec - 1;
        temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        temp.tv_sec = end.tv_sec - start.tv_sec;
        temp.tv_nsec = end.tv_nsec - start.tv_nsec;
    }

    val = temp.tv_sec;
    val += 0.000000001 * temp.tv_nsec;

    return val;
}





int file_exist(char *filename)
{
    struct stat   buffer;
    return (stat(filename, &buffer) == 0);
}



