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
 
/****************************************************************************
 *   LABORATOIRE D'AUTOMATIQUE ET D'ANALYSE DE SYSTEMES - LAAS / CNRS       
 *   PROJET HILARE II - TACHE DE CONTROLE DU MODULE  $MODULE$               
 *   FICHIER SOURCE: $module$CntrlTask.c                                    
 ****************************************************************************/

/* VERSION ACTUELLE / HISTORIQUE DES MODIFICATIONS :
   version ; auteur ; date;
*/

/* DESCRIPTION :
   Fichier source de la tache de controle du module.
*/


/*------------------------- INCLUSIONS -------------------------------------*/
#include "$module$Header.h"
#include "$module$MsgLib.h"

#include "commonStructLib.h"
#include "h2evnLib.h"
#include "h2timerLib.h"

#include <taskLib.h> 
#include <errnoLib.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef VXWORKS
#include <unistd.h>
#endif

#ifdef MODULE_EVENT
#include "moduleEvents.h"
#endif

/*-------------------- VARIABLES IMPORTEES ---------------------------------*/

/* Semaphores d'initialisation et de debug */
extern SEM_ID sem$module$CntrlTaskInit;   

#ifndef VXWORKS
extern char pidFilePath[];
#endif
/*-------------------- VARIABLES GLOBALES ---------------------------------*/

/* Nombre d'activites ZOMBIE */
/*XXXXX Voila une variable qu'il serait judicieux de mettre de la SDI de controle a l'occasion XXXXX*/
static int $module$NbZombie=0;
static POSTER_ID $module$CntrlPosterId;            /* Poster de controle */
static int $module$LastActivityNum=-1;
static int $module$LastAbsolutActivityNum=-1;
$execTaskNameTabDeclare$

#define TIMEOUT_CNTRL_TASK 2000 /* 10 sec (pas encore utilise) */

#ifdef MODULE_EVENT
static MODULE_EVENT_STR moduleEventCntrl;
#endif

/*---------------- PROTOTYPES DES FONCTIONS LOCALES ------------------------*/

static void   $module$CntrlIntEvnExec   (SERV_ID ServId);
static void   $module$CntrlExtEvnExec   (SERV_ID ServId);
static void   $module$SpawnActivities   (SERV_ID servId);
static void   $module$CntrlPosterWrite  (void);
static BOOL   $module$AbortActivity     (SERV_ID servId, int activity);
static STATUS $module$CntrlInitTask     (SERV_ID *ServId);
static int    allocActivity             (int rqstId, 
					 int requestNum, 
					 int execTaskNum, 
					 int reentrantFlag,
					 SERV_ID servId);
static void   freeActivity              (int activity);
static BOOL   controlExecTaskStatus(SERV_ID servId, int rqstId);
static BOOL   controlExecTaskStatusAndInitRqst(SERV_ID servId, int rqstId);
static void   $module$SendFinalReply    (SERV_ID servId, int activity);
static void   $module$RqstAbortActivity (SERV_ID ServId, int rqstId);
static void   $module$CntrlTaskSuspend  (BOOL giveFlag);
static void   $module$ReplyAndSuspend   (SERV_ID servId, int rqstId, 
					 BOOL giveFlag);      


/*---------------------- FONCTION EXPORTEE ---------------------------------*/

/*****************************************************************************
 *
 *   $module$CntrlTask - Tache de controle du module $MODULE$
 *
 *   Description:
 *   Cette tache attend suspendue l'arrivee de messages dans sa boite de
 *   reception ou d'evenements internes. A la reception d'une lettre,
 *   la requete correspondante est traitee. Les changement d'etat internes 
 *   sont pris en compte et les repliques finales sont emises au client.
 *   Cette tache boucle a jamais.
 */


void $module$CntrlTask ()

