/*	$LAAS$ */

/* 
 * Copyright (c) 1995-2003 LAAS/CNRS
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
#include "config.h"
__RCSID("$LAAS$");

/***
 *** Generation d'evenements date's a` la micro-seconde pres
 *** pour la trace du comportement des modules
 ***/

#ifdef VXWORKS
#include <vxWorks.h>
#include <semLib.h>
#include <rngLib.h>
#include <logLib.h>
#else
#include <portLib.h>
#endif

#include <stdio.h>

#ifdef VXWORKS
#include "../h2Clock/h2Clock.h" /* XXX utilise -I$(ROOTH2)/xxx/ */
#endif

#include "moduleEvents.h"

/*----------------------------------------------------------------------*/

/**
 ** Variables statiques
 **/
#ifdef VXWORKS
static SEM_ID moduleEventSem;
static RING_ID moduleEventRng;
static BOOL moduleEventInitialized = FALSE;
static BOOL moduleEventCapture = FALSE;
#endif

/* Timeout acces au semaphore d'exclusion mutuelle */
#define MODULE_EVENT_TIMEOUT 5 

/*----------------------------------------------------------------------*/

/**
 ** moduleEventInit - initialisation de la collecte des evenements 
 **/
STATUS
moduleEventInit(int size)
{
#ifdef VXWORKS
    if (moduleEventInitialized) {
	return(ERROR);
    }
    moduleEventRng = rngCreate(size);
    if (moduleEventRng == NULL) { 
	return(ERROR);
    }
    moduleEventSem = semMCreate(SEM_Q_PRIORITY | SEM_INVERSION_SAFE);
    if (moduleEventSem == NULL) {
	rngDelete(moduleEventRng);
	return(ERROR);
    }
#endif
    return(OK);
}

#ifdef VXWORKS
/*----------------------------------------------------------------------*/

/**
 **  moduleEventStart - demarre l'acquisition des evenements
 **/
STATUS 
moduleEventStart(void)
{
    if (rngIsFull(moduleEventRng)) {
	fprintf(stderr, "moduleEventStart: ring buffer full\n");
	return(ERROR);
    }
    moduleEventCapture = TRUE;
    return(OK);
}

/*----------------------------------------------------------------------*/

/**
 ** moduleEventStop - arrete l'acquisition des evenements
 **/
void 
moduleEventStop(void)
{
    moduleEventCapture = FALSE;
}
#endif
/*----------------------------------------------------------------------*/

/**
 ** sendModuleEvent - enregistrement d'un evenement
 **
 ** parametres: moduleEvent: pointeur sur une structure MODULE_EVENT 
 **                          description de l'evenement
 ** 
 ** retourne: la date en micro-secondes de l'evenement
 **/

unsigned long
sendModuleEvent(MODULE_EVENT_STR *moduleEvent)
{
#ifdef VXWORKS
    unsigned long stamp;

    stamp = h2ClockuSec();
    if (moduleEventCapture == FALSE) {
	return(stamp);
    }
    moduleEvent->stamp = stamp;
    if (semTake(moduleEventSem, MODULE_EVENT_TIMEOUT) == ERROR) {
	logMsg("sendModuleEvent cannot take sem");
	return(0);
    }
    rngBufPut(moduleEventRng, (char *)moduleEvent, sizeof(MODULE_EVENT_STR));
    if (rngIsFull(moduleEventRng)) {
	moduleEventCapture = FALSE;
    }
    semGive(moduleEventSem);
    return(stamp);

#else /* VXWORKS */
    return(0);
#endif /* VXWORKS */
} /* sendModuleEvent */

/*----------------------------------------------------------------------*/

/**
 ** moduleEventDump - vide le buffer d'evenements dans un fichier
 **/
#ifdef VXWORKS
STATUS
moduleEventDump(char *name)
{
    FILE *out;
    MODULE_EVENT_STR event;

    out = fopen(name, "w");
    if (out == NULL) { 
	return(ERROR);
    }
    moduleEventStop();
    semTake(moduleEventSem, WAIT_FOREVER);
    while (1) {
	if (rngBufGet(moduleEventRng, (char *)&event, 
		      sizeof(MODULE_EVENT_STR)) == 0) {
	    break;
	}
	fwrite(&event, sizeof(MODULE_EVENT_STR), 1, out);
    }
    semGive(moduleEventSem);
    fclose(out);
    return(OK);
}

#endif

