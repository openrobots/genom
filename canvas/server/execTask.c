/*	$LAAS$ */

/* --- GENERATED FILE, DO NOT EDIT BY HAND --------------------------- */

/* 
 * Copyright (c) 2004 
 *      Autonomous Systems Lab, Swiss Federal Institute of Technology.
 * Copyright (c) 1993-2004 LAAS/CNRS
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
 * Execution task body
 */

#include "$module$Header.h"

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/time.h>
# define gettimeofday(x,y)	do_gettimeofday(x)
#else
# ifdef VXWORKS
#  define HAS_POSIX_CLOCK
#  include <time.h>
# else
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# endif /* not VXWORKS */
#endif /* not RTAI && KERNEL */

#include <taskLib.h>
#include <errnoLib.h>

#include <commonStructLib.h>
#include <h2evnLib.h>
#include <h2timerLib.h>

#ifdef MODULE_EVENT
#include "moduleEvents.h"
#endif

/* En-tete du module */

/* Print debugging information */
#define GENOM_DEBUG_EXECTASK

#ifdef GENOM_DEBUG_EXECTASK
# define LOGDBG(x)	logMsg x
#else
# define LOGDBG(x)
#endif

/* Taille de la bal de reception des repliques des serveurs */
#define $MODULE$_$EXECTASKNAME$_MBOX_REPLY_SIZE $maxMboxReplySize$


/*--------------------------- VARIABLES EXTERNES -------------------------*/
/* ATTENTION: donner un nom unique */

/* Semaphore d'initialisation */
extern SEM_ID sem$module$InitExecTab[];

/*---------------- PROTOTYPES DES FONCTIONS INTERNES -----------------------*/

/* Initialisation de la tache d'execution */
static STATUS $module$$execTaskName$InitTaskFunc (H2TIMER_ID *execTimerId);

/* Suspension de la tache d'execution */
static void $module$$execTaskName$Suspend (BOOL giveFlag);

/* Appele des fonctions utilisateur */
static ACTIVITY_EVENT execTaskCallUserFunc (ACTIVITY_STATE state, 
					    int activityId);

/* Signal la une transition apres l'avoir validee */
static BOOL filterAndSendEvn (ACTIVITY_STATE state,
			     ACTIVITY_EVENT evn);

/* Retourne la chaine evnStateString caracterisant un etat ou un evenement */
char const * h2GetEvnStateString(int num);

/*--------------------------- VARIABLES LOCALES -------------------------*/
/* ATTENTION: donner un nom unique */

/*---------------- PROTOTYPES DES FONCTIONS utilisateur -------------------*/

/* La fonction d'execution permanente */
#if ($cFuncExecFlag$)
STATUS $cFuncExecName$ (int *bilan);
#endif

/* La fonction d'initialisation */
#if ($cFuncExecInitFlag$)
int $cFuncExecInitName$ ();
#endif

/*---------------------- ROUTINES EXPORTEES --------------------------------*/

/*****************************************************************************
 *
 *  $module$$execTaskName$  -  Tache d'execution
 *
 *  Description : Execution des fonctions 
 *
 *
 *  Retourne: Jamais
 */