{
  static SERV_ID $module$ServId;              /* Id du serveur */

  /* Routine d'initialisation */
  CNTRL_TASK_STATUS = $module$CntrlInitTask (&$module$ServId);
  CNTRL_TASK_BILAN = errnoGet();
  
  /* Donner le sem de fin d'initialisation */
  semGive (sem$module$CntrlTaskInit) ;

  /* Se suspendre en cas de probleme */
  if (CNTRL_TASK_STATUS == ERROR)
    $module$CntrlTaskSuspend (FALSE);

#ifdef MODULE_EVENT
  moduleEventCntrl.moduleNum = $numModule$;
#endif

  /* Boucler indefiniment */
  FOREVER
    {
      /* Bloquer dans l'attente d'evenements internes et externes */
      if (h2evnSusp (0) != TRUE)
	 $module$CntrlTaskSuspend (FALSE);

      /* Prendre l'acces aux SDI */
      commonStructTake ((void *) $module$CntrlStrId);
      commonStructTake ((void *) $module$DataStrId);
 
      /* Traiter les evenements internes */
      $module$CntrlIntEvnExec ($module$ServId);

      /* Traiter les evenements externes */
      $module$CntrlExtEvnExec ($module$ServId);

      /* Lancer les activites en attentes */
      $module$SpawnActivities($module$ServId);

      /* Mettre a jour le poster de controle */
      $module$CntrlPosterWrite();

      CNTRL_TASK_BILAN = errnoGet();

     /* Liberer l'acces aux SDI */
      commonStructGive ((void *) $module$DataStrId);
      commonStructGive ((void *) $module$CntrlStrId);

    }
}



/*-------------------- ROUTINES LOCALES ------------------------------------*/


 
/*****************************************************************************
 *
 *  $module$CntrlIntEvnExec  -  Traiter les evenements internes produits
 *                              par les taches d'execution
 */

static void $module$CntrlIntEvnExec(SERV_ID servId)
{
  int activity;

  /* Activites terminees ou activite a reveiller */
  for (activity = 0;  activity < MAX_ACTIVITIES; activity++) {

    /* Activite terminee */
    if (ACTIVITY_EVN(activity) == ETHER) {
      
      /* Est-ce l'activité d'init ? */
      if (INIT_RQST != -1)
	if (ACTIVITY_RQST_TYPE(activity) == INIT_RQST)
	  if (ACTIVITY_BILAN(activity) == OK)
	    INIT_RQST = -1;

      /* On envoie la replique finale */
      $module$SendFinalReply (servId, activity);
      
      /* On libere l'activite */
      freeActivity (activity);
    }
    
    /* Activite suspendue */
    else if (ACTIVITY_EVN(activity) == ZOMBIE) {
      
      /* On envoie la replique finale */
      $module$SendFinalReply (servId, activity);

      /* On bloque l'activite */
      $module$NbZombie++;
      ACTIVITY_STATUS(activity) = ZOMBIE;
      ACTIVITY_EVN(activity) = NO_EVENT;
    }

    /* Activite en attente */
    else if (ACTIVITY_EVN(activity) == SLEEP) {
      ACTIVITY_STATUS(activity) = SLEEP;
      ACTIVITY_EVN(activity) = NO_EVENT;
    }

    /* XXX Eveil systematique - le traitement des requetes est fait apres -> si l'une d'elle met le flag EXEC_TASK_WAKE_UP_FLAG on ne le voie pas ici */
/*    else if (ACTIVITY_STATUS(activity) == SLEEP) {*/
/*      && EXEC_TASK_WAKE_UP_FLAG(ACTIVITY_TASK_NUM(activity))) {*/
/*      ACTIVITY_EVN(activity) = EXEC;
      h2evnSignal(EXEC_TASK_ID(ACTIVITY_TASK_NUM(activity)));
    }*/

  } /* for */

  /* Reset demande d'eveil (pas utilise actuelement) */
/*  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++)
    EXEC_TASK_WAKE_UP_FLAG(i) = FALSE;*/
}


/*****************************************************************************
 *
 *  $module$CntrlExtEvnExec  -  Traiter les evenements externes: arrivee
 *                              de requetes
 */

static void $module$CntrlExtEvnExec (SERV_ID $module$ServId)
     
