
#define _GNU_SOURCE

/* ===============================================================================================
 */
/* ===============================================================================================
 */
/*                                        HEADER FILES */
/* ===============================================================================================
 */
/* ===============================================================================================
 */

#include "standalone_dependencies.h"
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "streamCTRL.h"

static int wrow, wcol;
int C_ERRNO = 0;

/* ===============================================================================================
 */
/*                                         DUPLICATED CODE */
/* ===============================================================================================
 */
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

int print_header(const char *str, char c)
{
    long n;
    long i;

    attron(A_BOLD);
    n = strlen(str);
    for(i = 0; i < (wcol - n) / 2; i++)
    {
        printw("%c", c);
    }
    printw("%s", str);
    for(i = 0; i < (wcol - n) / 2 - 1; i++)
    {
        printw("%c", c);
    }
    printw("\n");
    attroff(A_BOLD);

    return (0);
}

void qs2l(double *array, long *array1, long left, long right)
{
    register long i, j;
    double x, y;
    long l1;

    i = left;
    j = right;
    x = array[(left + right) / 2];

    do
    {
        while(array[i] < x && i < right)
        {
            i++;
        }
        while(x < array[j] && j > left)
        {
            j--;
        }

        if(i <= j)
        {
            y = array[i];
            array[i] = array[j];
            array[j] = y;

            l1 = array1[i];
            array1[i] = array1[j];
            array1[j] = l1;

            i++;
            j--;
        }
    }
    while(i <= j);

    if(left < j)
    {
        qs2l(array, array1, left, j);
    }
    if(i < right)
    {
        qs2l(array, array1, i, right);
    }
}

void quick_sort2l(double *array, long *array1, long count)
{
    qs2l(array, array1, 0, count - 1);
}

void qs2l_double(double *array, long *array1, long left, long right)
{
    register long i, j;
    double x, y;
    long l1;

    i = left;
    j = right;
    x = array[(left + right) / 2];

    do
    {
        while(array[i] < x && i < right)
        {
            i++;
        }
        while(x < array[j] && j > left)
        {
            j--;
        }

        if(i <= j)
        {
            y = array[i];
            array[i] = array[j];
            array[j] = y;

            l1 = array1[i];
            array1[i] = array1[j];
            array1[j] = l1;

            i++;
            j--;
        }
    }
    while(i <= j);

    if(left < j)
    {
        qs2l_double(array, array1, left, j);
    }
    if(i < right)
    {
        qs2l_double(array, array1, i, right);
    }
}

void quick_sort2l_double(double *array, long *array1, long count)
{
    qs2l_double(array, array1, 0, count - 1);
}

void qs_long(long *array, long left, long right)
{
    register long i, j;
    long x, y;

    i = left;
    j = right;
    x = array[(left + right) / 2];

    do
    {
        while(array[i] < x && i < right)
        {
            i++;
        }
        while(x < array[j] && j > left)
        {
            j--;
        }

        if(i <= j)
        {
            y = array[i];
            array[i] = array[j];
            array[j] = y;
            i++;
            j--;
        }
    }
    while(i <= j);

    if(left < j)
    {
        qs_long(array, left, j);
    }
    if(i < right)
    {
        qs_long(array, i, right);
    }
}

void quick_sort_long(long *array, long count)
{
    qs_long(array, 0, count - 1);
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
int printERROR(const char *file, const char *func, int line, char *errmessage)
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

    return(0);
}

int file_exists(const char *restrict file_name)
{
    FILE *fp;
    int exists = 1;

    if((fp = fopen(file_name, "r")) == NULL)
    {
        exists = 0;
        /*      printf("file %s does not exist\n",file_name);*/
    }
    else
    {
        fclose(fp);
    }

    return(exists);
}