void $module$$execTaskName$ (void)
{
  static H2TIMER_ID $module$$execTaskName$TimerId;
  int i, nb, nbActi;
  int prevExecTaskBilan;
  int wakeUpCntrlTask;
#ifdef SDI_UNLOCKABLE
  int SDItaken = FALSE;
#endif
#if($csServersFlag$ /* tache cliente */)
  int extEvn;
#endif
#ifdef MODULE_EVENT
  MODULE_EVENT_STR moduleEvent;
#else
#ifdef HAS_POSIX_CLOCK
  struct timespec tp; /*  mesure du temps */
#else
  struct timeval tv;
#endif /* HAS_POSIX_CLOCK */
#endif
  unsigned long msec0=0, msec1, msec2;
  int firstChrono=TRUE;

  /* Initialisation de la tache */
  errnoSet(0);
  EXEC_TASK_STATUS($execTaskNum$) = 
    $module$$execTaskName$InitTaskFunc (&$module$$execTaskName$TimerId);
  prevExecTaskBilan = EXEC_TASK_BILAN($execTaskNum$) = errnoGet();
  
  /* Rendre le semaphore d'init */
  semGive (sem$module$InitExecTab[$execTaskNum$]);
  
  /* Se suspendre en cas de probleme */
  if(EXEC_TASK_STATUS($execTaskNum$) != OK)
    $module$$execTaskName$Suspend (FALSE);
#ifdef MODULE_EVENT
  moduleEvent.moduleNum = $numModule$;
  moduleEvent.taskNum = $execTaskNum$;
#endif

  /* Boucler indefiniment */
  FOREVER {
#if($periodFlag$ /* Periodique */)
    /* Attendre le declenchement du timer d'asservissement */
    if (h2timerPause ($module$$execTaskName$TimerId) != OK) {
      logMsg("$module$$execTaskName$: h2timerPause error\n");
      $module$$execTaskName$Suspend (FALSE);
    }
#else
    /* Attendre un evenement interne ou externe */
    h2evnSusp(0);
#endif

    /* Lecture du temps */
#ifdef MODULE_EVENT
    moduleEvent.eventType = EXEC_START_EVENT;
    msec1 = sendModuleEvent(&moduleEvent);
#else
#ifdef HAS_POSIX_CLOCK
    clock_gettime(CLOCK_REALTIME, &tp);
    msec1 = (tp.tv_nsec / 1000000) + (tp.tv_sec * 1000);
#else
    gettimeofday(&tv, NULL);
    msec1 = (tv.tv_usec / 1000) + (tv.tv_sec * 1000);
#endif /* HAS_POSIX_CLOCK */
#endif
    if(firstChrono) {firstChrono=FALSE; msec0=msec1;}

#if($periodFlag$ /* tache periodique */)
    if ((EXEC_TASK_ON_PERIOD($execTaskNum$) = msec1 - msec0) 
	> EXEC_TASK_MAX_PERIOD($execTaskNum$))
      EXEC_TASK_MAX_PERIOD($execTaskNum$) = msec1 - msec0;
    msec0 = msec1;
#endif

    wakeUpCntrlTask = FALSE;

    /* Interruption requested */
    if (STOP_MODULE_FLAG) {
      
#if ($cFuncExecEndFlag$)
      /* Execution de la fonction de terminaison */
      $cFuncExecEndName$ ();
#endif
      
      /* Libérer les posters, clients, bal */
      $listPosterDelete$
#if($csServersFlag$ /* Tache cliente */)
      $listServerClientEnd$
      csMboxEnd();
#endif
      /* Libère le sémaphore */
      EXEC_TASK_WAKE_UP_FLAG($execTaskNum$) = FALSE;
      h2evnSignal(CNTRL_TASK_ID);

      return;
    }

    /* Prendre l'acces aux SDI */
#ifdef SDI_UNLOCKABLE
    if (!EXEC_TASK_UNLOCK_SDI($execTaskNum$)) {
#endif
      if (commonStructTake ($module$CntrlStrId) != OK ||
	  commonStructTake ($module$DataStrId) != OK) {
	logMsg("$module$$execTaskName$: commonStructTake error\n");
	$module$$execTaskName$Suspend (FALSE);
      }
#ifdef SDI_UNLOCKABLE
      SDItaken = TRUE;
    }
#endif
    
    /* Test reception d'une replique (XXX: voir remarque plus loin) */
#if($csServersFlag$ /* Tache cliente */)
    extEvn = FALSE;
    if (csMboxStatus(REPLY_MBOX) & REPLY_MBOX)
      extEvn = TRUE;
#endif


    /* Activite permanente */
#if ($cFuncExecFlag$) 
#  ifdef MODULE_EVENT
    moduleEvent.eventType = STATE_START_EVENT;
    moduleEvent.activityNum = -1;
    moduleEvent.activityState = EXEC;
    sendModuleEvent(&moduleEvent);    
#  endif
    CURRENT_ACTIVITY_NUM($execTaskNum$) = -1;
    EXEC_TASK_BILAN($execTaskNum$) = OK;
    if ($cFuncExecName$ (&EXEC_TASK_BILAN($execTaskNum$)) != OK) {
      logMsg("$module$$execTaskName$: permanent activity error\n");
      $module$$execTaskName$Suspend (TRUE);
    }
#  ifdef MODULE_EVENT
    moduleEvent.eventType = STATE_END_EVENT;
    sendModuleEvent(&moduleEvent);   
#  endif
#endif

    /* Recherche des activites */
    nbActi = EXEC_TASK_NB_ACTI($execTaskNum$);
    for (i = 0, nb = 0; nb < nbActi && i < MAX_ACTIVITIES; i++) 
      if (ACTIVITY_TASK_NUM(i) == $execTaskNum$) {
	
	nb++;

	/* Eveil des activites endormies */
	/* XXX pb: on peut louper l'info extEvn: si la lecture est effectuer par une autre activite ou par la fonction permanente et que la "reply" arrive apres le test extEvn et avant la lecture !!!
1ere solution: eveiller systematiquement les activites SLEEP ?!? 
2nd solution: la fonction qui recoit la replique execute EXEC_TASK_WAKE_UP */
	if (ACTIVITY_STATUS(i) == SLEEP && ACTIVITY_EVN(i) == NO_EVENT)
	  ACTIVITY_EVN(i) = EXEC;

	/* Execution en fonction de l'evenement */
	switch (ACTIVITY_EVN(i)) {
	  
	  /* Y'a rien (activite ZOMBIE, SLEEP) */
	case NO_EVENT:
	  break;
	  
	  /* C'est pour la tache de controle */
	case ETHER:
	case ZOMBIE:
	case INIT:
	case SLEEP:
	  break;
	  
	  /* A nous de jouer */
	case START:
	  EXEC_TASK_MAX_PERIOD($execTaskNum$) = 0;
	case EXEC:
	case END:
	case FAIL:
	case INTER:
#ifdef MODULE_EVENT	  
	  moduleEvent.eventType = STATE_START_EVENT;
	  moduleEvent.activityNum = i;
	  moduleEvent.activityState = ACTIVITY_EVN(i);
	  moduleEvent.rqstType = ACTIVITY_RQST_TYPE(i);
	  sendModuleEvent(&moduleEvent);
#endif	  
	  /* On enregistre le nouvel etat */
	  ACTIVITY_STATUS(i) = (ACTIVITY_STATE) ACTIVITY_EVN(i);
	  
	  /* On appele la fonction de traitement */
	  CURRENT_ACTIVITY_NUM($execTaskNum$) = i;
	  ACTIVITY_EVN(i) = execTaskCallUserFunc(ACTIVITY_STATUS(i), i);
	  
	  /* Test validite de l'evenement */
	  if(filterAndSendEvn(ACTIVITY_STATUS(i), ACTIVITY_EVN(i)))
	    wakeUpCntrlTask = TRUE;

#ifdef MODULE_EVENT	  
	  moduleEvent.eventType = STATE_END_EVENT;
	  sendModuleEvent(&moduleEvent);
#endif	  
	  break;
	  
	default:
	  logMsg("$module$$execTaskName$, activity %d: incoherent evn %s\n",
		  i, h2GetEvnStateString (ACTIVITY_EVN(i)));
	  $module$$execTaskName$Suspend (TRUE);
	}           /* switch evn */ 
      }        /* Tant qu'il y a des activites */ 
    
    /*****XXXX Test provisoire */
    if (nb != nbActi)
      logMsg("$module$$execTaskName$: invalid number of activities %d (expected %d) !\n", nb, nbActi);

    CURRENT_ACTIVITY_NUM($execTaskNum$) = -2;
    /* Mise a jour des posters auto */
    $listPosterUpdateFunc$
    
    /* Temps ecoule depuis la precedente lecture */
#ifdef MODULE_EVENT	  
    moduleEvent.eventType = EXEC_END_EVENT;
    msec2 = sendModuleEvent(&moduleEvent);
#else
#ifdef HAS_POSIX_CLOCK
    clock_gettime(CLOCK_REALTIME, &tp);
    msec2 = (tp.tv_nsec / 1000000) + (tp.tv_sec * 1000);
#else
    gettimeofday(&tv, NULL);
    msec2 = (tv.tv_usec / 1000) + (tv.tv_sec * 1000);
#endif /* HAS_POSIX_CLOCK */
#endif

#if (!$periodFlag$)
    EXEC_TASK_ON_PERIOD($execTaskNum$) = msec2 - msec1;
    if (EXEC_TASK_ON_PERIOD($execTaskNum$) > EXEC_TASK_MAX_PERIOD($execTaskNum$))
      EXEC_TASK_MAX_PERIOD($execTaskNum$) = EXEC_TASK_ON_PERIOD($execTaskNum$);
#endif

    /* Bilan a change: on previent la tache de controle */
    if (prevExecTaskBilan != EXEC_TASK_BILAN($execTaskNum$)) {
      prevExecTaskBilan =  EXEC_TASK_BILAN($execTaskNum$);
      wakeUpCntrlTask = TRUE;
    }

    /* On eveil la tache de controle */
    if(wakeUpCntrlTask)
      h2evnSignal(CNTRL_TASK_ID);

    /* Liberer l'acces aux SDI */
#ifdef SDI_UNLOCKABLE
    if (SDItaken) {
#endif
      if (commonStructGive ($module$DataStrId) != OK ||
	  commonStructGive ($module$CntrlStrId) != OK) {
	logMsg("$module$$execTaskName$: commonStructGive error\n");
	$module$$execTaskName$Suspend (FALSE);
      }
#ifdef SDI_UNLOCKABLE
    }
#endif
  }     /* FOREVER */

}