{
  int mboxStatus;                    /* Etat de la boite aux lettres */
  int status;
  int i;
  BOOL wakeUp=FALSE;
  
  /* Obtenir l'etat de la boite aux lettres de reception de requetes */
  status = (mboxStatus = csMboxStatus (RCV_MBOX));
 
  /* Execution de la requete */
  if (mboxStatus & RCV_MBOX)  {
    status = csServRqstExec ($module$ServId);
    wakeUp = TRUE;
  }

  /* Suspendre la tache, en cas d'erreur */
  if (status == ERROR)
    $module$CntrlTaskSuspend (TRUE);

  /* On eveil toutes les taches d'exec (non periodique) un petit coup */
  if (wakeUp)
    for (i=0; i<$MODULE$_NB_EXEC_TASK; i++)
      h2evnSignal(EXEC_TASK_ID(i));
}

/*****************************************************************************
 *
 *  $module$SpawnActivities  -  Lancer les activitees pretes
 *
 *  Description:  Pour chaque activite en attente, regarde si les activites
 *                incompatibles ont ete arretees. En ce cas lance l'activite.
 */

static void $module$SpawnActivities (SERV_ID servId)
     
{
  int activity;
  int i, j;
  int nbIncomp;

  /* Pour chaque activite */
  for (activity = 0; activity < MAX_ACTIVITIES; activity++) 

    /* Activite en attente */
    if (ACTIVITY_STATUS(activity) == INIT 
	&& ACTIVITY_EVN(activity) == NO_EVENT) {
      
      /* Activite incompatibles */
      nbIncomp = ACTIVITY_NB_INCOMP(activity);
      
      /* Liste des activites encore incompatibles */
      for (i=0, j=0; i<nbIncomp; i++)
	if (ACTIVITY_STATUS(ACTIVITY_TAB_INCOMP(activity)[i]) == ETHER
	    || ACTIVITY_STATUS(ACTIVITY_TAB_INCOMP(activity)[i]) == ZOMBIE)
	  ACTIVITY_NB_INCOMP(activity)--;
	else
	  ACTIVITY_TAB_INCOMP(activity)[j++]
	    = ACTIVITY_TAB_INCOMP(activity)[i]; 
      
      /* Il n'y a plus d'activite incompatible */
      if (ACTIVITY_NB_INCOMP(activity) == 0) {
    
	/* Declencher l'execution */
	ACTIVITY_EVN(activity) = START;
	h2evnSignal(EXEC_TASK_ID(ACTIVITY_TASK_NUM(activity)));
	
	/* Envoyer la replique intermediaire */
	if (csServReplySend (servId, ACTIVITY_RQST_ID(activity), INTERMED_REPLY, 
			     OK, (void *) &ACTIVITY_ID(activity), sizeof(int), 
			     (FUNCPTR) NULL) != OK)
	  $module$CntrlTaskSuspend (TRUE);
      }      /* Activite a lancer */
    }        /* Activite en attente */
}


/*****************************************************************************
 *
 *  $module$CntrlPosterWrite  - Met a jour le poster de controle
 *
 *  Description: Se suspend en cas d'erreur
 */

static void $module$CntrlPosterWrite ()
{
  if (posterWrite ($module$CntrlPosterId, 0, (void *) $module$CntrlStrId,
		   sizeof ($MODULE$_CNTRL_STR))
      != sizeof ($MODULE$_CNTRL_STR)) {
    h2perror("$module$CntrlPosterWrite");
    $module$CntrlTaskSuspend (TRUE);
  }
}

/*-------------------- FONCTIONS AUXILIAIRES -------------------------------*/

$tabRequestFuncDeclare$


/*****************************************************************************
 *
 *  $module$CntrlInitTask - Routine d'initialisation de la tache de controle
 */


static STATUS $module$CntrlInitTask (SERV_ID *$module$ServId)

