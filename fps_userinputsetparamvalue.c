/**
 * @file    fps_userinputsetparamvalue.c
 * @brief   read user input to set parameter value
 */



#include <limits.h>


#include "CommandLineInterface/CLIcore.h"

#include "fps_PrintParameterInfo.h"
#include "fps_WriteParameterToDisk.h"

#include "TUItools.h"





/** @brief Enter new value for parameter
 *
 *
 */
int functionparameter_UserInputSetParamValue(
    FUNCTION_PARAMETER_STRUCT *fpsentry,
    int pindex
)
{
    int inputOK;
    int strlenmax = 20;
    char buff[100];
    char c = -1;

    functionparameter_PrintParameterInfo(fpsentry, pindex);


    if(fpsentry->parray[pindex].fpflag & FPFLAG_WRITESTATUS)
    {
        inputOK = 0;
        fflush(stdout);

        while(inputOK == 0)
        {
            printf("\nESC or update value : ");
            fflush(stdout);

            int stringindex = 0;

			c = get_singlechar_block();

            while((c != 27) && (c != 10) && (c != 13) && (stringindex < strlenmax - 1))
            {
                buff[stringindex] = c;
                if(c == 127)   // delete key
                {
                    putchar(0x8);
                    putchar(' ');
                    putchar(0x8);
                    stringindex --;
                }
                else
                {					
                    putchar(c);  // echo on screen
                    fflush(stdout);
                    stringindex++;
                }
                if(stringindex < 0)
                {
                    stringindex = 0;
                }
                
				c = get_singlechar_block();
            }
            buff[stringindex] = '\0';
            inputOK = 1;
        }

        if(c != 27)   // do not update value if escape key
        {

            long lval = 0;
            double fval = 0.0;
            char *endptr;
            int vOK = 1;

            switch(fpsentry->parray[pindex].type)
            {

                case FPTYPE_INT64:
                    errno = 0;    /* To distinguish success/failure after call */
                    lval = strtol(buff, &endptr, 10);

                    /* Check for various possible errors */
                    if((errno == ERANGE && (lval == LONG_MAX || lval == LONG_MIN))
                            || (errno != 0 && lval == 0))
                    {
                        perror("strtol");
                        vOK = 0;
                        sleep(1);
                    }

                    if(endptr == buff)
                    {
                        fprintf(stderr, "\nERROR: No digits were found\n");
                        vOK = 0;
                        sleep(1);
                    }

                    if(vOK == 1)
                    {
                        fpsentry->parray[pindex].val.i64[0] = lval;
                    }
                    break;

                case FPTYPE_FLOAT64:
                    errno = 0;    /* To distinguish success/failure after call */
                    fval = strtod(buff, &endptr);

                    /* Check for various possible errors */
                    if((errno == ERANGE)
                            || (errno != 0 && fval == 0.0))
                    {
                        perror("strtod");
                        vOK = 0;
                        sleep(1);
                    }

                    if(endptr == buff)
                    {
                        fprintf(stderr, "\nERROR: No digits were found\n");
                        vOK = 0;
                        sleep(1);
                    }

                    if(vOK == 1)
                    {
                        fpsentry->parray[pindex].val.f64[0] = fval;
                    }
                    break;


                case FPTYPE_FLOAT32:
                    errno = 0;    /* To distinguish success/failure after call */
                    fval = strtod(buff, &endptr);

                    /* Check for various possible errors */
                    if((errno == ERANGE)
                            || (errno != 0 && fval == 0.0))
                    {
                        perror("strtod");
                        vOK = 0;
                        sleep(1);
                    }

                    if(endptr == buff)
                    {
                        fprintf(stderr, "\nERROR: No digits were found\n");
                        vOK = 0;
                        sleep(1);
                    }

                    if(vOK == 1)
                    {
                        fpsentry->parray[pindex].val.f32[0] = fval;
                    }
                    break;


                case FPTYPE_PID :
                    errno = 0;    /* To distinguish success/failure after call */
                    lval = strtol(buff, &endptr, 10);

                    /* Check for various possible errors */
                    if((errno == ERANGE && (lval == LONG_MAX || lval == LONG_MIN))
                            || (errno != 0 && lval == 0))
                    {
                        perror("strtol");
                        vOK = 0;
                        sleep(1);
                    }

                    if(endptr == buff)
                    {
                        fprintf(stderr, "\nERROR: No digits were found\n");
                        vOK = 0;
                        sleep(1);
                    }

                    if(vOK == 1)
                    {
                        fpsentry->parray[pindex].val.pid[0] = (pid_t) lval;
                    }
                    break;


                case FPTYPE_FILENAME :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

                case FPTYPE_FITSFILENAME :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

                case FPTYPE_EXECFILENAME :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

                case FPTYPE_DIRNAME :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

                case FPTYPE_STREAMNAME :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

                case FPTYPE_STRING :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

                case FPTYPE_FPSNAME :
                    if(snprintf(fpsentry->parray[pindex].val.string[0],
                                FUNCTION_PARAMETER_STRMAXLEN, "%s", buff) < 0)
                    {
                        PRINT_ERROR("snprintf error");
                    }
                    break;

            }

            fpsentry->parray[pindex].cnt0++;

            // notify GUI
            fpsentry->md->signal |= FUNCTION_PARAMETER_STRUCT_SIGNAL_UPDATE;


            // Save to disk
            if(fpsentry->parray[pindex].fpflag & FPFLAG_SAVEONCHANGE)
            {
                functionparameter_WriteParameterToDisk(fpsentry, pindex, "setval",
                                                       "UserInputSetParamValue");
            }
        }
    }
    else
    {
        printf("%s Value cannot be modified %s\n", AECBOLDHIRED, AECNORMAL );
        c = getchar();
    }



    return 0;
}

