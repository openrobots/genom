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


#ifndef $module$_ERROR_H
#define $module$_ERROR_H

#include "h2errorLib.h"

#define M_$module$CntrlTask ($numModule$ )
#define M_$module$MsgLib    ($numModule$ + 1)
#define M_$module$PosterLib    ($numModule$ + 2)

$listExecTask$           

#define S_$module$CntrlTask_ACTIVITY_INTERRUPTED  (M_$module$CntrlTask << 16 | 0)
#define S_$module$CntrlTask_TOO_MANY_ACTIVITIES (M_$module$CntrlTask << 16 | 1)
#define S_$module$CntrlTask_ACTIVITY_FAILED (M_$module$CntrlTask << 16 | 2)
#define S_$module$CntrlTask_WAIT_ABORT_ZOMBIE_ACTIVITY (M_$module$CntrlTask << 16 | 3)
#define S_$module$CntrlTask_UNKNOWN_ACTIVITY (M_$module$CntrlTask << 16 | 4)
#define S_$module$CntrlTask_FORBIDDEN_ACTIVITY_TRANSITION (M_$module$CntrlTask << 16 | 5)
#define S_$module$CntrlTask_SYSTEM_ERROR (M_$module$CntrlTask << 16 | 6)

#define S_$module$CntrlTask_ACTIVITY_ALREADY_ENDED (M_$module$CntrlTask << 16 | 7)
#define S_$module$CntrlTask_WAIT_INIT_RQST (M_$module$CntrlTask << 16 | 8)

#define S_$module$CntrlTask_CONTROL_CODEL_ERROR (M_$module$CntrlTask << 16 | 9)

/* Il faudra des numeros en plus ! */
/* #define S_$module$CntrlTask_EXEC_TASK_SUSPENDED  (M_$module$CntrlTask << 16 | 10) */

#define S_$module$MsgLib_BAD_BLOCK_TYPE (M_$module$MsgLib << 16 | 0)

$listCntrlFailures$
$listExecFailures$ 

/*-------------------- Fin de chargement du fichier -----------------------*/
#endif