{
  int i;

  /* Creation de la boite aux lettres de reception des requetes */
  if (csMboxInit ($MODULE$_MBOX_NAME, 
		  $MODULE$_MBOX_RQST_SIZE, 0)
      != OK) {
    printf("$module$CntrlTask/csMboxInit: ");
    h2printErrno(errnoGet());
    return (ERROR);
  }
  
  /* S'initialiser comme serveur */
  if (csServInitN ($MODULE$_MAX_RQST_SIZE, 
		  $MODULE$_MAX_REPLY_SIZE >= $MODULE$_MAX_INTERMED_REPLY_SIZE ?
		  $MODULE$_MAX_REPLY_SIZE : $MODULE$_MAX_INTERMED_REPLY_SIZE, 
		  NB_RQST_TYPE,
		  $module$ServId) 
      != OK) { 
    printf("$module$CntrlTask/csServInit: ");
    h2printErrno(errnoGet());
    return (ERROR);
  }
  
  /* Installer les routines de traitement des requetes */
  for (i=0; i<NB_RQST_TYPE-1; i++) {
    if (csServFuncInstall (*$module$ServId, $module$TabRequestNum[i], 
			   (FUNCPTR) $module$TabRequestFunc[i]) != OK) {
      printf("$module$CntrlTask/csServFuncInstall: ");
      h2printErrno(errnoGet());
      return (ERROR);
    }
  }

  /* Installer la requete abort
     (c'est le parser qui attribue le premier numero libre de requete ) */
  if (csServFuncInstall (*$module$ServId, $abortRequestNum$, 
			 (FUNCPTR) $module$RqstAbortActivity) != OK) {
    printf("$module$CntrlTask/csServFuncInstall: ");
    h2printErrno(errnoGet());
    return (ERROR);
  }
  
  /* Initialiser la structure de controle */
  STOP_MODULE_FLAG = FALSE;
  for (i=0; i<MAX_ACTIVITIES; i++) {
    ACTIVITY_STATUS(i) = ETHER;
    ACTIVITY_EVN(i) = NO_EVENT;
    ACTIVITY_TASK_NUM(i) = -1;
  }
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++)
    EXEC_TASK_NB_ACTI(i) = 0;

  CNTRL_NB_EXEC_TASKS = $MODULE$_NB_EXEC_TASK;

  /* La requete d'init */
  INIT_RQST = $initRqst$;

  /* Creer le poster de controle */
  if (posterCreate ($MODULE$_CNTRL_POSTER_NAME, sizeof ($MODULE$_CNTRL_STR), 
                    &$module$CntrlPosterId) != OK) {
    printf("Error $module$CntrlTask/posterCreate: ");
    h2printErrno(errnoGet());
    return (ERROR);
  }
 
  /* Obtenir son propre identificateur de tache */
  CNTRL_TASK_ID = taskIdSelf ();
  NB_ACTIVITIES = 0;

  /* C'est OK */
  return (OK);
}

/*****************************************************************************
 *
 *  $module$AbortActivity -  Arrete une activite
 *
 *  Description : Demande l'arret de l'activite si celle ci n'est pas
 *                deja arretee. Si elle est deja arretee, libere l'activite.
 *
 *  Retourne : TRUE si activite terminee, FALSE sinon
 */
 
static BOOL $module$AbortActivity (SERV_ID servId, int activity)
{
  
  switch (ACTIVITY_STATUS(activity)) {
    
    /* Activite inexistante */
  case ETHER:
    return(TRUE);

    /* Activite plantee */
  case ZOMBIE:
    freeActivity(activity);
    return(TRUE);
    
    /* Activite pas encore demarree */
  case INIT:
    $module$SendFinalReply(servId, activity);
    freeActivity (activity);
    return(TRUE);

    /* L'acitivite roupille */
  case SLEEP:
    ACTIVITY_EVN(activity) = INTER;
    h2evnSignal(EXEC_TASK_ID(ACTIVITY_TASK_NUM(activity)));
    return(FALSE);
    
    /* Activite en cours d'exec: START, EXEC, END, FAIL, INTER , SLEEP */
  default:
    switch (ACTIVITY_EVN(activity)) {
      
      /* Activite terminee */
    case ETHER:
      $module$SendFinalReply(servId, activity);
      freeActivity (activity);
      return(TRUE);
      
      /* Activite va se planter */
    case FAIL:
      return(FALSE);

      /* Interruption deja demandee */
    case INTER:
      return FALSE;

      /* Demande d'interruption END, START, EXEC, SLEEP? */
    default:
      ACTIVITY_EVN(activity) = INTER;
      h2evnSignal(EXEC_TASK_ID(ACTIVITY_TASK_NUM(activity)));
      return(FALSE);
    }
  }
}

