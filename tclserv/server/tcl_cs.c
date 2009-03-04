
/* --- FILE GENERATED WITH GENOM, DO NOT EDIT BY HAND ------------------ */

/* 
 * Copyright (c) 1999-2005 LAAS/CNRS
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
#include "genom-config.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>		/* nanosleep */
#include <sys/socket.h>
#include <netinet/in.h>

#include <tcl.h>

#include <portLib.h>
#include <errnoLib.h>
#include <h2devLib.h>

#include <taskLib.h>
#include "h2semLib.h"

#include "tcl_cs.h"
#include "tclServ.h"

#ifdef wanna_get_lots_of_horrid_dependencies
#include "modules.h"	/* GENOM_END_MODULE && GENOM_RESUME_EXEC_TASK */

#else /* be sure to keep this in sync with genom */

/* Numéro d'abort pour arrêter le module */
#   define GENOM_END_MODULE -99
/* Numéro d'abort pour débloquer les tâches d'exec suspendues */
#   define GENOM_RESUME_EXEC_TASK -88
/* Numéro d'abort pour afficher les temps d'exec */
#   define GENOM_SHOW_TIME -77
/* Numéro d'abort pour rendre le module verbose */
#   define GENOM_VERBOSE -66

#endif /* wanna_get_lots_of_horrid_dependencies */

/* Tableau de semaphores de synchronisation des taches */
extern H2SEM_ID tabSemTask[];

static int csInit = 0;

static char *csMboxEventScript = NULL;
static int csWakeUpTaskToBeDeleted = 0;
static int csWakeUpTaskSuspended = 0;

#define CSWAKEUPTASK_DESTROY	0
#define CSWAKEUPTASK_SPAWN	1
#define CSWAKEUPTASK_SUSPEND	2
#define CSWAKEUPTASK_RESUME	3

static int tclServCsWakeUpTaskInstall(int);

static int csMboxInitCb(ClientData, Tcl_Interp *, int, Tcl_Obj* const []);
static int csMboxEndCb(ClientData, Tcl_Interp *, int, Tcl_Obj *const []);
static int csMboxWaitCb(ClientData, Tcl_Interp *, int, Tcl_Obj *const []);
static int csMboxEventCb(ClientData, Tcl_Interp *, int, Tcl_Obj *const []);

static STATUS tclServCsWakeUpTask(int port, int timeout, int parentId);


/*
 * Start and stop communication with csLib ------------------------------
 */

int 
tclServCsStart(Tcl_Interp *interp)
{
   char buf[20];

   Tcl_CreateObjCommand(interp, "cs::mboxInit", csMboxInitCb, NULL, NULL);
   Tcl_CreateObjCommand(interp, "cs::mboxEnd", csMboxEndCb, NULL, NULL);
   Tcl_CreateObjCommand(interp, "cs::mboxWait", csMboxWaitCb, NULL, NULL);
   Tcl_CreateObjCommand(interp, "cs::mboxEvent", csMboxEventCb, NULL, NULL);

   sprintf(buf, "%d", GENOM_END_MODULE);
   Tcl_SetVar2(interp, "::cs::genom", "abort", buf, TCL_GLOBAL_ONLY);
   sprintf(buf, "%d", GENOM_SHOW_TIME);
   Tcl_SetVar2(interp, "::cs::genom", "showtime", buf, TCL_GLOBAL_ONLY);
   sprintf(buf, "%d", GENOM_VERBOSE);
   Tcl_SetVar2(interp, "::cs::genom", "verbose", buf, TCL_GLOBAL_ONLY);
   sprintf(buf, "%d", GENOM_RESUME_EXEC_TASK);
   Tcl_SetVar2(interp, "::cs::genom", "resume", buf, TCL_GLOBAL_ONLY);

   return TCL_OK;
}

int
tclServCsStop(Tcl_Interp *interp)
{
   if (csInit) Tcl_Eval(interp, "puts [cs::mboxEnd]");
   return TCL_OK;
}

/*
 * Send a request -------------------------------------------------------
 */

STATUS
tclServRqstSend(Tcl_Interp *interp, char *name,
		CLIENT_ID clientId, int rqstType, 
		void *inputData, int inputSize,
		int *rqstId, int *bilan)
{
   errnoSet(0);

   if (csClientRqstSend(clientId, rqstType, inputData, inputSize,
			(FUNCPTR) NULL, TRUE, 0, 0, rqstId) == ERROR) {
      *bilan = errnoGet();
      return ERROR;
   }

   return OK;
}


/*
 * Recieve a reply ------------------------------------------------------
 */

