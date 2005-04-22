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
 * $module$ installation
 */

int
$Module$_Init(Tcl_Interp *interp)
{
   char buf[20];
   int code;

   code = Tcl_PkgProvide(interp, "$Module$", "1.0");
   if (code != TCL_OK) return code;

   Tcl_CreateObjCommand(interp, "::$module$::connect",
			$module$ClientInitCb, NULL, NULL);

   sprintf(buf, "%d", (int)$MODULE$_CLIENT_MBOX_REPLY_SIZE);
   Tcl_SetVar(interp, "::$module$::mboxSize", buf, TCL_GLOBAL_ONLY);

   Tcl_SetVar(interp, "::$module$::datadir", DATADIR, TCL_GLOBAL_ONLY);

   return TCL_OK;
}

static int
$module$Install(Tcl_Interp *interp, struct ModuleInfo *m, int install)
{
   int offset;
   char name[$maxCmdLen$];

   strcpy(name, "::");
   strcat(name, m->name);
   strcat(name, "::");
   offset = strlen(name);

   if (install) {
      strcpy(name+offset, "disconnect");
      Tcl_CreateObjCommand(interp, name, $module$ClientEndCb, m, NULL);

      strcpy(name+offset, "AbortSend");
      Tcl_CreateObjCommand(interp, name, $module$AbortRqstSendCb, m, NULL);
      strcpy(name+offset, "AbortRcv");
      Tcl_CreateObjCommand(interp, name, $module$AbortReplyRcvCb, m, NULL);
$createCommands$
   } else {
      strcpy(name+offset, "disconnect");
      Tcl_DeleteCommand(interp, name);

      strcpy(name+offset, "AbortSend");
      Tcl_DeleteCommand(interp, name);
      strcpy(name+offset, "AbortRcv");
      Tcl_DeleteCommand(interp, name);
$deleteCommands$
   }

   return TCL_OK;
}