/****************************************************************************
 *
 *  allocActivity  -  Allocation/Initialisation d'une activite
 *
 *  Retourne : Numero de l'activite ou -1
 */

static int allocActivity (int rqstId,     /* Id de la requete */
			  int requestNum, /* Type d'activite */
			  int execTaskNum,
			  int reentrantFlag,
			  SERV_ID servId)
{
  int activity, i;
  
  /* Il y a une activite ZOMBIE */
  if ($module$NbZombie != 0) {
    if (csServReplySend (servId, rqstId, FINAL_REPLY, 
			 S_$module$CntrlTask_WAIT_ABORT_ZOMBIE_ACTIVITY, 
			 (void *) NULL, 0, (FUNCPTR) NULL) != OK)
      $module$CntrlTaskSuspend (TRUE);
    return -1;
  }

  /* Recherche d'une activite libre */
  /*  activity = 0;
  while(ACTIVITY_STATUS(activity) != ETHER && activity < MAX_ACTIVITIES) 
    activity++;*/

  
  for(i=0; i<MAX_ACTIVITIES; i++) {
    activity = ($module$LastActivityNum+1+i)%MAX_ACTIVITIES;
    if (ACTIVITY_STATUS(activity) == ETHER)
      break;
  }

  /* Pas d'activite libre */
  if (/*activity*/ i == MAX_ACTIVITIES) {
    if (csServReplySend (servId, rqstId, FINAL_REPLY, 
			 S_$module$CntrlTask_TOO_MANY_ACTIVITIES, 
			 (void *) NULL, 0, (FUNCPTR) NULL) != OK)
      $module$CntrlTaskSuspend (TRUE);
    return -1;
  }

  $module$LastActivityNum = activity;
  $module$LastAbsolutActivityNum++;

  /* Cree l'activite */
  ACTIVITY_STATUS(activity) = INIT;
  ACTIVITY_EVN(activity) = NO_EVENT;
  ACTIVITY_RQST_TYPE(activity) = requestNum;
  ACTIVITY_RQST_ID(activity) = rqstId;
  ACTIVITY_ID(activity) = $module$LastAbsolutActivityNum;
  ACTIVITY_BILAN(activity) = OK;
  ACTIVITY_NB_INCOMP(activity) = 0;
  ACTIVITY_TASK_NUM(activity) = execTaskNum;
  ACTIVITY_REENTRANCE(activity) = reentrantFlag;

  /* Initialise les parametres */
  ACTIVITY_INPUT_ID(activity) = NULL;
  ACTIVITY_INPUT_SIZE(activity) = 0;
  ACTIVITY_OUTPUT_ID(activity) = NULL;
  ACTIVITY_OUTPUT_SIZE(activity) = 0;

  /* Une activite de plus */
  EXEC_TASK_NB_ACTI(execTaskNum)++;
  NB_ACTIVITIES++;

  return(activity);
}

/****************************************************************************
 *
 *  controlExecTaskStatus  -  Controle l'état des tâches d'exec
 *
 */
static BOOL controlExecTaskStatus(SERV_ID servId, int rqstId)
{
  int i;

  /* Exec tasks status */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++)
    if (EXEC_TASK_STATUS(i) == ERROR) {
      /* XXXXX Il faudrait definir un bilan spécifique:
	 S_$module$CntrlTask_EXEC_TASK_SUSPENDED */
      csServReplySend (servId, rqstId, FINAL_REPLY, 
		       S_$module$CntrlTask_SYSTEM_ERROR,
		       (void *) NULL, 0, (FUNCPTR) NULL);
      return FALSE;
    }

  return TRUE;
}

