/**
 * @file    fps_load.c
 * @brief   Load FPS
 */



#include "CommandLineInterface/CLIcore.h"
#ifndef STANDALONE
#include "COREMOD_memory/COREMOD_memory.h"
#endif  // STANDALONE

#include "fps_connect.h"



long function_parameter_structure_load(
    char *fpsname
)
{
    long fpsID = -1;


    DEBUG_TRACEPOINT("loading FPS %s", fpsname);

#ifndef STANDALONE
    fpsID = fps_ID(fpsname);

    if(fpsID == -1)
    {
        // not found, searching


        // next fpsID available
        fpsID = 0;

        int foundflag = 0;

        while ( (foundflag == 0) && (fpsID < data.NB_MAX_FPS))
        {
            if ( data.fps[fpsID].SMfd < 0 )
            {
                foundflag = 1;
            }
            else
            {
                fpsID++;
            }
        }

        if(foundflag == 1)
        {
            data.fps[fpsID].NBparam = function_parameter_struct_connect(fpsname, &data.fps[fpsID], FPSCONNECT_SIMPLE);
            if (data.fps[fpsID].NBparam < 1 )
            {
                printf("--- cannot load FPS %s\n", fpsname);
                fpsID = -1;
            }
            else
            {
                printf("--- loaded FPS %s to ID %ld\n", fpsname, fpsID);
            }
        }
        else
        {
            fpsID = -1;
        }
    }
    else
    {
		printf("FPS already loaded at index %ld\n", fpsID);
	}
#else
    printf("--- cannot load FPS %s in standalone\n", fpsname);

#endif  // STANDALONE

    return fpsID;
}
