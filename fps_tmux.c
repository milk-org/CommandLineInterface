/**
 * @file    fps_tmux.c
 * 
 * @brief   tmux session management
 * 
 * tmux wrapper for FPS control
 */



#include "CommandLineInterface/CLIcore.h"


/** @brief Kill FPS tmux sesssion
 *
 */
errno_t functionparameter_FPS_tmux_kill(
    FUNCTION_PARAMETER_STRUCT *fps
)
{
    // terminate tmux sessions
    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:ctrl C-c 2> /dev/null",
                           fps->md->name);
    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:ctrl \"exit\" C-m 2> /dev/null",
                           fps->md->name);
                           
    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:conf C-c 2> /dev/null",
                           fps->md->name);                           
    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:conf \"exit\" C-m 2> /dev/null",
                           fps->md->name);

    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:run C-c 2> /dev/null",
                           fps->md->name);
    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:run \"exit\" C-m 2> /dev/null",
                           fps->md->name);


	EXECUTE_SYSTEM_COMMAND("tmux kill-session -t %s 2> /dev/null", fps->md->name);

    return RETURN_SUCCESS;
}





/** @brief Initialize FPS tmux sesssion
 *
 */
errno_t functionparameter_FPS_tmux_init(
    FUNCTION_PARAMETER_STRUCT *fps
)
{
	int funcstring_maxlen = 10000;
	int argstring_maxlen = 1000;
	
    // terminate tmux sessions
    functionparameter_FPS_tmux_kill(fps);

    EXECUTE_SYSTEM_COMMAND("tmux kill-session -t %s 2> /dev/null", fps->md->name);

    EXECUTE_SYSTEM_COMMAND("tmux new-session -s %s -d", fps->md->name);
    EXECUTE_SYSTEM_COMMAND("tmux rename-window -t %s:0 ctrl", fps->md->name);


    EXECUTE_SYSTEM_COMMAND("tmux new-window -t %s -n conf", fps->md->name);
    EXECUTE_SYSTEM_COMMAND("tmux new-window -t %s -n run", fps->md->name);


    // Write functions to tmux windows
    //
    char functionstring[funcstring_maxlen];
    char argstring[argstring_maxlen];
    char argstringcp[argstring_maxlen];

	if(fps->md->NBnameindex > 0)
	{		
		snprintf(argstring, argstring_maxlen, "%s", fps->md->nameindexW[0]);
	}
	else
	{
		sprintf(argstring, " ");
	}
	
	for(int i=1; i<fps->md->NBnameindex; i++)
	{
		snprintf(argstringcp, argstring_maxlen, "%s %s", argstring, fps->md->nameindexW[i]);
		strcpy(argstring, argstringcp);
	}


	// module load string
	char mloadstring[2000];
	sprintf(mloadstring, " ");
	for(int m=0; m < fps->md->NBmodule; m++)
	{
		sprintf(mloadstring, "%smload %s;", mloadstring, fps->md->modulename[m]);		 
	}
	

	// confstart
	//
    sprintf(functionstring,
            "function fpsconfstart {\n"
            "echo \"STARTING CONF PROCESS\"\n"
            "%s-exec -n %s \\\"%s%s _CONFSTART_ %s\\\"\n"
            "}\n",
            fps->md->callprogname,
            fps->md->name,
            mloadstring,
            fps->md->callfuncname,
            argstring
           );

    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:conf \"%s\" C-m", 
		fps->md->name,
		functionstring);

    
    // runstart
    //
    sprintf(functionstring,
            "function fpsrunstart {\n"
            "echo \"STARTING RUN PROCESS\"\n"
            "\\${TASKSETCMDPREFIX} %s-exec -n %s \\\"%s%s _RUNSTART_ %s\\\"\n"
            "}\n",
            fps->md->callprogname,
            fps->md->name,
            mloadstring,
            fps->md->callfuncname,
            argstring
           );

    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:run \"%s\" C-m", 
		fps->md->name,
		functionstring);    

    
    // runstop
    //
    sprintf(functionstring,
            "function fpsrunstop {\n"
            "echo \"STOPPING RUN PROCESS\"\n"
            "%s-exec -n %s \\\"%s%s _RUNSTOP_ %s\\\"\n"
            "}\n",
            fps->md->callprogname,
            fps->md->name,
            mloadstring,
            fps->md->callfuncname,
            argstring
           );

    EXECUTE_SYSTEM_COMMAND("tmux send-keys -t %s:run \"%s\" C-m", 
		fps->md->name,
		functionstring);    
    


    return RETURN_SUCCESS;
}