/****************************************************************************
 *
 *  controlExecTaskStatusAndInitRqst  -  Controle l'état des tâches d'exec
 *					 et la requete d'init
 *
 */
static BOOL controlExecTaskStatusAndInitRqst(SERV_ID servId, int rqstId)
{
  int i;

  /* Exec tasks status */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++)
    if (EXEC_TASK_STATUS(i) == ERROR) {
      /* XXXXX Il faudrait definir un bilan spécifique:
	 S_$module$CntrlTask_EXEC_TASK_SUSPENDED */
      csServReplySend (servId, rqstId, FINAL_REPLY, 
		       S_$module$CntrlTask_SYSTEM_ERROR,
		       (void *) NULL, 0, (FUNCPTR) NULL);
      return FALSE;
    }

  /* Init request */
  if (INIT_RQST != -1) {
    csServReplySend (servId, rqstId, FINAL_REPLY, 
		     S_$module$CntrlTask_WAIT_INIT_RQST, 
		     (void *) NULL, 0, (FUNCPTR) NULL);
    return FALSE;
  }    

  return TRUE;
}

/****************************************************************************
 *
 *  freeActivity  -  Retire l'activite du tableau d'activite
 */

static void freeActivity (int activity)
{
  int execTaskNum;
  
  /* C'etait une activite ZOMBIE */
  if (ACTIVITY_STATUS(activity) == ZOMBIE) $module$NbZombie--;

  /* Petit netoyage */
  execTaskNum = ACTIVITY_TASK_NUM(activity);
  ACTIVITY_STATUS(activity) = ETHER;
  ACTIVITY_EVN(activity) = NO_EVENT;
  ACTIVITY_TASK_NUM(activity) = -1;
  EXEC_TASK_NB_ACTI(execTaskNum)--;
  NB_ACTIVITIES --;
}


/****************************************************************************
 *
 *  $module$SendFinalReply  -  Envoie la replique finale
 *
 *  Description : Envoie la replique finale avec le bilan d'execution si 
 *                tout c'est bien termine' (dernier etat END), ou avec un 
 *                message d'interruption (dernier etat INTER ou INIT)
 *                ou d'echec (dernier etat FAIL).
 */

static void $module$SendFinalReply (SERV_ID servId,
				    int activity)

{
  int bilan;

  /* 
   * Select bilan according to the last event 
   */
  switch(ACTIVITY_EVN(activity)) {

    /* Normal end or "clean" interruption */
  case ETHER:
    /* Clean interruption or not yet started  */
    if (ACTIVITY_STATUS(activity) == INIT ||
	ACTIVITY_STATUS(activity) == INTER) {
      bilan = S_$module$CntrlTask_ACTIVITY_INTERRUPTED;
      ACTIVITY_BILAN(activity) = bilan;
    }
    
    /* Normal end */
    else {
      bilan = ACTIVITY_BILAN(activity);
      if (bilan != OK && !H2_MODULE_ERR_FLAG(bilan)) {
	CNTRL_TASK_BILAN = bilan;
	bilan = S_$module$CntrlTask_SYSTEM_ERROR;
      }
    }
    break;
    
    /* Activity not yet started */
  case START:
    if (ACTIVITY_STATUS(activity) == INIT) {
      bilan = S_$module$CntrlTask_ACTIVITY_INTERRUPTED;
      ACTIVITY_BILAN(activity) = bilan;      
    }

    /* Echec */
  case ZOMBIE:
    bilan = S_$module$CntrlTask_ACTIVITY_FAILED;
    break;

    /* Interruption before starting (state == INIT) */
  case NO_EVENT:
    bilan = S_$module$CntrlTask_ACTIVITY_INTERRUPTED;
    ACTIVITY_BILAN(activity) = bilan;
    break;

    /* Impossible termination event */
  default:
    printf("$module$CntrlTask: activity %d state %d event %d !?! \n", 
	   activity, ACTIVITY_STATUS(activity), ACTIVITY_EVN(activity));
    errnoSet(S_$module$CntrlTask_FORBIDDEN_ACTIVITY_TRANSITION);
    bilan = S_$module$CntrlTask_FORBIDDEN_ACTIVITY_TRANSITION;
    ACTIVITY_BILAN(activity) = bilan;
  }
  
  /* 
   * Send the final reply 
   */
  if (csServReplySend (servId, ACTIVITY_RQST_ID(activity), FINAL_REPLY, 
		       bilan, (void *) ACTIVITY_OUTPUT_ID(activity),
		       ACTIVITY_OUTPUT_SIZE(activity), (FUNCPTR) NULL) != OK)
    $module$CntrlTaskSuspend (TRUE);

#ifdef MODULE_EVENT
  moduleEventCntrl.eventType = STATE_END_EVENT;
  moduleEventCntrl.activityNum = activity;
  moduleEventCntrl.activityState = ACTIVITY_EVN(activity);
  moduleEventCntrl.rqstType = ACTIVITY_RQST_TYPE(activity);
  moduleEventCntrl.taskNum = ACTIVITY_TASK_NUM(activity);
  sendModuleEvent(&moduleEventCntrl);
#endif	  

}