/*----------------------  ROUTINES LOCALES ---------------------------------*/



/*****************************************************************************
*
*  $module$$execTaskName$InitTaskFunc  -  Routine initialisation
*
*  Description:
*  Cette fonction alloue et initialise tous les objets utilises par la tache
*  d'execution.
*
*  Retourne : OK ou ERROR
*/

static STATUS $module$$execTaskName$InitTaskFunc (H2TIMER_ID *execTimerId)

{
#if ($cFuncExecInitFlag$)
  int bilan;
#endif

    /* Enregistrer l'id de la tache */
  EXEC_TASK_ID($execTaskNum$) = taskIdSelf ();

#if($csServersFlag$) /* Client */
  /* Creation de la boite aux lettres de reception des repliques */
  if (csMboxInit ("$module$$execTaskName$", 0,  
                  $MODULE$_$EXECTASKNAME$_MBOX_REPLY_SIZE) != OK) {
    h2perror("$module$$execTaskName$InitTaskFunc: csMboxInit");
    return (ERROR); 
  }

  /* S'initialiser comme client */
  $listServerClientInit$
#endif

#if($periodFlag$) /* Periodic */

    /*** XXXX
      Passer la periode en milliseconde. Verifier que c'est un
      nombre entier de tic:
      Utiliser sysClkRateGet au lieu de NTICKS_PER_SEC
      Sinon sortir 
      XXX */
  /* Allouer un timer h2 */
  if ((*execTimerId = h2timerAlloc ()) == NULL) {
    h2perror("$module$$execTaskName$InitTaskFunc: h2timerAlloc");
    return (ERROR);
  }
  LOGDBG(("$module$$execTaskName$InitTaskFunc: timer allocated\n"));

  /* Demarrer le timer d'asservissement */
  if (h2timerStart (*execTimerId, $period$, $delay$) != OK) {
    h2perror("$module$$execTaskName$InitTaskFunc: h2timerStart");
    return (ERROR);
  }
  LOGDBG(("$module$$execTaskName$InitTaskFunc: timer started\n"));
  
  /* Obtenir la periode d'asservissement */
  EXEC_TASK_PERIOD($execTaskNum$) = 
    ((double) $period$ / (double) NTICKS_PER_SEC);
#else
  EXEC_TASK_PERIOD($execTaskNum$) = 0;
#endif /* Periodic */

  EXEC_TASK_MAX_PERIOD($execTaskNum$) = 0;
  EXEC_TASK_ON_PERIOD($execTaskNum$) = 0;
  EXEC_TASK_WAKE_UP_FLAG($execTaskNum$) = FALSE;
#ifdef SDI_UNLOCKABLE
  EXEC_TASK_UNLOCK_SDI($execTaskNum$) = FALSE;
#endif
  /* Creer le poster */
  $listPosterCreate$
  LOGDBG(("$module$$execTaskName$InitTaskFunc: posters created\n"));
      
  /* S'initialiser comme client des Posters */
  $listPosterInit$
  LOGDBG(("$module$$execTaskName$InitTaskFunc: client posters initialized\n"));

  /* Enregister le nom de la tache */
/*  strcpy (EXEC_TASK_NAME($execTaskNum$), "execTaskName"); */

#if ($cFuncExecInitFlag$)
    /* Execution de la fonction d'initialisation */
  if ($cFuncExecInitName$ (&bilan) != OK) {
    errnoSet(bilan);
    h2perror("$module$$execTaskName$InitTaskFunc: $cFuncExecInitName$");
    return (ERROR);
  }
#endif

   /* Donner le sem de fin d'initialisation */
  LOGDBG(("$module$$execTaskName$InitTaskFunc: ok\n"));
  return (OK);
}

