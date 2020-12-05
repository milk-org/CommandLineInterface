/**
 * @file    fps_outlog.c
 * @brief   output log functions for FPS
 */

#include <stdarg.h>

#include "CommandLineInterface/CLIcore.h"

#define STANDALONE
#ifdef STANDALONE
#include <dirent.h>
#include <regex.h>
int find_file(char *file_name, char *pattern, char *directory) {
  DIR *opened_dir;
  struct dirent *directory_structure;
  regex_t regex;
  int reti;
  char msgbuf[100];

  /* Compile regular expression */
  reti = regcomp(&regex, pattern, 0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

  char *temp_dir = (char *)malloc(sizeof(char) * 300);
  if (!temp_dir) {
    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);

    return -1;
  }

  strcat(directory, "/");
  opened_dir = opendir(directory);
  if (opened_dir == NULL) {
    /* Free memory allocated to the pattern buffer by regcomp() */
    regfree(&regex);
    free(temp_dir);
    return -1;
  }

  while (1) {
    directory_structure = readdir(opened_dir);
    if (!directory_structure) {
      /* Free memory allocated to the pattern buffer by regcomp() */
      regfree(&regex);
      closedir(opened_dir);
      free(temp_dir);
      return -1;
    }
    if (!strcmp(directory_structure->d_name, "..") ||
        !strcmp(directory_structure->d_name, "."))
      continue;

    /* Execute regular expression */
    reti = regexec(&regex, directory_structure->d_name, 0, NULL, 0);
    if (!reti) {
      // Match
      sprintf(file_name, "%s%s", directory, directory_structure->d_name);
      return 0;
    } else if (reti == REG_NOMATCH) {
      // No match
    } else {
      regerror(reti, &regex, msgbuf, sizeof(msgbuf));
      fprintf(stderr, "Regex match failed: %s\n", msgbuf);
      exit(1);
    }
  }
  /* Free memory allocated to the pattern buffer by regcomp() */
  regfree(&regex);
  free(temp_dir);
  return -1;
}
#endif  // STANDALONE

/** @brief Get FPS log filename
 *
 * logfname should be char [STRINGMAXLEN_FULLFILENAME]
 *
 */
errno_t getFPSlogfname(char *logfname) {
  char shmdname[STRINGMAXLEN_SHMDIRNAME];
  function_parameter_struct_shmdirname(shmdname);

#ifndef STANDALONE
  WRITE_FULLFILENAME(logfname, "%s/fpslog.%ld.%07d.%s", shmdname,
                     data.FPS_TIMESTAMP, getpid(), data.FPS_PROCESS_TYPE);
#else   // STANDALONE
  char pattern[STRINGMAXLEN_SHMDIRNAME];
  WRITE_FULLFILENAME(pattern, "fpslog.*.%07d.*", getpid());
  find_file(logfname, "fpslog.%ld.%07d.%s", shmdname);
#endif  // STANDALONE

  return RETURN_SUCCESS;
}

errno_t functionparameter_outlog_file(char *keyw, char *msgstring,
                                      FILE *fpout) {
  // Get GMT time
  struct timespec tnow;
  time_t now;

  clock_gettime(CLOCK_REALTIME, &tnow);
  now = tnow.tv_sec;
  struct tm *uttime;
  uttime = gmtime(&now);

  char timestring[30];
  sprintf(timestring, "%04d%02d%02dT%02d%02d%02d.%09ld", 1900 + uttime->tm_year,
          1 + uttime->tm_mon, uttime->tm_mday, uttime->tm_hour, uttime->tm_min,
          uttime->tm_sec, tnow.tv_nsec);

  fprintf(fpout, "%s %-12s %s\n", timestring, keyw, msgstring);
  fflush(fpout);

  return RETURN_SUCCESS;
}

errno_t functionparameter_outlog(char *keyw, const char *fmt, ...) {
  // identify logfile and open file

  static int LogOutOpen = 0;
  static FILE *fpout;

  if (LogOutOpen == 0)  // file not open
  {
    char logfname[STRINGMAXLEN_FULLFILENAME];
    getFPSlogfname(logfname);

    fpout = fopen(logfname, "a");
    if (fpout == NULL) {
      printf("ERROR: cannot open file\n");
      exit(EXIT_FAILURE);
    }
    LogOutOpen = 1;
  }

  // Get GMT time and create timestring

  struct timespec tnow;
  time_t now;

  clock_gettime(CLOCK_REALTIME, &tnow);
  now = tnow.tv_sec;
  struct tm *uttime;
  uttime = gmtime(&now);

  char timestring[30];
  sprintf(timestring, "%04d%02d%02dT%02d%02d%02d.%09ld", 1900 + uttime->tm_year,
          1 + uttime->tm_mon, uttime->tm_mday, uttime->tm_hour, uttime->tm_min,
          uttime->tm_sec, tnow.tv_nsec);

  fprintf(fpout, "%s %-12s ", timestring, keyw);

  va_list args;
  va_start(args, fmt);

  vfprintf(fpout, fmt, args);

  fprintf(fpout, "\n");

  fflush(fpout);

  va_end(args);

  if (strcmp(keyw, "LOGFILECLOSE") == 0) {
    fclose(fpout);
    LogOutOpen = 1;
  }

  return RETURN_SUCCESS;
}

/** @brief Establish sym link for convenience
 *
 * This is a one-time function when running FPS init.\n
 * Creates a human-readable informative sym link to outlog\n
 */
errno_t functionparameter_outlog_namelink() {
#ifndef STANDALONE

  char shmdname[STRINGMAXLEN_SHMDIRNAME];
  function_parameter_struct_shmdirname(shmdname);

  char logfname[STRINGMAXLEN_FULLFILENAME];
  getFPSlogfname(logfname);

  char linkfname[STRINGMAXLEN_FULLFILENAME];
  WRITE_FULLFILENAME(linkfname, "%s/fpslog.%s", shmdname,
                     data.FPS_PROCESS_TYPE);

  if (symlink(logfname, linkfname) == -1) {
    int errnum = errno;
    fprintf(stderr, "Error symlink: %s\n", strerror(errnum));
    PRINT_ERROR("symlink error %s %s", logfname, linkfname);
  }

#endif  // STANDALONE

  return RETURN_SUCCESS;
}
