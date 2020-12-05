/**
 * @file    fps_CONFupdate.c
 * @brief   FPS conf process update
 */

#include "CommandLineInterface/CLIcore.h"
#include "function_parameters.h"

/** @brief FPS update CONF process
 *
 */
int functionparameter_CONFupdate(FUNCTION_PARAMETER_STRUCT *fps) {
  int looptry = 1;
  int looptrycnt = 0;
  unsigned int timercnt = 0;
  useconds_t dt = 100;
  unsigned int timercntmax = 10000;  // 1 sec max

  while (looptry == 1) {
    DEBUG_TRACEPOINT(" ");
    fps->md->signal |=
        FUNCTION_PARAMETER_STRUCT_SIGNAL_CHECKED;  // update status: check
                                                   // waiting to be done
    fps->md->signal |=
        FUNCTION_PARAMETER_STRUCT_SIGNAL_UPDATE;  // request an update

    while (((fps->md->signal &
             FUNCTION_PARAMETER_STRUCT_SIGNAL_CHECKED)) &&
           (timercnt < timercntmax)) {
      usleep(dt);
      timercnt++;
    }
    usleep(dt);
    timercnt++;

    functionparameter_outlog(
        "CONFWUPDATE", "[%d] waited %d us on FPS %s. conferrcnt = %d",
        looptrycnt, dt * timercnt, fps->md->name,
        fps->md->conferrcnt);

    looptrycnt++;

    if (fps->md->conferrcnt == 0)  // no error ! we can proceed
    {
      looptry = 0;
    }

    if (timercnt > timercntmax)  // ran out of time ... giving up
    {
      looptry = 0;
    }
  }
  return EXIT_SUCCESS;
}
