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

/**
 ** This file was automagically generated.
 ** Manual editing not recommended. Changes will be overridden.
 **/

#ifndef VXWORKS
#include "portLib.h"
#include "errnoLib.h"
#else
#include <vxWorks.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>

#include "genom/tclServ.h"
#include "genom/tcl_cs.h"

#include "h2errorLib.h"
#include "$module$MsgLib.h"
#include "$module$PosterLib.h"
#include "$module$TclProto.h"

struct ModuleInfo {
   char *name;
   CLIENT_ID cid;
};

static int $module$Install(Tcl_Interp *interp,
			   struct ModuleInfo *, int install);
static int $module$ClientInitCb(ClientData clientData, Tcl_Interp *interp,
				int objc, Tcl_Obj *const objv[]);
static int $module$ClientEndCb(ClientData clientData, Tcl_Interp *interp,
			       int objc, Tcl_Obj *const objv[]);


#define TEST_BAD_USAGE(b)					\
{								\
   if (b) {							\
      Tcl_SetResult(interp, "wrong # args", TCL_STATIC);	\
      return TCL_ERROR;						\
   }								\
}

/*
 * ----------------------------------------------------------------------
 * $module$ connection / disconnection
 */

static int
$module$ClientInitCb(ClientData clientData, Tcl_Interp *interp,
		     int objc, Tcl_Obj *const objv[])
{
   char *mboxName;
   struct ModuleInfo *m;
   STATUS status;

   m = malloc(sizeof(struct ModuleInfo));
   if (m == NULL) {
      Tcl_SetResult(interp, "Not enough memory", TCL_STATIC);
      return TCL_ERROR;
   }

   if (objc > 1) {
      mboxName = Tcl_GetStringFromObj(objv[1], NULL);
      if (mboxName == NULL) return TCL_ERROR;
   } else {
      mboxName = $MODULE$_MBOX_NAME;
   }

   m->name = malloc(strlen(mboxName)+1);
   if (m->name == NULL) {
      free(m);
      Tcl_SetResult(interp, "Not enough memory", TCL_STATIC);
      return TCL_ERROR;
   }
   strcpy(m->name, mboxName);

   status = csClientInit(mboxName, $MODULE$_MAX_RQST_SIZE,
			 $MODULE$_MAX_INTERMED_REPLY_SIZE, 
			 $MODULE$_MAX_REPLY_SIZE, 
			 &m->cid);

   if (status == ERROR) {
      Tcl_SetResult(interp, (char *)h2getMsgErrno(errnoGet()), TCL_STATIC);
      return TCL_ERROR;
   }

   Tcl_SetResult(interp, "connected", TCL_STATIC);
   return $module$Install(interp, m, 1);
}

static int
$module$ClientEndCb(ClientData clientData, Tcl_Interp *interp,
		    int objc, Tcl_Obj *const objv[])
{
   struct ModuleInfo *m = (struct ModuleInfo *)clientData;

   if (csClientEnd(m->cid) == ERROR) {
      Tcl_SetResult(interp, (char *)h2getMsgErrno(errnoGet()), TCL_STATIC);
      return TCL_ERROR;
   }

   $module$Install(interp, m, 0);
   free(m->name);
   free(m);

   Tcl_SetResult(interp, "disconnected", TCL_STATIC);
   return TCL_OK;
}


/*
 * ----------------------------------------------------------------------
 * $module$ abort request
 */

static int $module$AbortRqstSendCb(ClientData, Tcl_Interp *, int, Tcl_Obj *const []); 
static int $module$AbortReplyRcvCb(ClientData, Tcl_Interp *, int, Tcl_Obj * const []);

static int
$module$AbortRqstSendCb(ClientData data, Tcl_Interp *interp,
			int objc, Tcl_Obj *const objv[])
{
   struct ModuleInfo *m = (struct ModuleInfo *)data;
   int rqstId, bilan;
   char buf[10];
   int activity;	/* input */

   TEST_BAD_USAGE(objc != 2);
		 
   if (Tcl_GetIntFromObj(interp, objv[1], &activity) != TCL_OK)
      return TCL_ERROR;

   if (tclServRqstSend(interp, Tcl_GetStringFromObj(objv[0], NULL),
		       m->cid, $MODULE$_ABORT_RQST, 
		       &activity, sizeof(int),
		       &rqstId, &bilan) == ERROR) {

      Tcl_SetResult(interp, (char *)h2getMsgErrno(bilan), TCL_STATIC);
      return TCL_ERROR;
   }
    
   sprintf(buf, " %d", rqstId);
   Tcl_SetResult(interp, "SENT", TCL_STATIC);
   Tcl_AppendResult(interp, buf, NULL);
   return TCL_OK;
}

static int
$module$AbortReplyRcvCb(ClientData clientData, Tcl_Interp *interp,
			int objc, Tcl_Obj * const objv[])
{
   struct ModuleInfo *m = (struct ModuleInfo *)clientData;
   int ret;
   int rqstId, bilan, activity, status;
   Tcl_Obj *result;

   TEST_BAD_USAGE(
      objc != 2 ||
      Tcl_GetIntFromObj(interp, objv[1], &rqstId) != TCL_OK
   );

   status = tclServReplyRcv(interp, m->cid, rqstId, NO_BLOCK,
			    &activity, NULL, 0, &bilan);

   switch (status) {
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
	 Tcl_SetResult(interp, (char *)h2getMsgErrno(bilan), TCL_STATIC);
	 return TCL_ERROR;
   }

   result = Tcl_NewListObj(0, NULL);
   if (result == NULL) return TCL_ERROR;

   ret = Tcl_ListObjAppendElement(interp, result,
				  Tcl_NewStringObj("TERM", -1));
   if (ret != TCL_OK) return TCL_ERROR;

   if (bilan == OK)
      ret = Tcl_ListObjAppendElement(interp, result,
				     Tcl_NewStringObj("OK", -1));
   else
      ret = Tcl_ListObjAppendElement(interp, result,
				     Tcl_NewStringObj(
					(char *)h2getMsgErrno(bilan), -1));
   if (ret != TCL_OK) return TCL_ERROR;

   Tcl_SetObjResult(interp, result);
   return TCL_OK;
}
