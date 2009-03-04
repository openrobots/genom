
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

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/kernel.h>
# include <linux/sched.h>
#else
# include <stdio.h>
# include <string.h>
#endif

#include <portLib.h>

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
 ** Macros to acces the SDI_C
 **
 ** To be used with the following parameters:
 **
 ** p: SDI_C   t: <MODULE>_<TASK>_NUM    a: CURRENT_ACTIVITY_NUM(t)
 **
 ** t can be also replaced by the macro: CURRENT_EXEC_TASK
 **
 **/

/* The execution task  */
#define EXEC_TASK_ID(t)         M_EXEC_TASK_ID(SDI_C,t)
#define EXEC_TASK_STATUS(t)     M_EXEC_TASK_STATUS(SDI_C,t)
#define EXEC_TASK_PERIOD(t)     M_EXEC_TASK_PERIOD(SDI_C,t)
#define EXEC_TASK_MAX_PERIOD(t) M_EXEC_TASK_MAX_PERIOD(SDI_C,t)
#define EXEC_TASK_ON_PERIOD(t)  M_EXEC_TASK_ON_PERIOD(SDI_C,t)
#define EXEC_TASK_TIME_BEGIN_LOOP(t) M_EXEC_TASK_TIME_BEGIN_LOOP(SDI_C,t)
#define EXEC_TASK_TIME_END_LOOP(t) M_EXEC_TASK_TIME_END_LOOP(SDI_C,t)
#define EXEC_TASK_DURATION_LOOP(t) M_EXEC_TASK_DURATION_LOOP(SDI_C,t)
#define EXEC_TASK_BILAN(t)      M_EXEC_TASK_BILAN(SDI_C,t)
#define EXEC_TASK_NB_ACTI(t)    M_EXEC_TASK_NB_ACTI(SDI_C,t)
#define EXEC_TASK_DONTLOCK_SDI(t) M_EXEC_TASK_DONTLOCK_SDI(SDI_C,t)
#define EXEC_TASK_POSTER_ID(t)  M_EXEC_TASK_POSTER_ID(SDI_C,t)
#define CURRENT_ACTIVITY_NUM(t) M_CURRENT_ACTIVITY_NUM(SDI_C,t)
#define EXEC_TASK_WAKE_UP_FLAG(t) M_EXEC_TASK_WAKE_UP_FLAG(SDI_C,t)
#define CURRENT_EXEC_TASK         M_CURRENT_EXEC_TASK(SDI_C)

/* The activities */
#define ACTIVITY_OUTPUT_ID(a)   M_ACTIVITY_OUTPUT_ID(SDI_C,a)
#define ACTIVITY_OUTPUT_SIZE(a) M_ACTIVITY_OUTPUT_SIZE(SDI_C,a)
#define ACTIVITY_INPUT_ID(a)    M_ACTIVITY_INPUT_ID(SDI_C,a)
#define ACTIVITY_INPUT_SIZE(a)  M_ACTIVITY_INPUT_SIZE(SDI_C,a)
#define ACTIVITY_RQST_ID(a)     M_ACTIVITY_RQST_ID(SDI_C,a)
#define ACTIVITY_RQST_TYPE(a)   M_ACTIVITY_RQST_TYPE(SDI_C,a)
#define ACTIVITY_ID(a)          M_ACTIVITY_ID(SDI_C,a)
#define ACTIVITY_TASK_NUM(a)    M_ACTIVITY_TASK_NUM(SDI_C,a)
#define ACTIVITY_STATUS(a)      M_ACTIVITY_STATUS(SDI_C,a)
#define ACTIVITY_EVN(a)         M_ACTIVITY_EVN(SDI_C,a)
#define ACTIVITY_BILAN(a)       M_ACTIVITY_BILAN(SDI_C,a)
#define ACTIVITY_REENTRANCE(a)  M_ACTIVITY_REENTRANCE(SDI_C,a)
#define ACTIVITY_NB_INCOMP(a)   M_ACTIVITY_NB_INCOMP(SDI_C,a)
#define ACTIVITY_TAB_INCOMP(a)  M_ACTIVITY_TAB_INCOMP(SDI_C,a)
 
/* The control task */
#define CNTRL_TASK_ID          M_CNTRL_TASK_ID(SDI_C)
#define CNTRL_TASK_STATUS      M_CNTRL_TASK_STATUS(SDI_C)
#define CNTRL_TASK_BILAN       M_CNTRL_TASK_BILAN(SDI_C)
#define NB_ACTIVITIES          M_NB_ACTIVITIES(SDI_C)
#define STOP_MODULE_FLAG       M_STOP_MODULE_FLAG(SDI_C)
#define INIT_RQST              M_INIT_RQST(SDI_C)
#define CNTRL_NB_EXEC_TASKS    M_CNTRL_NB_EXEC_TASKS(SDI_C)
#define CNTRL_SDI_F            M_CNTRL_SDI_F(SDI_C)

#define GENOM_VERBOSE_LEVEL    M_GENOM_VERBOSE_LEVEL(SDI_C)
#define GENOM_PRINT_TIME_FLAG  M_GENOM_PRINT_TIME_FLAG(SDI_C)

#define GENOM_VERBOSE(fmt, args...) {\
    if(GENOM_VERBOSE_LEVEL>=1){fprintf(stderr,"$module$: "fmt"\n", ##args);}\
}

#define GENOM_PRINT(fmt, args...) {\
fprintf(stderr, \
        "$module$ %s(): "fmt "\n"\
        "$module$ %s(): file %s, line %d\n", \
         __func__,  ##args, \
         __func__,  __FILE__, __LINE__);\
}

#define GENOM_PRINT_REPORT(report) {\
fprintf(stderr, \
        "$module$ %s(): " #report "\n" \
        "$module$ %s(): file %s, line %d\n", \
         __func__,  \
         __func__,  __FILE__, __LINE__);			\
}

#define GENOM_UNLOCK_SDI       M_GENOM_UNLOCK_SDI(SDI_C)
#define GENOM_LOCK_SDI         M_GENOM_LOCK_SDI(SDI_C)

/* Macros */

/* Wakeup an exec task */
#define WAKE_UP_EXEC_TASK(t)       (EXEC_TASK_WAKE_UP_FLAG(t) = TRUE)
/* Abort an activity
 * ONLY FOR CONTROL TASK */
#define CNTRL_ABORT_ACTIVITY(p,a)    (ACTIVITY_EVN(a) = INTER)
 

/*-------------------- Fin de chargement du fichier -----------------------*/

#endif
