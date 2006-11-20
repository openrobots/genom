/*	$LAAS$ */

/* --- Generated file, do not edit by hand --------------------------- */

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

/*
 * $module$Header.h 
 *
 * Include this file in codels _only_
 */

#ifndef $module$_HEADER_H
#define $module$_HEADER_H

#ifdef VXWORKS
# include <vxWorks.h>
#else
# include <portLib.h>
#endif

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/kernel.h>
# include <linux/sched.h>
#else
# include <stdio.h>
# include <string.h>
#endif

/* Inclusions concernant le module */
#include "$module$PosterLib.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "$module$PosterWriteLibProto.h"
#include "$module$PosterReadLibProto.h"
#ifdef __cplusplus
}
#endif

/* Inclusions des bibliotheques des serveurs du module */
$listServerHeader$
$listPosterHeader$

/* Nombre de types requetes definies pour ce serveur */
#define NB_RQST_TYPE                   ($nbRequest$+1)

/* Declaration des structures de donnees */
extern $MODULE$_CNTRL_STR *$module$CntrlStrId;
extern $internalDataType$ *$module$DataStrId;

#define SDI_C ($module$CntrlStrId)
#define SDI_F ($module$DataStrId)


$listExecTaskClientId$
$listPosterId$
$listExecTaskNum$


/* User functions signatures */
#ifdef __cplusplus
extern "C" {
#endif

$listUserFuncProto$

#ifdef __cplusplus
}
#endif


/**
 ** Macros d'acces a la SDI_C
 **
 ** ATTENTION: il faut que le pointeur sdiCIdAcces soit a jour
 ** 
 ** -> Risques d'effets de bord 
 **    (choix douteux du aux trop fameuses raisons historiques)
 **/

/* Les taches d'execution */
#define EXEC_TASK_ID(i)         M_EXEC_TASK_ID(SDI_C,i)
#define EXEC_TASK_STATUS(i)     M_EXEC_TASK_STATUS(SDI_C,i)
#define EXEC_TASK_PERIOD(i)     M_EXEC_TASK_PERIOD(SDI_C,i)
#define EXEC_TASK_MAX_PERIOD(i) M_EXEC_TASK_MAX_PERIOD(SDI_C,i)
#define EXEC_TASK_ON_PERIOD(i)  M_EXEC_TASK_ON_PERIOD(SDI_C,i)
#define EXEC_TASK_BILAN(i)      M_EXEC_TASK_BILAN(SDI_C,i)
#define EXEC_TASK_NB_ACTI(i)    M_EXEC_TASK_NB_ACTI(SDI_C,i)
#define EXEC_TASK_DONTLOCK_SDI(i) M_EXEC_TASK_DONTLOCK_SDI(SDI_C,i)
#define EXEC_TASK_POSTER_ID(i)  M_EXEC_TASK_POSTER_ID(SDI_C,i)
#define CURRENT_ACTIVITY_NUM(i) M_CURRENT_ACTIVITY_NUM(SDI_C,i)
#define EXEC_TASK_WAKE_UP_FLAG(i) M_EXEC_TASK_WAKE_UP_FLAG(SDI_C,i)

/* Les activites */
#define ACTIVITY_OUTPUT_ID(i)   M_ACTIVITY_OUTPUT_ID(SDI_C,i)
#define ACTIVITY_OUTPUT_SIZE(i) M_ACTIVITY_OUTPUT_SIZE(SDI_C,i)
#define ACTIVITY_INPUT_ID(i)    M_ACTIVITY_INPUT_ID(SDI_C,i)
#define ACTIVITY_INPUT_SIZE(i)  M_ACTIVITY_INPUT_SIZE(SDI_C,i)
#define ACTIVITY_RQST_ID(i)     M_ACTIVITY_RQST_ID(SDI_C,i)
#define ACTIVITY_RQST_TYPE(i)   M_ACTIVITY_RQST_TYPE(SDI_C,i)
#define ACTIVITY_ID(i)          M_ACTIVITY_ID(SDI_C,i)
#define ACTIVITY_TASK_NUM(i)    M_ACTIVITY_TASK_NUM(SDI_C,i)
#define ACTIVITY_STATUS(i)      M_ACTIVITY_STATUS(SDI_C,i)
#define ACTIVITY_EVN(i)         M_ACTIVITY_EVN(SDI_C,i)
#define ACTIVITY_BILAN(i)       M_ACTIVITY_BILAN(SDI_C,i)
#define ACTIVITY_REENTRANCE(i)  M_ACTIVITY_REENTRANCE(SDI_C,i)
#define ACTIVITY_NB_INCOMP(i)   M_ACTIVITY_NB_INCOMP(SDI_C,i)
#define ACTIVITY_TAB_INCOMP(i)  M_ACTIVITY_TAB_INCOMP(SDI_C,i)
 
/* La tache de controle */
#define CNTRL_TASK_ID          M_CNTRL_TASK_ID(SDI_C)
#define CNTRL_TASK_STATUS      M_CNTRL_TASK_STATUS(SDI_C)
#define CNTRL_TASK_BILAN       M_CNTRL_TASK_BILAN(SDI_C)
#define NB_ACTIVITIES          M_NB_ACTIVITIES(SDI_C)
#define STOP_MODULE_FLAG       M_STOP_MODULE_FLAG(SDI_C)
#define INIT_RQST              M_INIT_RQST(SDI_C)
#define CNTRL_NB_EXEC_TASKS    M_CNTRL_NB_EXEC_TASKS(SDI_C)
#define CNTRL_SDI_F            M_CNTRL_SDI_F(SDI_C)

#define GENOM_UNLOCK_SDI       M_GENOM_UNLOCK_SDI(SDI_C)
#define GENOM_LOCK_SDI         M_GENOM_LOCK_SDI(SDI_C)

/* Macros de commandes (depuis le serveur uniquement !) */

/* Eveille d'une tache d'execution 
 * Peut etre utilise par toutes les taches */
#define WAKE_UP_EXEC_TASK(i)       (EXEC_TASK_WAKE_UP_FLAG(i) = TRUE)
/* Abort d'une activite
 * Appele par le tache de controle uniquement */
#define CNTRL_ABORT_ACTIVITY(p,i)    (ACTIVITY_EVN(i) = INTER)
 

/*-------------------- Fin de chargement du fichier -----------------------*/

#endif
