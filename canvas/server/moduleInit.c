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

/*------------------  Fichier généré automatiquement ------------------*/
/*------------------  Ne pas éditer manuellement !!! ------------------*/
 
/****************************************************************************/
/*   LABORATOIRE D'AUTOMATIQUE ET D'ANALYSE DE SYSTEMES - LAAS / CNRS       */
/*   PROJET HILARE II - INITIALISATION DU MODULE $MODULE$                   */
/*   FICHIER SOURCE : "$module$TaskInit.c"                                  */
/****************************************************************************/

/* VERSION ACTUELLE / HISTORIQUE DES MODIFICATIONS :
   version ; date; auteur;
*/

/* DESCRIPTION :
   Routine d'initialisation du module $module$, qui doit etre appelee
   au moment du boot.
*/

#ifdef VXWORKS
#  include <vxWorks.h>
#else
#  include <sys/param.h>
#  include <portLib.h>
#endif
#include <stdio.h>
#include <stdlib.h>

#include <taskLib.h>
#include <errnoLib.h>

#ifndef VXWORKS
#include "shellLib.h"
#include "h2initGlob.h"
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/utsname.h>
#endif

#include "$module$Header.h"
#include "h2evnLib.h"
#include "commonStructLib.h"
#include "$module$MsgLib.h"

#ifndef VXWORKS
#define PID_FILE ".$module$.pid"
#endif

#define CNTRL_TASK_MIN_STACK_SIZE 4096
#define EXEC_TASK_MIN_STACK_SIZE  4096


/*----------------------- VARIABLES GLOBALES ------------------------------*/

/* Semaphores d'initialisation */
SEM_ID sem$module$CntrlTaskInit;
SEM_ID sem$module$InitExecTab[$MODULE$_NB_EXEC_TASK];

/* Les taches */
void $module$CntrlTask ();
$execTaskTabDescribe$

/* Declaration des SDI */
$internalDataType$ *$module$DataStrId;
$MODULE$_CNTRL_STR *$module$CntrlStrId;

#ifndef VXWORKS
/* Nom du fichier contenant le numero de processus */
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


STATUS $module$TaskInit ()

