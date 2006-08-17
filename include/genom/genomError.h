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
#ifndef REPORTS_STD_H
#define REPORTS_STD_H

#include "h2errorLib.h"

#define M_stdGenoM 100

#define S_stdGenoM_ACTIVITY_INTERRUPTED          H2_ENCODE_STD_ERR(M_stdGenoM, 1)
#define S_stdGenoM_TOO_MANY_ACTIVITIES           H2_ENCODE_STD_ERR(M_stdGenoM, 2)
#define S_stdGenoM_ACTIVITY_FAILED               H2_ENCODE_STD_ERR(M_stdGenoM, 3)
#define S_stdGenoM_WAIT_ABORT_ZOMBIE_ACTIVITY    H2_ENCODE_STD_ERR(M_stdGenoM, 4)
#define S_stdGenoM_UNKNOWN_ACTIVITY              H2_ENCODE_STD_ERR(M_stdGenoM, 5)
#define S_stdGenoM_FORBIDDEN_ACTIVITY_TRANSITION H2_ENCODE_STD_ERR(M_stdGenoM, 6)
#define S_stdGenoM_SYSTEM_ERROR                  H2_ENCODE_STD_ERR(M_stdGenoM, 7)
#define S_stdGenoM_ACTIVITY_ALREADY_ENDED        H2_ENCODE_STD_ERR(M_stdGenoM, 8)
#define S_stdGenoM_WAIT_INIT_RQST                H2_ENCODE_STD_ERR(M_stdGenoM, 9)
#define S_stdGenoM_CONTROL_CODEL_ERROR           H2_ENCODE_STD_ERR(M_stdGenoM, 10)
#define S_stdGenoM_EXEC_TASK_SUSPENDED           H2_ENCODE_STD_ERR(M_stdGenoM, 11)
#define S_stdGenoM_BAD_BLOCK_TYPE                H2_ENCODE_STD_ERR(M_stdGenoM, 12)
#define S_stdGenoM_BAD_POSTER_TYPE               H2_ENCODE_STD_ERR(M_stdGenoM, 13)


/* H2_ERROR[] */
#define GENOM_H2_ERR_MSGS \
{ \
  {"ACTIVITY_INTERRUPTED",          S_stdGenoM_ACTIVITY_INTERRUPTED}, \
  {"TOO_MANY_ACTIVITIES",           S_stdGenoM_TOO_MANY_ACTIVITIES}, \
  {"ACTIVITY_FAILED",               S_stdGenoM_ACTIVITY_FAILED}, \
  {"WAIT_ABORT_ZOMBIE_ACTIVITY",    S_stdGenoM_WAIT_ABORT_ZOMBIE_ACTIVITY}, \
  {"UNKNOWN_ACTIVITY",              S_stdGenoM_UNKNOWN_ACTIVITY}, \
  {"FORBIDDEN_ACTIVITY_TRANSITION", S_stdGenoM_FORBIDDEN_ACTIVITY_TRANSITION}, \
  {"SYSTEM_ERROR",                  S_stdGenoM_SYSTEM_ERROR}, \
  {"ACTIVITY_ALREADY_ENDED",        S_stdGenoM_ACTIVITY_ALREADY_ENDED}, \
  {"WAIT_INIT_RQST",                S_stdGenoM_WAIT_INIT_RQST}, \
  {"CONTROL_CODEL_ERROR",           S_stdGenoM_CONTROL_CODEL_ERROR},\
  {"EXEC_TASK_SUSPENDED",           S_stdGenoM_EXEC_TASK_SUSPENDED},	\
  {"BAD_BLOCK_TYPE",                S_stdGenoM_BAD_BLOCK_TYPE},	\
  {"BAD_POSTER_TYPE",               S_stdGenoM_BAD_POSTER_TYPE} \
}

extern int genomRecordH2errMsgs();

#endif
