/**
 * @file    IMGID.h
 * @brief   Image identifying structure
 * 
 */

#ifndef IMGID_H
#define IMGID_H

#include "ImageStreamIO/ImageStreamIO.h"

// image identifier type avoids name-resolving imageID multiple times
// provides quick and convenient access to data and metadata pointers
// Pass this as argument to functions to have both input-by-ID (ID>-1)
// and input-by-name (ID=-1).
typedef struct {
	imageID ID;       // -1 if not resolved
	char    name[32]; // used to resolve if needed
	IMAGE * im;
	IMAGE_METADATA * md; // pointer to metadata
} IMGID;


#endif
