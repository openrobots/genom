/* 
 * Copyright (c) 1995-2011 LAAS/CNRS
 * Matthieu Herrb - Thu Oct 19 1995
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
#include "genom-config.h"

#include <sys/types.h>
#include <stdarg.h>

/***
 *** Generation d'evenements date's a` la micro-seconde pres
 *** pour la trace du comportement des modules
 ***/

#include "genom/modules.h"
#include "genom/moduleEvents.h"
#include "genom/printStateProto.h"

#include <h2logLib.h>

/*----------------------------------------------------------------------*/

/**
 ** sendModuleEvent - enregistrement d'un evenement
 **
 ** parametres: moduleEvent: pointeur sur une structure MODULE_EVENT 
 **                          description de l'evenement
 ** 
 **/

#define ARRAY_SIZE(_a)        (sizeof((_a)) / sizeof((_a)[0]))

static char *eventTypeName[] = {
  "CNTRL_START_EVENT",
  "CNTRL_END_EVENT",  
  "EXEC_START_EVENT", 
  "EXEC_END_EVENT",   
  "STATE_START_EVENT",
  "STATE_END_EVENT"
};

static char *h2GetModuleEventType(int evnType)
{
	if (evnType < 0 || evnType >= ARRAY_SIZE(eventTypeName))
		return "UNKNOW";
	else
		return eventTypeName[evnType];
}
		    

void
sendModuleEvent(MODULE_EVENT_STR *moduleEvent)
{
	h2logMsg("module %hu task %u rqst %d activity %d state %s event %s",
	    moduleEvent->moduleNum,
	    moduleEvent->taskNum,
	    moduleEvent->rqstType,
	    moduleEvent->activityNum,
	    h2GetEvnStateString(moduleEvent->activityState),
	    h2GetModuleEventType(moduleEvent->eventType));
} /* sendModuleEvent */

/*----------------------------------------------------------------------*/