STATUS 
tclServReplyRcv(Tcl_Interp *interp,
		CLIENT_ID clientId, int rqstId, int block, 
		int *activity, void *outputData, int outputSize,
		int *bilan)
{
   int status;
   errnoSet(0);

   status = csClientReplyRcv(clientId, rqstId, block, 
                             (void *)activity, sizeof(int), (FUNCPTR)NULL, 
                             outputData, outputSize, (FUNCPTR)NULL);

   if (status == ERROR) {
      *bilan = errnoGet();
      if (H2_MODULE_ERR_FLAG(*bilan)) status = FINAL_REPLY_OK;
   }
   else *bilan = OK;
   
   return status;
}


/*
 * CsWakeUpTask ---------------------------------------------------------
 */

static int
tclServCsWakeUpTaskInstall(int action)
{
   static int csWakeUpTaskId = 0;

   if (action==CSWAKEUPTASK_SPAWN && csWakeUpTaskId == 0) {

      csWakeUpTaskId = taskSpawn("tTclServCsWakeUpTask",
				 TCLSERV_PRIORITY,
				 VX_FP_TASK, 4000,
				 (FUNCPTR)tclServCsWakeUpTask,
				 TCLSERV_CMDPORT, 0, taskIdSelf());
      if (csWakeUpTaskId == ERROR) {
	 perror("taskSpawn");
	 csWakeUpTaskId = 0;
	 return TCL_ERROR;
      }
      csWakeUpTaskToBeDeleted = 0;
      csWakeUpTaskSuspended = 0;
      return TCL_OK;

   } else if (action==CSWAKEUPTASK_DESTROY && csWakeUpTaskId != 0) {
      csWakeUpTaskSuspended = 0;
      csWakeUpTaskToBeDeleted = 1;

      /* wake up the daemon, that will exit */
      h2semGive(H2DEV_TASK_SEM_ID(taskGetUserData(taskIdSelf())));

      csWakeUpTaskId = 0;
      return TCL_OK;

   } else if (action==CSWAKEUPTASK_SUSPEND) {
      csWakeUpTaskSuspended = 1;
      return TCL_OK;

   } else if (action==CSWAKEUPTASK_RESUME) {
      csWakeUpTaskSuspended = 0;
      return TCL_OK;
   }

   return TCL_ERROR;
}


/*
 * Callbacks ------------------------------------------------------------
 */

static int 
csMboxInitCb(ClientData clientData, Tcl_Interp *interp, 
	     int argc, Tcl_Obj *const argv[])
{
   static char *syntax = "<mboxName> <rcvSize> <replySize>";
   int rcvSize, replySize;
   char strerr[64];
    
   if (csInit) {
      Tcl_SetResult(interp, "csMbox already initialized", TCL_STATIC);
      return TCL_ERROR;
   }
   
   if (argc != 4) {
      Tcl_WrongNumArgs(interp, 1, argv, syntax);
      return(TCL_ERROR);
   }
   if (Tcl_GetIntFromObj(interp, argv[2], &rcvSize) != TCL_OK) {
      Tcl_WrongNumArgs(interp, 1, argv, syntax);
      return(TCL_ERROR);
   }
   if (Tcl_GetIntFromObj(interp, argv[3], &replySize) != TCL_OK) {
      Tcl_WrongNumArgs(interp, 1, argv, syntax);
      return(TCL_ERROR);
   }

   if (csMboxInit(Tcl_GetStringFromObj(argv[1], NULL), 
		  rcvSize, replySize) != OK) {
      Tcl_SetObjResult(interp, Tcl_NewStringObj(
	 h2getErrMsg(errnoGet(), strerr, 64), -1));
      return TCL_ERROR;
   }

   if (csMboxEventScript != NULL)
      tclServCsWakeUpTaskInstall(CSWAKEUPTASK_SPAWN);

   csInit = 1;
   Tcl_SetResult(interp, "csMbox created", TCL_STATIC);
   return TCL_OK;
}


static int 
csMboxEndCb(ClientData clientData, Tcl_Interp *interp,
	    int argc, Tcl_Obj *const argv[])
{
   static char *syntax = "";
   char strerr[64];

   if (!csInit) {
      Tcl_SetResult(interp, "csMbox not initialized", TCL_STATIC);
      return TCL_ERROR;
   }

   if (argc != 1) {
      Tcl_WrongNumArgs(interp, 1, argv, syntax);
      return TCL_ERROR;
   }

   tclServCsWakeUpTaskInstall(CSWAKEUPTASK_DESTROY);

   if (csMboxEnd() != OK) {
      Tcl_SetObjResult(interp, Tcl_NewStringObj(
	 h2getErrMsg(errnoGet(), strerr, 64), -1));
      return(TCL_ERROR);
   }

   csInit = 0;
   Tcl_SetResult(interp, "mbox removed", TCL_STATIC);
   return TCL_OK;
}


