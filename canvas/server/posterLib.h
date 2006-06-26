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

#ifndef $module$_POSTER_LIB_H
#define $module$_POSTER_LIB_H

#include "posterLib.h"

/* Types definis ou utilises par le module */
#include "$module$Type.h"
#include "$module$Error.h"

/* 
 * Definition de la SDI_C 
*/
typedef struct {
  CNTRL_TASK_DESCRIPTION cntrlTask;
  EXEC_TASK_DESCRIPTION execTaskTab[$MODULE$_NB_EXEC_TASK];
  ACTIVITY_DESCRIPTION activityTab[MAX_ACTIVITIES];
} $MODULE$_CNTRL_STR;

/* For consistency with other posters */
typedef $MODULE$_CNTRL_STR $MODULE$_CNTRL_POSTER_STR;
 
/* 
 * Definition des structures de posters
 */
#include "$module$PosterLibStruct.h"

/*---------------- PROTOTYPES DES FONCTIONS EXTERNES ------------------*/

#include "$module$PosterLibProto.h"


/* Posters */
#define $MODULE$_CNTRL_POSTER_NAME         "$module$Cntrl"
$listPosterNameDeclare$


/*-------------------- Fin de chargement du fichier -----------------------*/
#endif
