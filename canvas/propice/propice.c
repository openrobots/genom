/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS
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

/*----------------------------------------------------------------------
 *   Automatically generated: do not edit by hand.
 *----------------------------------------------------------------------*/

#ifndef UNIX
#include <vxWorks.h>
#else
#include <portLib.h>
#endif

#ifdef UNIX
#include <stdlib.h>
#else
#include <taskLib.h>
#endif

#include "$module$MsgLib.h"
#undef FREE

#include "macro-pub.h"
#include "opaque-pub.h"
#include "constant-pub.h"
#include "propice-type-pub.h"
#include "pu-mk-term_f.h"
#include "pu-parse-tl_f.h"

#include "propice-type_f-pub.h"
#include "propice-sprint_f-pub.h"

#include "user-decl-propice-post_f.h"
#include "user-decl-propice-com_f.h"

#include "propice-com.h"
#include "propice-com_f.h"
#include "propice-post_f.h"

#include "pu-enum_f.h"
#include "pu-genom_f.h"

#include "$module$Error.h"
#include "$module$MsgLib.h"
#include "$module$PosterLib.h"

#include "propice/$module$DecodePropice.h"
#include "propice/$module$EncodePropice.h"