/*****************************************************************************
*
*  $module$$execTaskName$Suspend  -  Suspension de la tache d'asservissement
*
*  Description :
*  Cette fonction signale qu'une erreur a ete retrouvee a l'interieur de 
*  la tache et se suspend par la suite
*
*  Retourne: Neant
*/

static void $module$$execTaskName$Suspend (BOOL giveFlag)

{
  /* Indiquer qu'une erreur a ete detectee */
  EXEC_TASK_STATUS($execTaskNum$) = ERROR;
  if (EXEC_TASK_BILAN($execTaskNum$) == OK)
    EXEC_TASK_BILAN($execTaskNum$) = errnoGet();

  logMsg("Suspend $module$$execTaskName$: %s\n", 
	  h2getMsgErrno(EXEC_TASK_BILAN($execTaskNum$)));

  /* Eveiller la tache de controle */
  h2evnSignal(CNTRL_TASK_ID);

  /* Liberer les SDIs */
  if (giveFlag) {
    commonStructGive ((void *) $module$DataStrId);
    commonStructGive ((void *) $module$CntrlStrId);    
  }

  /* Suspendre la tache */
  taskSuspend (0);
}

/*****************************************************************************
*
*  execTaskCallUserFunc  -  Appele la fonction utilisateur correspondant au 
*                           numero d'activite et on etat. 
*
*  Description : 
*
*  Retourne: L'etat retourne par la fonction utilisateur 
*/