int is_fits_file(const char *restrict file_name)
{
    FILE *fp;
    int exists = 1;

    if((fp = fopen(file_name, "r")) == NULL)
    {
        exists = 0;
        /*      printf("file %s does not exist\n",file_name);*/
    }
    else
    {
        fclose(fp);
    }

    return(exists);
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

/**
 * @brief Duplicated code from MILK/COREMOD_memory fps_create.c
 *
 * @param NBparamMAX Max number of parameters
 * @param name name of the fps
 * @return errno_t
 */
errno_t function_parameter_struct_create(
    int NBparamMAX,
    const char *name
)
{
    int index;
    char *mapv;
    FUNCTION_PARAMETER_STRUCT fps;

    //  FUNCTION_PARAMETER_STRUCT_MD *funcparammd;
    //  FUNCTION_PARAMETER *funcparamarray;

    char SM_fname[200];
    size_t sharedsize = 0; // shared memory size in bytes
    int SM_fd; // shared memory file descriptor

    char shmdname[200];
    function_parameter_struct_shmdirname(shmdname);

    if(snprintf(SM_fname, sizeof(SM_fname), "%s/%s.fps.shm", shmdname, name) < 0)
    {
        PRINT_ERROR("snprintf error");
    }
    remove(SM_fname);

    printf("Creating file %s, holding NBparamMAX = %d\n", SM_fname, NBparamMAX);
    fflush(stdout);

    sharedsize = sizeof(FUNCTION_PARAMETER_STRUCT_MD);
    sharedsize += sizeof(FUNCTION_PARAMETER) * NBparamMAX;

    SM_fd = open(SM_fname, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    if(SM_fd == -1)
    {
        perror("Error opening file for writing");
        printf("STEP %s %d\n", __FILE__, __LINE__);
        fflush(stdout);
        exit(0);
    }

    fps.SMfd = SM_fd;

    int result;
    result = lseek(SM_fd, sharedsize - 1, SEEK_SET);
    if(result == -1)
    {
        close(SM_fd);
        printf("ERROR [%s %s %d]: Error calling lseek() to 'stretch' the file\n",
               __FILE__, __func__, __LINE__);
        printf("STEP %s %d\n", __FILE__, __LINE__);
        fflush(stdout);
        exit(0);
    }

    result = write(SM_fd, "", 1);
    if(result != 1)
    {
        close(SM_fd);
        perror("Error writing last byte of the file");
        printf("STEP %s %d\n", __FILE__, __LINE__);
        fflush(stdout);
        exit(0);
    }

    fps.md = (FUNCTION_PARAMETER_STRUCT_MD *) mmap(0, sharedsize,
             PROT_READ | PROT_WRITE, MAP_SHARED, SM_fd, 0);
    if(fps.md == MAP_FAILED)
    {
        close(SM_fd);
        perror("Error mmapping the file");
        printf("STEP %s %d\n", __FILE__, __LINE__);
        fflush(stdout);
        exit(0);
    }
    //funcparamstruct->md = funcparammd;

    mapv = (char *) fps.md;
    mapv += sizeof(FUNCTION_PARAMETER_STRUCT_MD);
    fps.parray = (FUNCTION_PARAMETER *) mapv;



    printf("shared memory space = %ld bytes\n", sharedsize); //TEST


    fps.md->NBparamMAX = NBparamMAX;

    for(index = 0; index < NBparamMAX; index++)
    {
        fps.parray[index].fpflag = 0; // not active
        fps.parray[index].cnt0 = 0;   // update counter
    }


    strncpy(fps.md->name, name, STRINGMAXLEN_FPS_NAME);
	  strncpy(fps.md->callprogname, "", FPS_CALLPROGNAME_STRMAXLEN);
	  strncpy(fps.md->callfuncname, "", FPS_CALLFUNCNAME_STRMAXLEN);

    char cwd[FPS_CWD_STRLENMAX];
    if(getcwd(cwd, sizeof(cwd)) != NULL)
    {
        strncpy(fps.md->workdir, cwd, FPS_CWD_STRLENMAX);
    }
    else
    {
        perror("getcwd() error");
        return 1;
    }

    strncpy(fps.md->sourcefname, "NULL", FPS_SRCDIR_STRLENMAX);
    fps.md->sourceline = 0;

	// set default fpsdatadir
	sprintf(fps.md->datadir, "fps.%s.datadir", fps.md->name);
	// and create the directory
	mkdir(fps.md->datadir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


	// set default fpsconfdir
	sprintf(fps.md->confdir, "fps.%s.confdir", fps.md->name);
	// and create the directory
	mkdir(fps.md->confdir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);


    fps.md->signal     = (uint64_t) FUNCTION_PARAMETER_STRUCT_SIGNAL_CONFRUN;
    fps.md->confwaitus = (uint64_t) 1000; // 1 kHz default
    fps.md->msgcnt = 0;

    munmap(fps.md, sharedsize);


    return EXIT_SUCCESS;
}

/* ===============================================================================================
 */
/*                                 END OF DUPLICATED CODE */
/* ===============================================================================================
 */
