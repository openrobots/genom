/*
 * Copyright (c) 1993-2005 CNRS/LAAS
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS.
 */

#ifndef VXWORKS
#include <portLib.h>
#else
#include <vxWorks.h>
#endif
#include "commonStructLib.h"
#include "genom/modules.h"

/* ----------------------------------------------------------------------
 *
 * genomGiveSDIs - This function free both SDI (Functional and Control)
 *
 * return OK, or ERROR if commonStructGive fails
 */
STATUS genomGiveSDIs(void *sdic)
{
  if (commonStructGive ((void *) ((CNTRL_TASK_DESCRIPTION *)sdic)->sdif) == ERROR) 
    return ERROR;
  if (commonStructGive (sdic) == ERROR) 
    return ERROR;

  return OK;
}


/* ----------------------------------------------------------------------
 *
 * genomTakeSDIs - This function take both SDI (Functional and Control)
 *
 * return OK, or ERROR if commonStructTake fails
 */
STATUS genomTakeSDIs(void *sdic)
{
  if (commonStructTake (sdic) == ERROR) 
    return ERROR;
  if (commonStructTake ((void *) ((CNTRL_TASK_DESCRIPTION *)sdic)->sdif) == ERROR) 
    return ERROR;

  return OK;
}
