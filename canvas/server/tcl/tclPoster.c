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

/*
 * ----------------------------------------------------------------------
 * $posterName$ Poster
 */

static int $module$$posterFullName$PosterTclRead(ClientData, Tcl_Interp *, int, Tcl_Obj *const []); 

static int
$module$$posterFullName$PosterTclRead(ClientData data, Tcl_Interp *interp,
			 int objc, Tcl_Obj *const objv[])
{
   int ret;
   static $posterStructType$ *_posterData;	/* data */
   Tcl_Obj *my_own_private_unique_result;

   TEST_BAD_USAGE(objc != 1);
		 
  if ((_posterData = malloc(sizeof($dataType$))) == NULL) {
      Tcl_SetResult(interp, (char *)h2getMsgErrno(errnoGet()), TCL_STATIC);
      return TCL_ERROR;
  }

   if ($module$$posterFullName$PosterRead(_posterData) != OK) {
      Tcl_SetResult(interp, (char *)h2getMsgErrno(errnoGet()), TCL_STATIC);
      free(_posterData);
      return TCL_ERROR;
   }

   my_own_private_unique_result = Tcl_NewListObj(0, NULL);
   if (my_own_private_unique_result == NULL) {
     free(_posterData);
     return TCL_ERROR;
   }

$output$

   Tcl_SetObjResult(interp, my_own_private_unique_result);
   free(_posterData);
   return TCL_OK;
}
