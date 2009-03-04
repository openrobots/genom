
/* --- FILE GENERATED WITH GENOM, DO NOT EDIT BY HAND ------------------ */

/* 
 * Copyright (c) 2004-2005
 *      Autonomous Systems Lab, Swiss Federal Institute of Technology.
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

/* module $module$ initialization routines */

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/init.h>
# include <linux/module.h>
# include <linux/kernel.h>
# include <linux/sched.h>

#else
# include <stdio.h>
#  include <sys/param.h>
#  include <stdlib.h>
#  include <fcntl.h>
#  include <string.h>
#  include <errno.h>
#  include <unistd.h>
#  include <sys/utsname.h>

#  define PID_FILE	".$module$.pid"
#endif /* RTAI && KERNEL */

#include <portLib.h>
#include <shellLib.h>
#include <h2initGlob.h>

#include <taskLib.h>
#include <errnoLib.h>
#include <h2evnLib.h>

#include "$module$Header.h"
#include "commonStructLib.h"
#include "$module$MsgLib.h"

#define CNTRL_TASK_MIN_STACK_SIZE 4096
#define EXEC_TASK_MIN_STACK_SIZE  4096


/* --- Global variables ---------------------------------------------- */

/* Initialization semaphores */
SEM_ID sem$module$CntrlTaskInit;
SEM_ID sem$module$InitExecTab[$MODULE$_NB_EXEC_TASK];

/* Les taches */
void $module$CntrlTask ();
$execTaskTabDescribe$

/* Internal data structures */
$internalDataType$ *$module$DataStrId = NULL;
$MODULE$_CNTRL_STR *$module$CntrlStrId = NULL;

#ifdef PID_FILE
/* name of the file containing module's PID */
char pidFilePath[MAXPATHLEN];
#endif

/****************************************************************************
*
*  $module$TaskInit  -  Initialisation de l'module de $module$
*
*  Description:
*  Routine d'initialisation du module $module$, a etre appelee au moment
*  du boot.
*
*  Retourne: OK ou ERROR
*/

STATUS
$module$TaskInit()
{
  int i;
#ifdef PID_FILE
  FILE* pidFile;
  int pidFileFd;
  char *pidDir;
  struct utsname uts;
#endif /* PID_FILE */
  char strerr[64];

  /*
   * Create internal data structures
   */

  /* SDI_f */
  if (commonStructCreate (sizeof ($internalDataType$), 
			  (void *) &$module$DataStrId) 
      != OK)  {
    h2perror ("$module$TaskInit: cannot create the Functional Data Base");
    goto error;
  }
  
  /* SDI_c */
  if (commonStructCreate (sizeof ($MODULE$_CNTRL_STR), 
			  (void *) &$module$CntrlStrId) 
      != OK)  {
    h2perror ("$module$TaskInit: cannot create the Control Data Base");
    goto error;
  }

#ifdef PID_FILE
  /*
   * Create pid file
   */
  pidDir = getenv("H2DEV_DIR");
  if (pidDir == NULL) {
      pidDir = getenv("HOME");
  }
  if (pidDir == NULL) {
      pidDir = "/tmp";
  }
  if (uname(&uts) == -1) {
      errnoSet(errno);
      goto error;
  }
  snprintf(pidFilePath, MAXPATHLEN, "%s/%s-%s", pidDir, PID_FILE, uts.nodename);
  pidFileFd = open(pidFilePath, O_CREAT|O_EXCL|O_WRONLY, 0644);
  if (pidFileFd < 0) {
      fprintf(stderr, "$module$: error creating %s: %s\n",
	      pidFilePath, strerror(errno));
      goto error;
  }
  pidFile = fdopen(pidFileFd, "w");
  if (pidFile == NULL) {
      perror("$module$: error opening pid file");
      goto error;
  }
  fprintf(pidFile, "%d ", getpid());
  fputc('\n', pidFile);
  fclose(pidFile);
#endif /* PID_FILE */

  /* 
   * Spawn control task
   */
  logMsg("$module$: spawning control task.\n");

  sem$module$CntrlTaskInit = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
  if (sem$module$CntrlTaskInit == NULL) {
     h2perror("$module$TaskInit: cannot create control semaphore");
     goto error; 
  }
  
  if (taskSpawn("$module$CntrlTask", 10 /* priorite */, VX_FP_TASK,
		$MODULE$_MAX_RQST_SIZE + CNTRL_TASK_MIN_STACK_SIZE /*size*/, 
		(FUNCPTR)$module$CntrlTask) == ERROR) {
     h2perror("$module$TaskInit: cannot spawn $module$CntrlTask");
     goto error;
  }
  /* wait for initialization */
  if (semTake(sem$module$CntrlTaskInit, WAIT_FOREVER) != OK)
     goto error;
  
  semDelete(sem$module$CntrlTaskInit);
  sem$module$CntrlTaskInit = NULL;

  /* check task status */
  if (CNTRL_TASK_STATUS == ERROR) {
     errnoSet(CNTRL_TASK_BILAN);
     h2perror("$module$: Control task failed");
     goto error;
  }

  /* 
   * Spawn execution tasks
   */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) {
    logMsg("$module$: spawning task %s.\n", $module$ExecTaskTab[i].name);

    sem$module$InitExecTab[i] = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
    if (sem$module$InitExecTab[i] == NULL) {
       h2perror("$module$TaskInit: cannot create init semaphore");
       goto error; 
    }
    
    if (taskSpawn($module$ExecTaskTab[i].name, 
		  $module$ExecTaskTab[i].priority, VX_FP_TASK, 
		  $module$ExecTaskTab[i].size + EXEC_TASK_MIN_STACK_SIZE, 
		  (FUNCPTR)$module$ExecTaskTab[i].func) == ERROR) {
       h2perror("$module$TaskInit: cannot spawn exec task");
       goto error;
    }
    /* wait for initialization */
    if (semTake(sem$module$InitExecTab[i], WAIT_FOREVER) != OK) {
       h2perror("$module$TaskInit: semTake init sem");
       goto error;
    }
    
    semDelete(sem$module$InitExecTab[i]);
    sem$module$InitExecTab[i] = NULL;

    /* check task status */
    if (EXEC_TASK_STATUS(i) == ERROR) {
       logMsg("$module$TaskInit: Exec task %s failed : %s\n", 
	      $module$ExecTaskTab[i].name, h2getErrMsg(EXEC_TASK_BILAN(i), strerr, 64));
       errnoSet(EXEC_TASK_BILAN(i));
       goto error;
    }
  }

  /* wake up control task once for poster initialization */
  h2evnSignal(CNTRL_TASK_ID);

  /*
   * Module is ready
   */
  logMsg("$module$: all tasks are spawned\n");

  return OK;

  error:
  /* Destroy initialization semaphores */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) if (sem$module$InitExecTab[i]) {
     semDelete(sem$module$InitExecTab[i]);
     sem$module$InitExecTab[i] = NULL;
  }

  if (sem$module$CntrlTaskInit) {
     semDelete(sem$module$CntrlTaskInit);
     sem$module$CntrlTaskInit = NULL;
  }

  /* Destroy internal data structures */
  if ($module$CntrlStrId) {
     commonStructDelete($module$CntrlStrId);
     $module$CntrlStrId = NULL;
  }

  if ($module$DataStrId) {
     commonStructDelete($module$DataStrId);
     $module$DataStrId = NULL;
  }
  return ERROR;
}