static ACTIVITY_EVENT execTaskCallUserFunc (ACTIVITY_STATE state, 
					    int activityId)

{
  void *inputId;          /* Adresse input */
  void *outputId;         /* Adresse output */
  int inputFlag;          /* Flag input */
  int outputFlag;         /* Flag output */
  int *bilan;             /* Adresse bilan */
  int activityNum;        /* Numero d'activite */

  /* Tableau des fonction d'execution (user) */
  $execFuncTabDeclare$
  /* Tableau des fonctions de demarrage (user)*/
  $execFuncTabStartDeclare$
  /* Tableau des fonctions de terminaison (user) */
  $execFuncTabEndDeclare$
  /* Tableau des fonctions de terminaison en cas d'interruption (user) */
  $execFuncTabInterDeclare$ 
  /* Tableau des fonctions de terminaison en cas d'echec (user) */
  $execFuncTabFailDeclare$ 

  /* Numero de requete */
  activityNum = ACTIVITY_RQST_TYPE(activityId);

  /* Adresse des parametres */
  inputId = ACTIVITY_INPUT_ID(activityId);
  outputId = ACTIVITY_OUTPUT_ID(activityId);
  bilan =  &ACTIVITY_BILAN(activityId);

  /* Flag parametres */
  inputFlag = inputId == NULL ? FALSE : TRUE;
  outputFlag = outputId == NULL ? FALSE : TRUE;

  switch(state) {
    /* Fonction de demarage */
  case START:
    if ($module$$execTaskName$ExecFuncStartTab [activityNum] == NULL)
      return (EXEC);
    if (inputFlag && outputFlag)
      return((*$module$$execTaskName$ExecFuncStartTab [activityNum])
	     (inputId, outputId, bilan));
    if (inputFlag)
      return((*$module$$execTaskName$ExecFuncStartTab [activityNum])
	     (inputId, bilan));
    if (outputFlag)
      return((*$module$$execTaskName$ExecFuncStartTab [activityNum])
	     (outputId, bilan));
    return((*$module$$execTaskName$ExecFuncStartTab [activityNum])
	   (bilan));
    
    /* Fonction d'execution */
  case EXEC:
    if ($module$$execTaskName$ExecFuncTab [activityNum] == NULL)
      return (END);
    if (inputFlag && outputFlag)
      return((*$module$$execTaskName$ExecFuncTab [activityNum])
	     (inputId, outputId, bilan));
    if (inputFlag)
      return((*$module$$execTaskName$ExecFuncTab [activityNum])
	     (inputId, bilan));
    if (outputFlag)
      return((*$module$$execTaskName$ExecFuncTab [activityNum])
	     (outputId, bilan));
    return((*$module$$execTaskName$ExecFuncTab [activityNum])
	   (bilan));
    
    /* Fonction de terminaison normale, sur pb ou sur interruption */
  case END:
    if ($module$$execTaskName$ExecFuncEndTab [activityNum] == NULL)
      return(ETHER);
    if (inputFlag && outputFlag)
      return((*$module$$execTaskName$ExecFuncEndTab [activityNum])
	     (inputId, outputId, bilan));
    if (inputFlag)
      return((*$module$$execTaskName$ExecFuncEndTab [activityNum])
	     (inputId, bilan));
    if (outputFlag)
      return((*$module$$execTaskName$ExecFuncEndTab [activityNum])
	     (outputId, bilan));
    return((*$module$$execTaskName$ExecFuncEndTab [activityNum])
	   (bilan));

    /* Fonction a appeler en cas d'echec */
  case FAIL:
    if ($module$$execTaskName$ExecFuncFailTab [activityNum] == NULL)
      return(ZOMBIE);
    if (inputFlag && outputFlag)
      return((*$module$$execTaskName$ExecFuncFailTab [activityNum])
	     (inputId, outputId, bilan));
    if (inputFlag)
      return((*$module$$execTaskName$ExecFuncFailTab [activityNum])
	     (inputId, bilan));
    if (outputFlag)
      return((*$module$$execTaskName$ExecFuncFailTab [activityNum])
	     (outputId, bilan));
    return((*$module$$execTaskName$ExecFuncFailTab [activityNum])
	   (bilan));

    /* Fonction a appeler en cas d'interruption */
  case INTER:
    if ($module$$execTaskName$ExecFuncInterTab [activityNum] == NULL)
      return(ETHER);
    if (inputFlag && outputFlag)
      return((*$module$$execTaskName$ExecFuncInterTab [activityNum])
	     (inputId, outputId, bilan));
    if (inputFlag)
      return((*$module$$execTaskName$ExecFuncInterTab [activityNum])
	     (inputId, bilan));
    if (outputFlag)
      return((*$module$$execTaskName$ExecFuncInterTab [activityNum])
	     (outputId, bilan));
    return((*$module$$execTaskName$ExecFuncInterTab [activityNum])
	   (bilan));

    /* Etats impossibles: ZOMBIE ETHER INIT et autres */
  default:
    logMsg("$module$$execTaskName$: Activity %d status %s incoherent\n",
	    activityId, h2GetEvnStateString(state));
    $module$$execTaskName$Suspend (TRUE);
    return(ZOMBIE);
  }  /* Switch state */
}
    
