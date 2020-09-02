/**
 * @file timeutils.h
 */

errno_t mkUTtimestring_nanosec(char *timestring, struct timespec tnow);
errno_t mkUTtimestring_nanosec_now(char *timestring);

errno_t mkUTtimestring_microsec(char *timestring, struct timespec tnow);
errno_t mkUTtimestring_microsec_now(char *timestring);

errno_t mkUTtimestring_millisec(char *timestring, struct timespec tnow);
errno_t mkUTtimestring_millisec_now(char *timestring);

errno_t mkUTtimestring_sec(char *timestring, struct timespec tnow);
errno_t mkUTtimestring_sec_now(char *timestring);


struct timespec timespec_diff(
    struct timespec start,
    struct timespec end
);

double timespec_diff_double(
    struct timespec start,
    struct timespec end
);
