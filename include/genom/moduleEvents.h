
/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * All rights reserved.
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#ifndef MODULE_EVENT_H
#define MODULE_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum MODULE_EVENT_TYPE {
  CNTRL_START_EVENT,             /* Request reception */
  CNTRL_END_EVENT,               /* Reply emission */
  EXEC_START_EVENT,              /* Starting exec task iteration */
  EXEC_END_EVENT,                /* Ending exec task iteration */
  STATE_START_EVENT,             /* Starting activity state execution */
  STATE_END_EVENT                /* Ending activity state execution */
} MODULE_EVENT_TYPE;


typedef struct MODULE_EVENT_STR {
  unsigned long     stamp;	   /* time stamp (not used) */
  unsigned short    moduleNum;     /* Module number */
  unsigned int      taskNum;       /* Exec task number */
  int               rqstType;      /* Request type (-1: no request) */
  int               activityNum;   /* Activity number (-1: permanent activity) */
  MODULE_EVENT_TYPE eventType;     /* MODULE_EVENT_TYPE */
  ACTIVITY_STATE    activityState; /* ACTIVITY_STATE */
} MODULE_EVENT_STR;


/* Sends module event (returns usec) */
extern void sendModuleEvent(MODULE_EVENT_STR *moduleEvent);

#ifdef __cplusplus
};
#endif

#endif