/****************************************************************************
 *
 *  $module$CntrlTaskSuspend  -  Suspension de la tache de controle
 *
 *  Description : recupere le bilan, puis suspend la tache de controle
 *
 */

static void $module$CntrlTaskSuspend (BOOL giveFlag)

{
  /* Indiquer qu'une erreur a ete detectee */
  CNTRL_TASK_STATUS = ERROR;
  CNTRL_TASK_BILAN = errnoGet();

  printf ("$module$CntrlTaskSuspend: %s\n", h2getMsgErrno(CNTRL_TASK_BILAN));

  /* Mettre a jour le poster de controle */
  if (posterWrite ($module$CntrlPosterId, 0, (void *) $module$CntrlStrId,
		   sizeof ($MODULE$_CNTRL_STR)) != sizeof ($MODULE$_CNTRL_STR)) {
    h2perror("$module$CntrlTaskSuspend: posterWrite");
  }

   /* Liberer les SDIs */
  if (giveFlag) {
    commonStructGive ((void *) $module$DataStrId);
    commonStructGive ((void *) $module$CntrlStrId);    
  }

  /* Suspendre la tache */
  taskSuspend (0);
}



/****************************************************************************
 *
 *  $module$ReplyAndSuspend  -  Repliquer et suspendre
 *
 *  Description : envoie la replique puis se suspend
 */

static void $module$ReplyAndSuspend (SERV_ID servId,  
				     int rqstId,
				     BOOL giveFlag)
{
  /* Indiquer qu'une erreur a ete detectee */
  CNTRL_TASK_STATUS = ERROR;
  CNTRL_TASK_BILAN = errnoGet();

  printf ("$module$CntrlTaskReplyAndSuspend: %s\n", 
	  h2getMsgErrno(CNTRL_TASK_BILAN));

  /* Mettre a jour le poster de controle */
  if (posterWrite ($module$CntrlPosterId, 0, (void *) $module$CntrlStrId,
		   sizeof ($MODULE$_CNTRL_STR)) != sizeof ($MODULE$_CNTRL_STR)) {
    h2perror ("$module$ReplyAndSuspend: posterWrite");
  }

/*XXX  $module$CntrlPosterShow();*/


  /* Liberer les SDIs */
  if (giveFlag) {
    commonStructGive ((void *) $module$DataStrId);
    commonStructGive ((void *) $module$CntrlStrId);    
  }

  /* Envoyer le code d'erreur vers le client */
  (void) csServReplySend (servId, rqstId, FINAL_REPLY, CNTRL_TASK_BILAN, 
			  (void *) NULL, 0, (FUNCPTR) NULL);

  /* Suspendre la tache */
  taskSuspend (0);

}

/*------------------- TRAITEMENT DES REQUETES  -----------------------------*/


/****************************************************************************
 *
 *  $module$RqstAbortActivity  -  Traitement de la requete Abort
 */

static void $module$RqstAbortActivity (SERV_ID servId, int rqstId)

