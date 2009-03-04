
/* 
 * Copyright (c) 1993-2003 LAAS/CNRS
 * Matthieu Herrb - Thu Jul 15 1993
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
#ifndef _MSGLIBGEN_H
#define _MSGLIBGEN_H

#define PROTO_MSG_CONTROL "server/msgCntrl.c"
#define PROTO_MSG_EXEC    "server/msgExec.c"
#define PROTO_MSG_CONTROL_PROTO "server/msgLibCntrlProto.h"
#define PROTO_MSG_EXEC_PROTO    "server/msgLibExecProto.h"
#define PROTO_MSG_PROTO  "server/msgLibProto.h"
#define PROTO_MSG_HEADER  "server/msgLib.h"
#define PROTO_MSG_TITLE   "server/msgLib.c"
#define PROTO_MSG_CONNECT "server/connectLib.c"

/* msgLibGen.c */
extern int msgLibGen(FILE *out);

#endif
