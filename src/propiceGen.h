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
#ifndef _PROPICEGEN_H
#define _PROPICEGEN_H

/* Canevas utilisé par propiceGen */
#define PROTO_PROPICE_C         "propice/propice.c"
#define PROTO_PROPICE_PROTO     "propice/propiceProto.h"

/* Canevas utilisé par propiceDecodeGen */
#define PROTO_PROPICE_RQST_OP   "propice/propiceRqst.op"
#define PROTO_PROPICE_PSTR_OP   "propice/propicePoster.op"

/* Canevas utilisé par propiceMakeGen */
#define PROTO_PROPICE_MAKEFILE      "propice/Makefile"
#define PROTO_PROPICE_MAKEFILE_PROTOS      "propice/Makefile.protos"
#define PROTO_PROPICE_MAKEFILE_UNIX "propice/Makefile.unix"
#define PROTO_PROPICE_MAKEFILE_VX   "propice/Makefile.vxworks"

/* Canevas utilisé par propiceDecodeGen */
#define PROTO_PROPICE_DECODE_C  "propice/propiceDecode.c"
#define PROTO_PROPICE_DECODE_H  "propice/propiceDecode.h"

/* Canevas utilisé par propiceEncodeGen */
#define PROTO_PROPICE_ENCODE_C  "propice/propiceEncode.c"
#define PROTO_PROPICE_ENCODE_H  "propice/propiceEncode.h"

/* Prototypes des fonctions de generation */
#include "propiceGenProto.h"
#include "propiceMakeGenProto.h"
#include "propiceEncodeGenProto.h"
#include "propiceDecodeGenProto.h"
#include "propiceOpGenProto.h"

#endif
