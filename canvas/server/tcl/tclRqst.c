
/* --- FILE GENERATED WITH GENOM, DO NOT EDIT BY HAND ------------------ */

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

/*
 * ----------------------------------------------------------------------
 * $requestName$ Request
 */

static int $module$$requestName$RqstSendCb(ClientData, Tcl_Interp *, int, Tcl_Obj *const []); 
static int $module$$requestName$ReplyRcvCb(ClientData, Tcl_Interp *, int, Tcl_Obj * const []);

static int
$module$$requestName$RqstSendCb(ClientData data, Tcl_Interp *interp,
				int objc, Tcl_Obj *const objv[])
{
   struct ModuleInfo *tclModuleInfo = (struct ModuleInfo *)data;
   int rqstId, bilan;
   char buf[10];
   $inputType$ $inputVar;$	/* input */
#if ($argNumber$ > 0)
   int ret;
   int curObjc = 0;
#endif
   char strerr[64];

   TEST_BAD_USAGE(objc != $argNumber$+1);
		 
$parseInput$

   if (tclServRqstSend(interp, Tcl_GetStringFromObj(objv[0], NULL),
		       tclModuleInfo->cid, $requestNum$,
		       $&inputVar,$ $inputSize$,
		       &rqstId, &bilan) == ERROR) {

     Tcl_SetResult(interp, h2getErrMsg(bilan, strerr, 64), TCL_VOLATILE);
      return TCL_ERROR;
   }
    
   sprintf(buf, " %d", rqstId);
   Tcl_SetResult(interp, "SENT", TCL_STATIC);
   Tcl_AppendResult(interp, buf, NULL);
   return TCL_OK;
}

static int
$module$$requestName$ReplyRcvCb(ClientData data, Tcl_Interp *interp,
				int objc, Tcl_Obj * const objv[])
{
   struct ModuleInfo *tclModuleInfo = (struct ModuleInfo *)data;
   int ret;
   int rqstId, bilan, activity;
   $outputType$ $outputVar;$	/* output */
   Tcl_Obj *my_own_private_unique_result;
   char strerr[64];

   TEST_BAD_USAGE(
      objc != 2 ||
      Tcl_GetIntFromObj(interp, objv[1], &rqstId) != TCL_OK
   );

   switch (
      /* c'est laid */
      tclServReplyRcv(interp,
		      tclModuleInfo->cid, rqstId, NO_BLOCK,
		      &activity, $&outputVar,$ $outputSize$,
		      &bilan)
      ) {
      case WAITING_INTERMED_REPLY:
	 Tcl_SetResult(interp, "SENT", TCL_STATIC);
	 return TCL_OK;

      case WAITING_FINAL_REPLY: {
	 char tmp[50];
	 sprintf(tmp, "ACK %d", activity);
	 Tcl_SetResult(interp, tmp, TCL_VOLATILE);
	 return TCL_OK;
      }

      case FINAL_REPLY_OK:
	 break;

      default:
	 Tcl_SetResult(interp, h2getErrMsg(bilan, strerr, 64), TCL_VOLATILE);
	 return TCL_ERROR;
   }

   my_own_private_unique_result = Tcl_NewListObj(0, NULL);
   if (my_own_private_unique_result == NULL) return TCL_ERROR;

   ret = Tcl_ListObjAppendElement(interp, my_own_private_unique_result,
				  Tcl_NewStringObj("TERM", -1));
   if (ret != TCL_OK) return TCL_ERROR;

   if (bilan == OK)
      ret = Tcl_ListObjAppendElement(interp, my_own_private_unique_result,
				     Tcl_NewStringObj("OK", -1));
   else
      ret = Tcl_ListObjAppendElement(interp, my_own_private_unique_result,
				     Tcl_NewStringObj(
					h2getErrMsg(bilan, strerr, 64), -1));
   if (ret != TCL_OK) return TCL_ERROR;

$output$

   Tcl_SetObjResult(interp, my_own_private_unique_result);
   return TCL_OK;
}