static int 
csMboxWaitCb(ClientData clientData, Tcl_Interp *interp,
	     int argc, Tcl_Obj *const argv[])
{
   static char *syntax = "<timeOut>";
   int timeout, mask;
   char strerr[64];
    
   if (!csInit) {
      Tcl_SetResult(interp, "csMbox not initialized", TCL_STATIC);
      return TCL_ERROR;
   }

   if (argc != 2) {
      Tcl_WrongNumArgs(interp, 1, argv, syntax);
      return TCL_ERROR;
   }
   if (Tcl_GetIntFromObj(interp, argv[1], &timeout) != TCL_OK) {
      Tcl_WrongNumArgs(interp, 1, argv, syntax);
      return TCL_ERROR;
   }

   if ((mask = csMboxWait(timeout, REPLY_MBOX)) == ERROR) {
      Tcl_SetObjResult(interp, Tcl_NewStringObj(
	 h2getErrMsg(errnoGet(), strerr, 64), -1));
      return TCL_ERROR;
   }

   Tcl_SetObjResult(interp, Tcl_NewIntObj(mask==REPLY_MBOX));
   return TCL_OK;
}

static int
csMboxEventCb(ClientData clientData, Tcl_Interp *interp, 
	      int objc, Tcl_Obj *const objv[])
{
   int len;
   char *script;
   static char *syntax = "?script?";

   if (objc != 1 && objc != 2) {
      Tcl_WrongNumArgs(interp, 1, objv, syntax);
      return TCL_ERROR;
   }

   if (objc == 1) {
      if (csMboxEventScript == NULL)
	 Tcl_SetResult(interp, "", TCL_STATIC);
      else
	 Tcl_SetResult(interp, csMboxEventScript, TCL_VOLATILE);
      return TCL_OK;
   }

   script = Tcl_GetStringFromObj(objv[1], &len);
   if (csMboxEventScript != NULL) ckfree(csMboxEventScript);

   if (len == 0) {
      csMboxEventScript = NULL;
      if (csInit) tclServCsWakeUpTaskInstall(CSWAKEUPTASK_SUSPEND);
      return TCL_OK;
   }

   csMboxEventScript = ckalloc(len + 1);
   strcpy(csMboxEventScript, script);

   if (csInit) tclServCsWakeUpTaskInstall(CSWAKEUPTASK_RESUME);
   return TCL_OK;
}


/*
 * ======================================================================
 * Daemon monitoring mbox
 */

static STATUS
tclServCsWakeUpTask(int port, int timeout, int parentId)
{
   char buf[10], *tmp;
   int noblock = FALSE;
   int sock;
   struct sockaddr_in tcl;
   H2SEM_ID semId = H2DEV_TASK_SEM_ID(taskGetUserData(parentId));
   struct timespec wait, waited;

   /* port not redefined with option -p, then test env(TCLSERV_PORT) */
   if (port == TCLSERV_CMDPORT) {
     tmp = getenv("TCLSERV_PORT");
     if (tmp) { port = atoi(tmp); }
     fprintf(stdout, "(re)starting tclServ cs daemon on port %d\n", port);
   }

   sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock == ERROR) { perror("socket"); return ERROR; }

   memset(&tcl, 0, sizeof(struct sockaddr_in));
   tcl.sin_family = AF_INET;
   tcl.sin_port = htons((unsigned short)(port & 0xFFFF));
   tcl.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */

   if (connect(sock, (struct sockaddr *)&tcl,
	       sizeof(struct sockaddr)) == ERROR) {
      perror("connect");
      return ERROR;
   }

   if ((noblock = fcntl(sock, F_GETFL)) < 0) {
       perror("fcntl(F_GETFL)");
       return ERROR;
   }
   noblock &= ~O_NONBLOCK;
   if (fcntl(sock, F_SETFL, noblock) < 0) {
       perror("fcntl(F_SETFL)");
       return ERROR;
   }

   write(sock, "HELLO\n", 6);
   if (read(sock, buf, 5) != 5 || strncmp(buf, "HELLO", 5) != 0)
      perror("read");
   while (read(sock, buf, 1) == 1)
      if (buf[0] == '\r' && read(sock, buf, 1) == 1 && buf[0]=='\n')
	 break;

   for(;!csWakeUpTaskToBeDeleted;)
      switch (h2semTake(semId, timeout)) {
	 case TRUE:
	    while (csWakeUpTaskSuspended) {
	       wait.tv_sec = 0;
	       wait.tv_nsec = 5e7; /* 50ms */
	       nanosleep(&wait, &waited);
	    }

	    if (!csWakeUpTaskToBeDeleted) {
	       write(sock, "CSREPLY\n", 8);
	       if (read(sock, buf, 4) != 4 || 
		   strncmp(buf, "OK", 2) != 0) perror("read");
	    }
	    break;

	 case FALSE:
	    /* timeout */
	    break;

	 case ERROR:
	    perror("h2SemTake");
	    csWakeUpTaskToBeDeleted = 1;
	    break;
      }

   write(sock, "BYE\n", 4);
   close(sock);

   csWakeUpTaskToBeDeleted = 0;
   fputs("leaving tclServ daemon\n", stdout);
   return OK;
}
