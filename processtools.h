/**
 * @file    processtools.h
 * @brief   Command line interface
 *
 * Command line interface (CLI) definitions and function prototypes
 *
 * @bug No known bugs.
 *
 */

/* =============================================================================================== */
/* =============================================================================================== */
/*                                      DEFINES, MACROS                                            */
/* =============================================================================================== */
/* =============================================================================================== */

#ifndef _PROCESSTOOLS_H
#define _PROCESSTOOLS_H

#include <semaphore.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>

#include "processinfo.h"
#include "processtools_trigger.h"



#define PROCESSINFOLISTSIZE 10000


#define MAXNBSUBPROCESS 50
#define MAXNBCPU 100





#ifndef __STDC_LIB_EXT1__
typedef int errno_t;
#endif






//
// This structure maintains a list of active processes
// It is used to quickly build (without scanning directory) an array of
// PROCESSINFO
//
typedef struct {
    pid_t PIDarray[PROCESSINFOLISTSIZE];
    int   active[PROCESSINFOLISTSIZE];
    char  pnamearray[PROCESSINFOLISTSIZE][STRINGMAXLEN_PROCESSINFO_NAME];  // short name

} PROCESSINFOLIST;



typedef struct
{
	int           active;
	pid_t         PID;
	char          name[40];
	long          updatecnt;

	long          loopcnt;
	int           loopstat;
	
	int           createtime_hr;
	int           createtime_min;
	int           createtime_sec;
	long          createtime_ns;
	
	char          cpuset[16];       /**< cpuset name  */
	char          cpusallowed[20];
	int           cpuOKarray[MAXNBCPU];
	int           threads;
	
	int           rt_priority;
	float         memload;
		
	char          statusmsg[200];
	char          tmuxname[100];
	
	int           NBsubprocesses;
	int           subprocPIDarray[MAXNBSUBPROCESS];	
	
	double        sampletimearray[MAXNBSUBPROCESS];  // time at which sampling was performed [sec]
	double        sampletimearray_prev[MAXNBSUBPROCESS];
	
	long          ctxtsw_voluntary[MAXNBSUBPROCESS];
	long          ctxtsw_nonvoluntary[MAXNBSUBPROCESS];
	long          ctxtsw_voluntary_prev[MAXNBSUBPROCESS];
	long          ctxtsw_nonvoluntary_prev[MAXNBSUBPROCESS];
	
	long long     cpuloadcntarray[MAXNBSUBPROCESS];
	long long     cpuloadcntarray_prev[MAXNBSUBPROCESS];
	float         subprocCPUloadarray[MAXNBSUBPROCESS];
	float         subprocCPUloadarray_timeaveraged[MAXNBSUBPROCESS];
	
	long          VmRSSarray[MAXNBSUBPROCESS];
	
	int           processorarray[MAXNBSUBPROCESS];

	
} PROCESSINFODISP;





typedef struct
{
	int      loop;   // 1 : loop     0 : exit
	long     loopcnt;
	
	int      twaitus; // sleep time between scans
	double   dtscan; // measured time interval between scans [s]
	pid_t    scanPID;
	int      scandebugline; // for debugging	
	
	
	// ensure list of process and mmap operation blocks display
	int      SCANBLOCK_requested;  // scan thread toggles to 1 to requests blocking
	int      SCANBLOCK_OK;         // display thread toggles to 1 to let scan know it can proceed
		
	PROCESSINFOLIST *pinfolist;  // copy of pointer  static PROCESSINFOLIST *pinfolist

	long NBpinfodisp;
	PROCESSINFODISP *pinfodisp;
	
	int DisplayMode;
	int DisplayDetailedMode;
	
    //
    // these arrays are indexed together
    // the index is different from the displayed order
    // new process takes first available free index
    //
    PROCESSINFO  *pinfoarray[PROCESSINFOLISTSIZE];
    int           pinfommapped[PROCESSINFOLISTSIZE];             // 1 if mmapped, 0 otherwise
    pid_t         PIDarray[PROCESSINFOLISTSIZE];                 // used to track changes
    int           updatearray[PROCESSINFOLISTSIZE];              // 0: don't load, 1: (re)load
    int           fdarray[PROCESSINFOLISTSIZE];                  // file descriptors
    long          loopcntarray[PROCESSINFOLISTSIZE];
    long          loopcntoffsetarray[PROCESSINFOLISTSIZE];
    int           selectedarray[PROCESSINFOLISTSIZE];

    int           sorted_pindex_time[PROCESSINFOLISTSIZE];
		
		
	int NBcpus;
	int NBcpusocket;
	
	float CPUload[MAXNBCPU];
	long long CPUcnt0[MAXNBCPU];
	long long CPUcnt1[MAXNBCPU];
	long long CPUcnt2[MAXNBCPU];
	long long CPUcnt3[MAXNBCPU];
	long long CPUcnt4[MAXNBCPU];
	long long CPUcnt5[MAXNBCPU];
	long long CPUcnt6[MAXNBCPU];
	long long CPUcnt7[MAXNBCPU];
	long long CPUcnt8[MAXNBCPU];

	int CPUids[MAXNBCPU];  // individual cpus (same cores)
	int CPUphys[MAXNBCPU]; // Physical CPU socket

	int CPUpcnt[MAXNBCPU];	
	
	int NBpindexActive;
	int pindexActive[PROCESSINFOLISTSIZE];
	int psysinfostatus[PROCESSINFOLISTSIZE];
	
} PROCINFOPROC;





// ---------------------  -------------------------------

typedef struct {
    char name[200];
    char description[200];
} STRINGLISTENTRY;

#ifdef __cplusplus
extern "C" {
#endif



PROCESSINFO * processinfo_setup(
    char *pinfoname,	
    char descriptionstring[200],
    char msgstring[200],
    const char *functionname,
    const char *filename,
    int   linenumber
);

errno_t processinfo_error(
    PROCESSINFO *processinfo,
    char *errmsgstring
);

errno_t processinfo_loopstart(
    PROCESSINFO *processinfo
);

int processinfo_loopstep(
    PROCESSINFO *processinfo
);

int processinfo_compute_status(
    PROCESSINFO *processinfo
);




PROCESSINFO *processinfo_shm_create(const char *pname, int CTRLval);
PROCESSINFO *processinfo_shm_link(const char *pname, int *fd);
int processinfo_shm_close(PROCESSINFO *pinfo, int fd);
int processinfo_cleanExit(PROCESSINFO *processinfo);
int processinfo_SIGexit(PROCESSINFO *processinfo, int SignalNumber);
int processinfo_WriteMessage(PROCESSINFO *processinfo, const char *msgstring);
int processinfo_exec_start(PROCESSINFO *processinfo);
int processinfo_exec_end(PROCESSINFO *processinfo);


int processinfo_CatchSignals();
int processinfo_ProcessSignals(PROCESSINFO *processinfo);



errno_t processinfo_update_output_stream(
    PROCESSINFO *processinfo,
    imageID outstreamID
);



errno_t processinfo_CTRLscreen();

#ifdef __cplusplus
}
#endif

#endif  // _PROCESSTOOLS_H
