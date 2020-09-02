/**
 * @file timeutils.c
 */

#include <time.h>
#include "CommandLineInterface/CLIcore.h"





errno_t mkUTtimestring_nanosec(char *timestring, struct timespec tnow)
{
	struct tm *uttime;
    time_t tvsec0;
    
    tvsec0 = tnow.tv_sec;
    uttime = gmtime(&tvsec0);

    sprintf(timestring, "%04d%02d%02dT%02d%02d%02d.%09ldZ",
            1900 + uttime->tm_year, 1 + uttime->tm_mon, uttime->tm_mday, uttime->tm_hour,
            uttime->tm_min,  uttime->tm_sec, tnow.tv_nsec);


    return RETURN_SUCCESS;
}

errno_t mkUTtimestring_nanosec_now(char *timestring)
{
    struct timespec tnow;    
    
    clock_gettime(CLOCK_REALTIME, &tnow);
    mkUTtimestring_nanosec(timestring, tnow);

    return RETURN_SUCCESS;
}








errno_t mkUTtimestring_microsec(char *timestring, struct timespec tnow)
{
	struct tm *uttime;
    time_t tvsec0;
    
    
    tvsec0 = tnow.tv_sec;
    uttime = gmtime(&tvsec0);

    sprintf(timestring, "%04d%02d%02dT%02d%02d%02d.%06ldZ",
            1900 + uttime->tm_year, 1 + uttime->tm_mon, uttime->tm_mday, uttime->tm_hour,
            uttime->tm_min,  uttime->tm_sec, (long) (tnow.tv_nsec/1000));


    return RETURN_SUCCESS;
}


errno_t mkUTtimestring_microsec_now(char *timestring)
{
    struct timespec tnow;    
    
    clock_gettime(CLOCK_REALTIME, &tnow);
    mkUTtimestring_microsec(timestring, tnow);

    return RETURN_SUCCESS;
}






errno_t mkUTtimestring_millisec(char *timestring, struct timespec tnow)
{
	struct tm *uttime;
    time_t tvsec0;
        
    tvsec0 = tnow.tv_sec;
    uttime = gmtime(&tvsec0);

    sprintf(timestring, "%04d%02d%02dT%02d%02d%02d.%03ldZ",
            1900 + uttime->tm_year, 1 + uttime->tm_mon, uttime->tm_mday, uttime->tm_hour,
            uttime->tm_min,  uttime->tm_sec, (long) (tnow.tv_nsec/1000000));


    return RETURN_SUCCESS;
}

errno_t mkUTtimestring_millisec_now(char *timestring)
{
    struct timespec tnow;    
    
    clock_gettime(CLOCK_REALTIME, &tnow);
    mkUTtimestring_millisec(timestring, tnow);

    return RETURN_SUCCESS;
}







errno_t mkUTtimestring_sec(char *timestring, struct timespec tnow)
{
	struct tm *uttime;
    time_t tvsec0;
    
    
    tvsec0 = tnow.tv_sec;
    uttime = gmtime(&tvsec0);

    sprintf(timestring, "%04d%02d%02dT%02d%02d%02dZ",
            1900 + uttime->tm_year, 1 + uttime->tm_mon, uttime->tm_mday, uttime->tm_hour,
            uttime->tm_min,  uttime->tm_sec);


    return RETURN_SUCCESS;
}

errno_t mkUTtimestring_sec_now(char *timestring)
{
    struct timespec tnow;    
    
    clock_gettime(CLOCK_REALTIME, &tnow);
    mkUTtimestring_sec(timestring, tnow);

    return RETURN_SUCCESS;
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
