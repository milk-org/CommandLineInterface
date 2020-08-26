/**
 * @file    fps_connect.h
 * @brief   connect to FPS
 */

#include "function_parameters.h"

#ifndef FPS_CONNECT_H
#define FPS_CONNECT_H


long function_parameter_struct_connect(
    const char *name,
    FUNCTION_PARAMETER_STRUCT *fps,
    int fpsconnectmode
);

#endif