/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS                  --  Thu Feb  4 1999
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
#include "config.h"
__RCSID("$LAAS$");

#ifdef VXWORKS
# include <vxWorks.h>
#else
# include "portLib.h"
# include "h2initGlob.h"

# include <rpc/types.h>
#endif /* VXWORKS */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tclServ.h"
#include "tcl_cs.h"

#ifdef VXWORKS
#include <hostLib.h>
#include <ioLib.h>
#endif

#include <taskLib.h>

static int xes_verbose = 0;

int
Tcl_AppInit(Tcl_Interp *interp)
{
#define MAX_STRLEN	50
   char portStr[MAX_STRLEN], *tmp;
   const char *tclServLibrary[2];
   Tcl_DString initFile;

#ifdef VXWORKS
   if (xes_verbose) {
      int fd;

      /* Rediriger les E/S vers un xterm de la machine hote */

      fd = xes_init(NULL); 
      if (fd < 0) {
	 fprintf(stderr, "Could not connect to xes_server\n");
	 exit(2);

      } else {
	 xes_set_title("tclServ on %s", Tcl_GetHostName());

	 /* stderr */
	 ioTaskStdSet(0, 2, fd);
	 setbuf(stderr, NULL);
      }
   }
#endif

   if (Tcl_Init(interp) == TCL_ERROR) {
      fprintf(stderr, "Could not init tcl interpreter\n");
      return TCL_ERROR;
   }

   /* Create csLib mailbox */
   if (tclServCsStart(interp) != TCL_OK) {
      fprintf(stderr, "Could not start csLib services\n");
      return TCL_ERROR;
   }

   /* Create global useful variables */
   tmp = getenv("TCLSERV_PORT");
   if (tmp) {
      strncpy(portStr, tmp, MAX_STRLEN);
   } else if (sprintf(portStr, "%d", TCLSERV_CMDPORT) >= MAX_STRLEN) {
      fprintf(stderr,
	      "Warning: string overflow in \"%s\", line %d\n",
	      __FILE__,__LINE__);
   }      

   Tcl_CreateObjCommand(interp, "exit", TclServExit, 0, NULL);

   Tcl_SetVar(interp, "verbose", xes_verbose ? "1":"0", TCL_GLOBAL_ONLY);
   Tcl_SetVar(interp, "hostname", Tcl_GetHostName(), TCL_GLOBAL_ONLY);
   Tcl_SetVar(interp, "port", portStr, TCL_GLOBAL_ONLY);
   Tcl_SetVar(interp, "tclserv_moduledir", TCLSERV_LIBDIR, TCL_GLOBAL_ONLY);

   if (xes_verbose) {
      printf("\n\ttclServ - Copyright (C) 1999-2003 LAAS-CNRS\n\n");
   }


   /* source standard tclServ libraries */
   tclServLibrary[0] = getenv("TCLSERV_LIBRARY");
   if (!tclServLibrary[0]) {
      tclServLibrary[0] = TCLSERV_DATA;
   }
   tclServLibrary[1] = "init.tcl";
   Tcl_SetVar(interp, "tclserv_library", tclServLibrary[0], TCL_GLOBAL_ONLY);
   Tcl_DStringInit(&initFile);
   if (Tcl_EvalFile(interp, 
		    Tcl_JoinPath(2, tclServLibrary, &initFile)) != TCL_OK) {
      Tcl_AppendResult(interp,
		       "\nThe directory ",
		       tclServLibrary[0],
		       " does not contain a valid ",
		       tclServLibrary[1],
		       " file.\nPlease check your installation.\n",
		       NULL);
      Tcl_DStringFree(&initFile);
      return TCL_ERROR;
   }
   Tcl_DStringFree(&initFile);

   /* A pure cheat code */
   return Tcl_Eval(interp, "vwait forever");

#undef MAX_STRLEN
}
 
static STATUS
tclServReal(char *verbose)
{
   char *argv[] = { "tcl" };

   if (verbose != NULL && 
       *verbose == '-' &&
       strncmp(verbose+1, "console", strlen(verbose+1)) == 0) {
      xes_verbose = 1;
   } else
      xes_verbose = 0;

   Tcl_Main(1, argv, Tcl_AppInit);
   return OK;
}

#ifndef VXWORKS

int 
main(int argc, char *argv[])
{
   /* daemonize */
   if (argc < 2) {
      switch(fork()) {
	 case -1:
	    perror("cannot fork");
	    exit(2);

	 case 0: break;
	 default: exit(0);
      }
   }

   h2initGlob(0);
   
   if (tclServReal((argc < 1) ? NULL : argv[1]) == ERROR) {
      return EXIT_FAILURE;
   } 
   return EXIT_SUCCESS;
}

#else

STATUS
tclServ(char *verbose)
{
   static STATUS tclServReal(char *);

   return taskSpawn("tTclServ", TCLSERV_PRIORITY, 8, 50000,
		    tclServReal, verbose);
}

#endif /* VXWORKS */

/*
 * Wish you were here ---------------------------------------------------
 */

int
TclServExit(ClientData dummy, Tcl_Interp *interp,
	    int objc, Tcl_Obj *const objv[])
{
   int value;

   if ((objc != 1) && (objc != 2)) {
      Tcl_WrongNumArgs(interp, 1, objv, "?returnCode?");
      return TCL_ERROR;
   }
    
   if (objc == 1) {
      value = 0;
   } else if (Tcl_GetIntFromObj(interp, objv[1], &value) != TCL_OK) {
      return TCL_ERROR;
   }

   tclServCsStop(interp);

#ifdef VXWORKS
   if (xes_verbose)
      printf("\nType C-d to remove this window");
#endif

   Tcl_Exit(value);
   /*NOTREACHED*/
   return TCL_OK;			/* Better not ever reach this! */
}
