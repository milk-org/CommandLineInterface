/**
 * @file processtools_trigger.c
 * 
 * 
 */


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <sched.h>


#include "CLIcore.h"

#include "processinfo.h"
#include "processtools_trigger.h"






/** @brief Set up input wait stream
 *
 * Specify stream on which the loop process will be triggering, and
 * what is the trigger mode.
 *
 * The actual trigger mode may be different from the requested trigger mode.
 *
 * The standard option should be tiggermode = PROCESSINFO_TRIGGERMODE_SEMAPHORE
 * and semindex = -1, which will automatically find a suitable semaphore
 *
 */
errno_t processinfo_waitoninputstream_init(
    PROCESSINFO *processinfo,
    imageID      trigID,
    int          triggermode,
    int          semindexrequested
)
{


    processinfo->triggerstreamID    = trigID;


    if(trigID > -1)
    {
        processinfo->triggerstreaminode = data.image[trigID].md[0].inode;
        strncpy(processinfo->triggerstreamname, data.image[trigID].md[0].name,
                STRINGMAXLEN_IMAGE_NAME);
    }
    else
    {
        processinfo->triggerstreaminode       = 0;
		strcpy(processinfo->triggerstreamname, " ");
    }


	processinfo->triggermissedframe_cumul = 0;
	processinfo->trigggertimeoutcnt       = 0;
	processinfo->triggerstatus            = 0;

	// default
	processinfo->triggermode = PROCESSINFO_TRIGGERMODE_SEMAPHORE;
	
	
	// valid modes
	
	if(triggermode == PROCESSINFO_TRIGGERMODE_CNT0)
	{
		// trigger on cnt0 increment
		processinfo->triggermode = PROCESSINFO_TRIGGERMODE_CNT0;
		processinfo->triggerstreamcnt = data.image[processinfo->triggerstreamID].md[0].cnt0;
	}
	
	
	if(triggermode == PROCESSINFO_TRIGGERMODE_CNT1)
	{
		// trigger on cnt1 increment
		processinfo->triggermode = PROCESSINFO_TRIGGERMODE_CNT1;
		processinfo->triggerstreamcnt = data.image[processinfo->triggerstreamID].md[0].cnt1;
	}
	
	
	if(triggermode == PROCESSINFO_TRIGGERMODE_IMMEDIATE)
	{
		// immmediate trigger
		processinfo->triggermode = PROCESSINFO_TRIGGERMODE_IMMEDIATE;
		processinfo->triggerstreamcnt = 0; //data.image[processinfo->triggerstreamID].md[0].cnt0;
	}
	
	
	if(triggermode == PROCESSINFO_TRIGGERMODE_DELAY)
	{
		// time wait
		
		processinfo->triggermode = PROCESSINFO_TRIGGERMODE_DELAY;
		processinfo->triggerstreamcnt = 0; //data.image[processinfo->triggerstreamID].md[0].cnt0;
	}	
	
	
	
	// checking if semaphore trigger mode OK
	if(processinfo->triggermode == PROCESSINFO_TRIGGERMODE_SEMAPHORE)
	{
		processinfo->triggersem = ImageStreamIO_getsemwaitindex(&data.image[trigID], semindexrequested);		
		if(processinfo->triggersem == -1) 
		{
			// could not find available semaphore
			// fall back to CNT0 trigger mode
			processinfo->triggermode = PROCESSINFO_TRIGGERMODE_CNT0;			
		}
		else
		{
			// register PID to stream
			data.image[trigID].semReadPID[processinfo->triggersem] = getpid();
		}
	}



	// set default timeout to 2 sec
	processinfo->triggertimeout.tv_sec = 2;
	processinfo->triggertimeout.tv_nsec = 0;

	return RETURN_SUCCESS;
}
	
	


/** @brief Wait on a stream
 *
 */

