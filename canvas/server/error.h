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

#define M_$module$    ($numModule$)
#define M_$module$Std ($numModule$ + 1)

#define S_$module$Std_ACTIVITY_INTERRUPTED          (M_$module$Std << 16 | 1 )
#define S_$module$Std_TOO_MANY_ACTIVITIES           (M_$module$Std << 16 | 2 )
#define S_$module$Std_ACTIVITY_FAILED               (M_$module$Std << 16 | 3 )
#define S_$module$Std_WAIT_ABORT_ZOMBIE_ACTIVITY    (M_$module$Std << 16 | 4 )
#define S_$module$Std_UNKNOWN_ACTIVITY              (M_$module$Std << 16 | 5 )
#define S_$module$Std_FORBIDDEN_ACTIVITY_TRANSITION (M_$module$Std << 16 | 6 )
#define S_$module$Std_SYSTEM_ERROR                  (M_$module$Std << 16 | 7 )
#define S_$module$Std_ACTIVITY_ALREADY_ENDED        (M_$module$Std << 16 | 8 )
#define S_$module$Std_WAIT_INIT_RQST                (M_$module$Std << 16 | 9 )
#define S_$module$Std_CONTROL_CODEL_ERROR           (M_$module$Std << 16 | 10 )
#define S_$module$Std_EXEC_TASK_SUSPENDED           (M_$module$Std << 16 | 11 )
#define S_$module$Std_BAD_BLOCK_TYPE                (M_$module$Std << 16 | 12 )

$listCntrlFailures$
$listExecFailures$ 

/*-------------------- Fin de chargement du fichier -----------------------*/
#endif
