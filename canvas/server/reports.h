/*	$LAAS$ */

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

/*------------------  Fichier généré automatiquement ------------------*/
/*------------------  Ne pas éditer manuellement !!! ------------------*/


#ifndef $module$_REPORTS_H
#define $module$_REPORTS_H

#include "genomReportsLib.h"


#define M_$module$        ($numModule$)
#define M_$module$Std     ($numModule$ + 1)

#define $MODULE$_NB_REPORTS    $nbTabFailures$
#define $MODULE$_MODULE_NAME   "$MODULE$"

#define S_$module$_ACTIVITY_INTERRUPTED   (M_$module$Std << 16 | S_genom_ACTIVITY_INTERRUPTED)
#define S_$module$_TOO_MANY_ACTIVITIES    (M_$module$Std << 16 | S_genom_TOO_MANY_ACTIVITIES)
#define S_$module$_ACTIVITY_FAILED        (M_$module$Std << 16 | S_genom_ACTIVITY_FAILED)
#define S_$module$_WAIT_ABORT_ZOMBIE_ACTIVITY (M_$module$Std << 16 | S_genom_WAIT_ABORT_ZOMBIE_ACTIVITY)
#define S_$module$_UNKNOWN_ACTIVITY       (M_$module$Std << 16 | S_genom_UNKNOWN_ACTIVITY)
#define S_$module$_FORBIDDEN_ACTIVITY_TRANSITION (M_$module$Std << 16 | S_genom_FORBIDDEN_ACTIVITY_TRANSITION)
#define S_$module$_SYSTEM_ERROR           (M_$module$Std << 16 | S_genom_SYSTEM_ERROR)
#define S_$module$_ACTIVITY_ALREADY_ENDED (M_$module$Std << 16 | S_genom_ACTIVITY_ALREADY_ENDED)
#define S_$module$_WAIT_INIT_RQST         (M_$module$Std << 16 | S_genom_WAIT_INIT_RQST)
#define S_$module$_BAD_BLOCK_TYPE         (M_$module$Std << 16 | S_genom_BAD_BLOCK_TYPE)

$listCntrlFailures$
$listExecFailures$

static H2_FAILED_STRUCT $MODULE$_REPORTS_TAB[] = {
$listTabFailures$
};



static GENOM_REPORTS_STRUCT $MODULE$_REPORTS_STRUCT = {
    M_$module$,
    $MODULE$_NB_REPORTS,
    $MODULE$_MODULE_NAME,
    $MODULE$_REPORTS_TAB
};


/*-------------------- Fin de chargement du fichier -----------------------*/
#endif