{
  int i;
#ifndef VXWORKS
  FILE* pidFile;
  int pidFileFd;
  char *home;
  struct utsname uts;
#endif

  /* Allouer la structure commune des donnees */
  if (commonStructCreate (sizeof ($internalDataType$), 
			  (void *) &$module$DataStrId) 
      != OK)  {
    h2perror ("$module$TaskInit: cannot create the Functional Data Base");
    return (ERROR);
  }
  
  /* Allouer la structure commune de controle */
  if (commonStructCreate (sizeof ($MODULE$_CNTRL_STR), 
			  (void *) &$module$CntrlStrId) 
      != OK)  {
    h2perror ("$module$TaskInit: cannot create the Control Data Base");
    return (ERROR);
  }

#ifndef VXWORKS
  /* Creation du fichier de pid */
  home = getenv("HOME");
  if (home == NULL) {
      home = "/tmp";
  }
  if (uname(&uts) == -1) {
      errnoSet(errno);
      return ERROR;
  }
  snprintf(pidFilePath, MAXPATHLEN, "%s/%s-%s", home, PID_FILE, uts.nodename);
  pidFileFd = open(pidFilePath, O_CREAT|O_EXCL|O_WRONLY, 0644);
  if (pidFileFd < 0) {
      fprintf(stderr, "$module$: error creating %s: %s\n",
	      pidFilePath, strerror(errno));
      return ERROR;
  }
  pidFile = fdopen(pidFileFd, "w");
  if (pidFile == NULL) {
      perror("$module$: error opening pid file");
      return(ERROR);
  }
  fprintf(pidFile, "%d ", getpid());
#endif /* UNIX */
  /* 
   * Lancement de la tache de controle 
   */
  printf ("\n$MODULE$ :\nSpawn control task ... ");
#ifndef VXWORKS
  fflush (stdout);
#endif
  if ((sem$module$CntrlTaskInit = semBCreate (SEM_Q_PRIORITY, SEM_EMPTY)) 
      == NULL) {
    h2perror ("$module$TaskInit: cannot create control semaphore");
    return (ERROR); 
  }
  
  if (taskSpawn ("$module$CntrlTask", 10 /* priorite */, 
		 VX_FP_TASK,
		 $MODULE$_MAX_RQST_SIZE + CNTRL_TASK_MIN_STACK_SIZE /* size */, 
		 (FUNCPTR)$module$CntrlTask) == ERROR) {
    h2perror ("$module$TaskInit: cannot spawn $module$CntrlTask");
    return (ERROR);
  }
  
  /* Attendre le semaphore de fin d'initialisation */
  if (semTake ( sem$module$CntrlTaskInit, WAIT_FOREVER ) != OK)
    return (ERROR);
  
  /* Destruction du semaphore */
  semDelete (sem$module$CntrlTaskInit);
  
  /* Verifier si la tache a bien ete lancee */
  if (CNTRL_TASK_STATUS == ERROR) {
    errnoSet(CNTRL_TASK_BILAN);
    h2perror ("$module$: Control task failed");
    return (ERROR);
  }
  printf ("OK\n");

  /* 
   * Lancement des taches d'execution 
   */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) {

    printf ("Spawn task %s ... ", $module$ExecTaskTab[i].name);
#ifndef VXWORKS
    fflush (stdout);
#endif

    /* Creation du semaphore d'init */
    if ((sem$module$InitExecTab[i] = semBCreate (SEM_Q_PRIORITY, SEM_EMPTY)) 
	== NULL) {
      h2perror ("$module$TaskInit: cannot create init semaphore");
      return (ERROR); 
    }
    
    /* Lancer la tache d'execution */
    if (taskSpawn ($module$ExecTaskTab[i].name, 
		   $module$ExecTaskTab[i].priority, 
		   VX_FP_TASK, 
		   $module$ExecTaskTab[i].size + EXEC_TASK_MIN_STACK_SIZE, 
		   (FUNCPTR)$module$ExecTaskTab[i].func) == ERROR) {
      printf ("$module$TaskInit: cannot spawn task %s\n",
	      $module$ExecTaskTab[i].name);
      h2perror ("$module$TaskInit: taskSpawn");
      return (ERROR);
    }
    
    /* Attendre le semaphore de fin d'initialisation */
    if (semTake (sem$module$InitExecTab[i], WAIT_FOREVER ) != OK) {
      h2perror ("$module$TaskInit: semTake init sem");
      return (ERROR);
    }
    
    /* Destruction du semaphore */
    semDelete (sem$module$InitExecTab[i]);
    
    /* Verifier si la tache a ete bien lancee */
    if (EXEC_TASK_STATUS(i) == ERROR) {
      printf ("$module$TaskInit: Exec task %s failed : %s\n", 
	      $module$ExecTaskTab[i].name, 
	      h2getMsgErrno(EXEC_TASK_BILAN(i)));
      errnoSet(EXEC_TASK_BILAN(i));
      return (ERROR);
    }
    printf ("OK\n");
  }

  /* Faire faire 1 boucle a la tache de controle (init poster) */
  h2evnSignal(CNTRL_TASK_ID);

  /**** Toutes les taches sont lancees  ****/
  printf ("$module$: All tasks are spawned\n\n");

#ifndef VXWORKS
  fputc('\n', pidFile);
  fclose(pidFile);
#endif /* UNIX */

  return (OK);
}

/***********************************************************************
 *
 * main pour UNIX
 *
 */
#ifndef VXWORKS
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
    int n;

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
			"moduleInit: error waiting for module start\n");
	    } 
	    exit(0);
	}
	/* close(waitPipe[0]); */
    }

    /* Initialise environnement comLib */
    if (h2initGlob(100) == ERROR) {
	/* informe la tache en avant plan */
	write(waitPipe[1], "h2initGlob error\n", 17); 
	exit(2);
    }
    /* Demarrage des taches du module */
    if ($module$TaskInit() == ERROR) {
	printf("\nError moduleInit: ");
	h2printErrno(errnoGet());
	/* informe la tache en avant plan */
	write(waitPipe[1], "moduleTaskInit error\n", 21); 
	exit(1);
    }
    if (shellOpt) {
	/* Lance un shell */
	shellMainLoop(stdin, stdout, stderr, prompt);
    } else if (essaiOpt) {
	fprintf(stderr, "pas encore implementé\n");
    } 
    /* informe la tache en avant plan */
    if (backgroundOpt) {
	write(waitPipe[1], "OK\n", 3);
	/* close(waitPipe[1]); */
    }
    /* Wait forever */
    while (1) {
	select(64, NULL, NULL, NULL, NULL);
    }
    unlink(pidFilePath);
    exit(0);
}
#endif /* UNIX */