errno_t processinfo_waitoninputstream(
    PROCESSINFO *processinfo
)
{
	processinfo->triggermissedframe = 0;
	
    if ( processinfo->triggermode == PROCESSINFO_TRIGGERMODE_IMMEDIATE )
    {
		processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_RECEIVED;
        // return immediately
        return RETURN_SUCCESS;
    }


    if ( processinfo->triggermode == PROCESSINFO_TRIGGERMODE_CNT0 )
    {
        // use cnt0
        
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_WAITING;
        
        while(data.image[processinfo->triggerstreamID].md[0].cnt0 == processinfo->triggerstreamcnt)
        {
            // test if new frame exists
            usleep(5);
        }
        processinfo->triggermissedframe = data.image[processinfo->triggerstreamID].md[0].cnt0 - processinfo->triggerstreamcnt - 1;
        // update trigger counter
        processinfo->triggerstreamcnt = data.image[processinfo->triggerstreamID].md[0].cnt0;
        
        processinfo->triggermissedframe_cumul += processinfo->triggermissedframe;
        
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_RECEIVED;
        
        return RETURN_SUCCESS;
    }


    if ( processinfo->triggermode == PROCESSINFO_TRIGGERMODE_CNT1 )
    {
        // use cnt1
        
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_WAITING;
        
        while(data.image[processinfo->triggerstreamID].md[0].cnt1 == processinfo->triggerstreamcnt)
        {
            // test if new frame exists
            usleep(5);
        }
        processinfo->triggermissedframe = data.image[processinfo->triggerstreamID].md[0].cnt1 - processinfo->triggerstreamcnt - 1;
        // update trigger counter
        processinfo->triggerstreamcnt = data.image[processinfo->triggerstreamID].md[0].cnt1;
        
        processinfo->triggermissedframe_cumul += processinfo->triggermissedframe;
        
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_RECEIVED;
        
        return RETURN_SUCCESS;
    }




    if ( processinfo->triggermode == PROCESSINFO_TRIGGERMODE_DELAY )
    {
        // return after fixed delay
        
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_WAITING;
        
        nanosleep(&processinfo->triggerdelay, NULL);        
        processinfo->triggerstreamcnt++; 
                
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_RECEIVED;
        
        return RETURN_SUCCESS;
    }




    if ( processinfo->triggermode == PROCESSINFO_TRIGGERMODE_SEMAPHORE )
    {
        int semr;
        int tmpstatus = PROCESSINFO_TRIGGERSTATUS_RECEIVED;
        
        processinfo->triggerstatus = PROCESSINFO_TRIGGERSTATUS_WAITING;

        // get current time
        struct timespec ts;
        if(clock_gettime(CLOCK_REALTIME, &ts) == -1)
        {
            perror("clock_gettime");
            exit(EXIT_FAILURE);
        }

        // is semaphore at zero ?
        semr = 0;
        while ( semr == 0 )
        {
            // this should only run once, returning semr = -1 with errno = EAGAIN
            // otherwise, we're potentially missing frames
            semr = sem_trywait(data.image[processinfo->triggerstreamID].semptr[processinfo->triggersem]);
            if ( semr == 0 )
            {
                processinfo->triggermissedframe ++;
            }
        }

        // expected state: NBmissedframe = 0, semr = -1, errno = EAGAIN
        // missed frame state: NBmissedframe>0, semr = -1, errno = EAGAIN

        if (processinfo->triggermissedframe == 0)
        {
            // add timeout
            ts.tv_sec += processinfo->triggertimeout.tv_sec;
            ts.tv_nsec += processinfo->triggertimeout.tv_nsec;
            while(ts.tv_nsec > 1000000000)
            {
                ts.tv_nsec -= 1000000000;
                ts.tv_sec ++;
            }

            semr = sem_timedwait(data.image[processinfo->triggerstreamID].semptr[processinfo->triggersem], &ts);
            if(semr == -1)
            {
				if (errno == ETIMEDOUT)
				{
					// timeout condition
					processinfo->trigggertimeoutcnt ++;
					tmpstatus = PROCESSINFO_TRIGGERSTATUS_TIMEDOUT;
				}
			}
            
            // measure time spent waiting for input
			// get current time
			struct timespec ts1;
			if(clock_gettime(CLOCK_REALTIME, &ts1) == -1)
			{
				perror("clock_gettime");
				exit(EXIT_FAILURE);
			}
		}            
            
        processinfo->triggermissedframe_cumul += processinfo->triggermissedframe;
        
        processinfo->triggerstatus = tmpstatus;
        
        return RETURN_SUCCESS;        
    }


	return RETURN_FAILURE;
}


