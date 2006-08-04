/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS
 * Sara Fleury -   Apr  1 1999
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
#ifndef _OPENPRSGEN_H
#define _OPENPRSGEN_H

/* Canevas utilisé par openprsGen */
#define PROTO_OPENPRS_C         "server/openprs/openprs.c"
#define PROTO_OPENPRS_PROTO     "server/openprs/openprsProto.h"

/* Canevas utilisé par openprsDecodeGen */
#define PROTO_OPENPRS_RQST_OP   "server/openprs/openprsRqst.op"
#define PROTO_OPENPRS_PSTR_OP   "server/openprs/openprsPoster.op"

/* Canevas utilisé par openprsDecodeGen */
#define PROTO_OPENPRS_DECODE_C  "server/openprs/openprsDecode.c"
#define PROTO_OPENPRS_DECODE_H  "server/openprs/openprsDecode.h"

/* Canevas utilisé par openprsEncodeGen */
#define PROTO_OPENPRS_ENCODE_C  "server/openprs/openprsEncode.c"
#define PROTO_OPENPRS_ENCODE_H  "server/openprs/openprsEncode.h"

/* Prototypes des fonctions de generation */
extern int openprsGen(FILE *out);
extern int openprsMakeGen ( FILE *out );
extern int openprsEncodeGen ( FILE *out );
extern int openprsDecodeGen ( FILE *out );
extern int openprsOpGen(FILE *out);
extern char *getOpenprsTYPE(TYPE_STR *type);

#endif