/*****************************************************************************
*
*  filterAndSendEvn  -  Teste et signal les transitions
*
*  Description : Cette fonction teste si une transition d'une activite
*                est valide. Sinon la tache est suspendue.
*
*  Retourne: TRUE s'il faut eveiller la tache de controle.
*/
	
static BOOL filterAndSendEvn (ACTIVITY_STATE state,
			      ACTIVITY_EVENT evn)
{
  BOOL transition=FALSE;
  BOOL wakeUpCntrlTask=FALSE;

  /* Transitions autorisees */
  switch (state) {
  case START:
    if (evn == EXEC || evn == END || evn == FAIL || 
	evn == ETHER || evn == ZOMBIE || evn == SLEEP || evn == START)
      transition = TRUE;
    break;
  case EXEC: 
    if (evn == EXEC || evn == END || evn == FAIL || 
	evn == ETHER || evn == ZOMBIE || evn == SLEEP)
      transition = TRUE;
    break;
  case END:
    if (evn == ZOMBIE || evn == ETHER || evn == END || evn == FAIL)
      transition = TRUE;
    break;
  case FAIL:
    if (evn == ZOMBIE)
      transition = TRUE;
    break;
  case INTER:
    if (evn == ZOMBIE || evn == ETHER || evn == FAIL || evn == INTER)
      transition = TRUE;
    break;
  default:
    logMsg("$module$$execTaskName$: status %s incoherent\n",
	    h2GetEvnStateString(state));
    $module$$execTaskName$Suspend (TRUE);  
  } 

  /* Transition autorisee */
  if (transition) {

#if(!$periodFlag$) /* tache non periodique */
    /* Tache non periodique: Auto reveille sauf evn SLEEP */
    if (evn != SLEEP)
      h2evnSignal(EXEC_TASK_ID($execTaskNum$));
/*    semGive (EXEC_TASK_SEM_ID($execTaskNum$));*/
#endif

    /* Changement d'etat en vu : il faudra prevenir la tache de controle */
    if (state != (ACTIVITY_STATE) evn) 
      wakeUpCntrlTask = TRUE;
  }

  /* Transition interdite */
  else {
    /* Ne pas appeler 2 fois la fonction h2GetEvnStateString dans le logMsg */
    logMsg("$module$$execTaskName$: event %s ", h2GetEvnStateString(evn));
    logMsg("from state %s not allowed\n", h2GetEvnStateString(state)); 
    $module$$execTaskName$Suspend (TRUE);
  }
  return wakeUpCntrlTask;
}