{
  int activityId; /* Absolut activity id */
  int activity; /* Local activity Id */
  int bilan = OK;
  int i;

  /* Get activity to abort */
  if (csServRqstParamsGet (servId, rqstId, (void *) &activityId, 
			   sizeof(int), (FUNCPTR) NULL) != OK)
    $module$ReplyAndSuspend (servId, rqstId, TRUE);

  /* Activity identification */
  if (activityId < 0) {

    switch (activityId) 
      {
	/* Activity -88: on relance les taches d'exec suspendues */
      case GENOM_RESUME_EXEC_TASK:
	for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) {
	  taskResume(EXEC_TASK_ID(i));
	  EXEC_TASK_STATUS(i) = OK;
	}
	break;
    
	/* Activity -99: on libère le module */
      case GENOM_END_MODULE:

	/* Test s'il n'y a pas d'activités en cours */
	if (NB_ACTIVITIES != 0) {
	  bilan = S_$module$CntrlTask_WAIT_ABORT_ZOMBIE_ACTIVITY;
	  /* bilan = S_ACTIVITIES_REMAINED; */
	  break;
	}

	/* Termine le module */
	else {

	  STOP_MODULE_FLAG = TRUE;
	  for (i=$MODULE$_NB_EXEC_TASK-1; i > -1; i--) 
	    EXEC_TASK_WAKE_UP_FLAG(i) = TRUE;

	  /* Liberer l'acces aux SDI */
	  commonStructGive ((void *) $module$DataStrId);
	  commonStructGive ((void *) $module$CntrlStrId);

	  /* Interrompt les taches d'exec */
	  for (i=$MODULE$_NB_EXEC_TASK-1; i > -1; i--) {
	    printf ("Killing task %s ... \n", $module$ExecTaskNameTab[i]);
	    taskResume(EXEC_TASK_ID(i));
	    h2evnSignal(EXEC_TASK_ID(i));
	  }

	  /* Attend la fin */
	  for (i=$MODULE$_NB_EXEC_TASK-1; i > -1; i--) {
	    while (EXEC_TASK_WAKE_UP_FLAG(i))
	      h2evnSusp(0);
	    printf ("    ... task %s killed\n", $module$ExecTaskNameTab[i]);
	  }

	  /* Envoie la réplique finale */
	  csServReplySend (servId, rqstId, FINAL_REPLY, bilan, 
			   (void *) NULL, 0, (FUNCPTR) NULL);

	  
	  /* Destruction des bal, SDI et poster et CIAO ! */
	  csMboxEnd();
	  commonStructDelete ((void *) $module$DataStrId);
	  commonStructDelete ((void *) $module$CntrlStrId);
	  posterDelete($module$CntrlPosterId);
	  printf ("$module$CntrlTask ended\n");
#ifndef VXWORKS
	  /* Detruit le fichier de PID */
	  unlink(pidFilePath);
#endif
	  exit(0);
	}
	break;

      default:
	bilan = S_$module$CntrlTask_UNKNOWN_ACTIVITY;
	break;
      } /* switch */
  }
  
  /* Numéro d'activité positive inconnu */
  else if(activityId > $module$LastAbsolutActivityNum) {
    bilan = S_$module$CntrlTask_UNKNOWN_ACTIVITY;
  }
    
  /* Numéro valide */
  else {

    /* Recherche de l'activité */
    for (activity = 0; activity < MAX_ACTIVITIES; activity++)
      if (ACTIVITY_ID(activity) == activityId)
	break;

    /* Activité déjà terminée */
    if (activity == MAX_ACTIVITIES || ACTIVITY_STATUS(activity) == ETHER)
      bilan = S_$module$CntrlTask_ACTIVITY_ALREADY_ENDED;

    /* Activité trouvée: interruption */
    else 
      $module$AbortActivity(servId, activity);
  }

  /* Send final reply */
  if (csServReplySend (servId, rqstId, FINAL_REPLY, bilan, 
		       (void *) NULL, 0, (FUNCPTR) NULL) != OK)
    $module$CntrlTaskSuspend (TRUE); 
}

/* Requetes de types controle */
/* Requetes de types execution */

