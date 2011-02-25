
/* 
 * Copyright (c) 1993-2005 LAAS/CNRS
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


#ifndef $module$_TCLSERV_CLIENT_ERROR_H
#define $module$_TCLSERV_CLIENT_ERROR_H

#include <server/$module$Error.h>

/* -- MODULES ERRORS -------------------------------------------------- */

enum $module$_error {
	E_$module$_OK					  			= 0,

	/* module errors */
$listCntrlFailures$$listExecFailures$

	/* std errors */
$listStdFailures$
};

typedef enum $module$_error $module$_error_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Take an error string in parameter, and try to get the error index associated
 * to this string
 * Returns -1 in failure case (and err is not set)
 * Returns 0 in success case (and err has a sensitive value)
 */
int $module$_decode_error(const char* str, int *err);

/*
 * Take an integer representing an error, and try to get its textual
 * representation, in the $module$ context.
 *
 * Returns NULL in case of failure
 */
const char* $module$_error_str(int err);

#ifdef __cplusplus
}
#endif


/*-------------------------- end file loading ---------------------------*/
#endif
