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

#define M_genom 550

#define S_genom_ACTIVITY_INTERRUPTED          1
#define S_genom_TOO_MANY_ACTIVITIES           2
#define S_genom_ACTIVITY_FAILED               3
#define S_genom_WAIT_ABORT_ZOMBIE_ACTIVITY    4
#define S_genom_UNKNOWN_ACTIVITY              5
#define S_genom_FORBIDDEN_ACTIVITY_TRANSITION 6
#define S_genom_SYSTEM_ERROR                  7
#define S_genom_ACTIVITY_ALREADY_ENDED        8
#define S_genom_WAIT_INIT_RQST                9
#define S_genom_CONTROL_CODEL_ERROR          10
#define S_genom_EXEC_TASK_SUSPENDED          11
#define S_genom_BAD_BLOCK_TYPE               12

#define GENOM_STD_FAILED \
{ \
  {"ACTIVITY_INTERRUPTED",   S_genom_ACTIVITY_INTERRUPTED}, \
  {"TOO_MANY_ACTIVITIES",    S_genom_TOO_MANY_ACTIVITIES}, \
  {"ACTIVITY_FAILED",        S_genom_ACTIVITY_FAILED}, \
  {"WAIT_ABORT_ZOMBIE_ACTIVITY", S_genom_WAIT_ABORT_ZOMBIE_ACTIVITY}, \
  {"UNKNOWN_ACTIVITY",       S_genom_UNKNOWN_ACTIVITY}, \
  {"FORBIDDEN_ACTIVITY_TRANSITION", S_genom_FORBIDDEN_ACTIVITY_TRANSITION}, \
  {"SYSTEM_ERROR",           S_genom_SYSTEM_ERROR}, \
  {"ACTIVITY_ALREADY_ENDED", S_genom_ACTIVITY_ALREADY_ENDED}, \
  {"WAIT_INIT_RQST",         S_genom_WAIT_INIT_RQST}, \
  {"CONTROL_CODEL_ERROR",  S_genom_CONTROL_CODEL_ERROR},\
  {"EXEC_TASK_SUSPENDED", S_genom_EXEC_TASK_SUSPENDED},	\
  {"BAD_BLOCK_TYPE",         S_genom_BAD_BLOCK_TYPE} \
}


#endif