#if defined(__RTAI__) && defined(__KERNEL__)

/*
 * --- Module initialization for RTAI -----------------------------------
 *
 */

MODULE_DESCRIPTION("$module$ module");

int
$module$_module_init(void)
{
   if ($module$TaskInit() == ERROR) {
      /* we don't report an error to insmod: there might still be
       * suspended tasks lying around and the module's code must remain
       * in memory. */
      h2perror("Error moduleInit");
   }

   return 0;
}

void
$module$_module_exit(void)
{
   ;
}

module_init($module$_module_init);
module_exit($module$_module_exit);

#endif /* RTAI && KERNEL */


#if !defined(__KERNEL__)
/*
 * --- main for unix ----------------------------------------------------
 */
static char *prompt = "$module$> ";
extern char *optarg;
extern int optind;

static void 
usage(void)
{
    fprintf(stderr, "usage: $module$ [-b|-e|-s] [-f freq]\n");
    exit(3);
}

int
main(int argc, char *argv[])
{
    int c;
    /* Options par defaut */
    int backgroundOpt = 0, essaiOpt = 0, shellOpt = 0;
    int frequence = 100;		/* frequence par defaut */
    int errFlag = 0;
    int waitPipe[2];
    char buf[80];

    /* Parse les options */
    while ((c = getopt(argc, argv, "besf:")) != -1) {
	switch (c) {
	  case 'b':
	    backgroundOpt++;
	    break;
	  case 'e':
	    if (shellOpt) {
		errFlag++;
	    } else {
		essaiOpt++;
	    }
	  case 's':
	    if (essaiOpt) {
		errFlag++;
	    } else {
		shellOpt++;
	    }
	    break;
	  case 'f':
	    frequence = atoi(optarg);
	    break;
	  case '?':
	    errFlag++;
	    break;
	} /* switch */
    } /* while */
    if (errFlag) {
	/* Erreur */
	usage();
    }

    if (backgroundOpt) {
	if (shellOpt || essaiOpt) {
	    /* Erreur */
	    usage();
	}
	/* Creation d'un pipe pour envoyer un message a la fin de l'init */
	pipe(waitPipe);
	
	/* Lancement en background */
	if (fork() != 0) {
	    /* close(waitPipe[1]); */
	    /* Attend le lancement du module par une lecture sur le pipe */
	    if (read(waitPipe[0], buf, sizeof(buf)) <= 0) {
		fprintf(stderr, 
			"$module$ moduleInit: error waiting for module start\n");
	    } 
	    exit(0);
	}
	/* close(waitPipe[0]); */
    }

    /* Initialise environnement comLib */
#if $periodic$
    errFlag = h2initGlob(100);
#else
    errFlag = h2initGlob(0);
#endif
    if (errFlag == ERROR) {
	/* informe la tache en avant plan */
	write(waitPipe[1], "$module$: h2initGlob error\n", 17); 
	exit(2);
    }
    /* Demarrage des taches du module */
    if ($module$TaskInit() == ERROR) {
	printf("$module$: Error moduleInit: ");
	h2printErrno(errnoGet());
	/* informe la tache en avant plan */
	write(waitPipe[1], "$module$: moduleTaskInit error\n", 21); 
	exit(1);
    }
    if (shellOpt) {
	/* Lance un shell */
	shellMainLoop(stdin, stdout, stderr, prompt);
    } else if (essaiOpt) {
	fprintf(stderr, "$module$: not implemented\n");
    } 
    /* informe la tache en avant plan */
    if (backgroundOpt) {
	write(waitPipe[1], "$module$: OK\n", 3);
	/* close(waitPipe[1]); */
    }
    /* Wait forever */
    while (1) {
	select(64, NULL, NULL, NULL, NULL);
    }
    unlink(pidFilePath);
    exit(0);
}
#endif /* !__KERNEL__ */
