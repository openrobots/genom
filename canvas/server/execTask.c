
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

/*
 * Execution task body
 */

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/time.h>
# define gettimeofday(x,y)	do_gettimeofday(x)
#else
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#endif /* not RTAI && KERNEL */

#include <taskLib.h>
#include <errnoLib.h>

#include <commonStructLib.h>
#include <h2evnLib.h>
#include <h2timerLib.h>

#include <genom/modules.h>
#include <genom/moduleEvents.h>

#include "$module$Header.h"


/* Print debugging information */
#define GENOM_DEBUG_EXECTASK

#ifdef GENOM_DEBUG_EXECTASK
# define LOGDBG(x)	logMsg x
#else
# define LOGDBG(x)
#endif

/* Size of the mailbox receiving replies from servers */
#define $MODULE$_$EXECTASKNAME$_MBOX_REPLY_SIZE $maxMboxReplySize$


/*--------------------------- EXTERNAL VARIABLES -------------------------*/
/* WARNING: use unique names! */

/* Initialization semaphore */
extern SEM_ID sem$module$InitExecTab[];

/*---------------- PROTOTYPES OF INTERNAL FUNCTIONS ----------------------*/

/* Execution task initialization */
static STATUS $module$$execTaskName$InitTaskFunc (H2TIMER_ID *execTimerId);

/* Suspend the the execution task */
static void $module$$execTaskName$Suspend (BOOL giveFlag);

/* Call of user functions */
static ACTIVITY_EVENT execTaskCallUserFunc (ACTIVITY_STATE state, 
					    int activityId);

/* Signal a transition after handling it */
static BOOL filterAndSendEvn (ACTIVITY_STATE state,
			     ACTIVITY_EVENT evn);

/* Returns a string representing the h2 event */
char const * h2GetEvnStateString(int num);

/*--------------------------- LOCAL VARIABLES -------------------------*/
/* WARNING: use unique names */

/*---------------- User function prototypes -------------------*/

/* Permanent activity functions */
#if ($cFuncExecFlag$)
STATUS $cFuncExecName$ (int *bilan);
#endif
#if ($cFuncExecFlag2$)
STATUS $cFuncExecName2$ (int *bilan);
#endif

/* Initialisation function */
#if ($cFuncExecInitFlag$)
int $cFuncExecInitName$ ();
#endif

/*---------------------- EXPORTED PROCEDURES --------------------------------*/

/*****************************************************************************
 *
 *  $module$$execTaskName$  -  Exec task
 *
 *  Description : Execute fonctions in the module
 *
 *
 *  Returns: never
 */

void $module$$execTaskName$ (void)
{
  static H2TIMER_ID $module$$execTaskName$TimerId;
  int i, nb, nbActi;
  int prevExecTaskBilan;
  int wakeUpCntrlTask;
#if ($csServersFlag$) /* client task */
  int extEvn;
#endif
  int periodOverShot=0;

  MODULE_EVENT_STR moduleEvent;
#ifdef HAS_POSIX_CLOCK
  struct timespec tp; /* time measure */
#else
  struct timeval tv;
#endif /* HAS_POSIX_CLOCK */

  unsigned long mseBeginPrev=0, msecBegin, msecEnd, meanDuration, nbIter=1;
  int firstChrono=TRUE;
  int computeMeanFlag = FALSE; /* try to compute meanDuration only if main exec loop and not abnormal overshot */

  /* Initialization of task */
  errnoSet(0);
  EXEC_TASK_STATUS($execTaskNum$) = 
    $module$$execTaskName$InitTaskFunc (&$module$$execTaskName$TimerId);
  prevExecTaskBilan = EXEC_TASK_BILAN($execTaskNum$) = errnoGet();
  
  /* Release the initialization semaphore */
  semGive (sem$module$InitExecTab[$execTaskNum$]);
  
  /* suspend ourselves in case of problems */
  if(EXEC_TASK_STATUS($execTaskNum$) != OK)
    $module$$execTaskName$Suspend (FALSE);
  moduleEvent.moduleNum = $numModule$;
  moduleEvent.taskNum = $execTaskNum$;

  /* main loop */
  FOREVER {
#if ($periodFlag$) /* Periodic task */
    if (h2timerPause ($module$$execTaskName$TimerId) != OK) {
      logMsg("$module$$execTaskName$: h2timerPause error\n");
      $module$$execTaskName$Suspend (FALSE);
    }
#else /* wait for external events */
    if (h2evnSusp(0) != TRUE) {
      printf ("$module$$execTaskName$: h2evnSusp error\n");
      $module$$execTaskName$Suspend (FALSE);
    }
#endif

    /* Get time */
    moduleEvent.eventType = EXEC_START_EVENT;
    sendModuleEvent(&moduleEvent);

#ifdef HAS_POSIX_CLOCK
    clock_gettime(CLOCK_REALTIME, &tp);
    msecBegin = (tp.tv_nsec / 1000000) + (tp.tv_sec * 1000);
#else
    gettimeofday(&tv, NULL);
    msecBegin = (tv.tv_usec / 1000) + (tv.tv_sec * 1000);
#endif /* HAS_POSIX_CLOCK */
    EXEC_TASK_TIME_BEGIN_LOOP($execTaskNum$) = msecBegin;

    if(firstChrono) {firstChrono=FALSE; mseBeginPrev=msecBegin;}

    computeMeanFlag = 1;
#if ($periodFlag$) /* periodic task */
    if ((EXEC_TASK_ON_PERIOD($execTaskNum$) = msecBegin - mseBeginPrev) 
	> EXEC_TASK_MAX_PERIOD($execTaskNum$)) {
      EXEC_TASK_MAX_PERIOD($execTaskNum$) = msecBegin - mseBeginPrev;
      computeMeanFlag = 0;
    }
    mseBeginPrev = msecBegin;

    /* Previous computation overshot */
    if (EXEC_TASK_DURATION_LOOP($execTaskNum$) > (1000.0*EXEC_TASK_PERIOD($execTaskNum$))) {
      periodOverShot = (int) ((EXEC_TASK_DURATION_LOOP($execTaskNum$)
			       /(1000.0*EXEC_TASK_PERIOD($execTaskNum$))));
      
      if (GENOM_PRINT_TIME_FLAG) 
	printf("$module$$execTaskName$ overshot of %d periods (d=%ldms mean=%ldms p=%ldms max=%ldms th=%dms)\n",
	       periodOverShot,
	       EXEC_TASK_DURATION_LOOP($execTaskNum$),
	       meanDuration,
	       EXEC_TASK_ON_PERIOD($execTaskNum$),
	       EXEC_TASK_MAX_PERIOD($execTaskNum$),
	       (int)(1000*EXEC_TASK_PERIOD($execTaskNum$)));
    }
    else
      periodOverShot = 0;
#endif

    wakeUpCntrlTask = FALSE;

    /* Interruption requested */
    if (STOP_MODULE_FLAG) {
      
#if ($cFuncExecEndFlag$)
      /* Execute end codel */
      $cFuncExecEndName$ ();
#endif
      
      /* free posters, clients, mailboxes */
      $listPosterDelete$
#if ($csServersFlag$) /* client task */
      $listServerClientEnd$
      csMboxEnd();
#else
#if (!$periodFlag$) /* non-periodic task */
      /* free device created to manage h2evn required to aperiodic tasks */
      mboxEnd(0);
#endif
#endif
      /* free semaphore */
      EXEC_TASK_WAKE_UP_FLAG($execTaskNum$) = FALSE;
      h2evnSignal(CNTRL_TASK_ID);

      return;
    }

    /* take IDS access */
      if (commonStructTake ($module$CntrlStrId) != OK ||
	  commonStructTake ($module$DataStrId) != OK) {
	logMsg("$module$$execTaskName$: commonStructTake error\n");
	$module$$execTaskName$Suspend (FALSE);
      }
    
    /* Check for a received reply (XXX: see remark down) */
#if ($csServersFlag$) /* client task */
    extEvn = FALSE;
    if (csMboxStatus(REPLY_MBOX) & REPLY_MBOX)
      extEvn = TRUE;
#endif


    /* If last period overshot, does not execute activities this time */
    /* XXXXXX NOT YET: AFTER COGNIRON, SEEMS TO HAVE SIDES EFFECTS
       if (!periodOverShot) { */
    
    /* permanent activity 1 */
#if ($cFuncExecFlag$) 
    moduleEvent.eventType = STATE_START_EVENT;
    moduleEvent.activityNum = -1;
    moduleEvent.activityState = EXEC;
    sendModuleEvent(&moduleEvent);    

    CURRENT_ACTIVITY_NUM($execTaskNum$) = -1;
    EXEC_TASK_BILAN($execTaskNum$) = OK;
    if ($cFuncExecName$ (&EXEC_TASK_BILAN($execTaskNum$)) != OK) {
      logMsg("$module$$execTaskName$: permanent activity error\n");
      $module$$execTaskName$Suspend (TRUE);
    }

    moduleEvent.eventType = STATE_END_EVENT;
    sendModuleEvent(&moduleEvent);   
#endif

    /* Look for activities */
    nbActi = EXEC_TASK_NB_ACTI($execTaskNum$);
    for (i = 0, nb = 0; nb < nbActi && i < MAX_ACTIVITIES; i++) 
      if (ACTIVITY_TASK_NUM(i) == $execTaskNum$) {
	
	nb++;

	/* Wake up sleeping activities */
	/* XXX pb: on peut louper l'info extEvn: si la lecture est effectuer par une autre activite ou par la fonction permanente et que la "reply" arrive apres le test extEvn et avant la lecture !!!
1ere solution: eveiller systematiquement les activites SLEEP ?!? 
2nd solution: la fonction qui recoit la replique execute EXEC_TASK_WAKE_UP */
	if (ACTIVITY_STATUS(i) == SLEEP && ACTIVITY_EVN(i) == NO_EVENT)
	  ACTIVITY_EVN(i) = EXEC;

	/* Execute, depending on the event */
	switch (ACTIVITY_EVN(i)) {
	  
	  /* Nothing (ZOMBIE, SLEEP activity) */
	case NO_EVENT:
	  break;
	  
	  /* Handled by the control task */
	case ETHER:
	case ZOMBIE:
	case INIT:
	case SLEEP:
	  break;
	  
	  /* This is for us */
	case START:
	  EXEC_TASK_MAX_PERIOD($execTaskNum$) = 0;
	case END:
	case FAIL:
	case INTER:
	  computeMeanFlag = 0;
	case EXEC:
	  moduleEvent.eventType = STATE_START_EVENT;
	  moduleEvent.activityNum = i;
	  moduleEvent.activityState = ACTIVITY_EVN(i);
	  moduleEvent.rqstType = ACTIVITY_RQST_TYPE(i);
	  sendModuleEvent(&moduleEvent);

	  /* record new state */
	  ACTIVITY_STATUS(i) = (ACTIVITY_STATE) ACTIVITY_EVN(i);
	  
	  /* Call the processing function */
	  CURRENT_ACTIVITY_NUM($execTaskNum$) = i;
	  ACTIVITY_EVN(i) = execTaskCallUserFunc(ACTIVITY_STATUS(i), i);
	  
	  /* Check event validity */
	  if(filterAndSendEvn(ACTIVITY_STATUS(i), ACTIVITY_EVN(i)))
	    wakeUpCntrlTask = TRUE;

	  moduleEvent.eventType = STATE_END_EVENT;
	  sendModuleEvent(&moduleEvent);
	  break;
	  
	default:
	  logMsg("$module$$execTaskName$, activity %d: incoherent evn %s\n",
		  i, h2GetEvnStateString (ACTIVITY_EVN(i)));
	  $module$$execTaskName$Suspend (TRUE);
	}           /* switch evn */ 
      }        /* while there are activities */ 
    
    /*****XXXX Temporary test */
    if (nb != nbActi)
      logMsg("$module$$execTaskName$: invalid number of activities %d (expected %d) !\n", nb, nbActi);

/*XXXXXXXX     }  *//* period overshot test */

    /* permanent activity 2 */
#if ($cFuncExecFlag2$) 
    moduleEvent.eventType = STATE_START_EVENT;
    moduleEvent.activityNum = -1;
    moduleEvent.activityState = EXEC;
    sendModuleEvent(&moduleEvent);    

    CURRENT_ACTIVITY_NUM($execTaskNum$) = -1;
    EXEC_TASK_BILAN($execTaskNum$) = OK;
    if ($cFuncExecName2$ (&EXEC_TASK_BILAN($execTaskNum$)) != OK) {
      logMsg("$module$$execTaskName$: permanent activity 2 error\n");
      $module$$execTaskName$Suspend (TRUE);
    }

    moduleEvent.eventType = STATE_END_EVENT;
    sendModuleEvent(&moduleEvent);   
#endif

    /* no more activity */
    CURRENT_ACTIVITY_NUM($execTaskNum$) = -2;

    /* update "auto" posters */
    $listPosterUpdateFunc$
    
    /* Time elapsed since previous read */
    moduleEvent.eventType = EXEC_END_EVENT;
    sendModuleEvent(&moduleEvent);

#ifdef HAS_POSIX_CLOCK
    clock_gettime(CLOCK_REALTIME, &tp);
    msecEnd = (tp.tv_nsec / 1000000) + (tp.tv_sec * 1000);
#else
    gettimeofday(&tv, NULL);
    msecEnd = (tv.tv_usec / 1000) + (tv.tv_sec * 1000);
#endif /* HAS_POSIX_CLOCK */

#if (!$periodFlag$)
    EXEC_TASK_ON_PERIOD($execTaskNum$) = msecEnd - msecBegin;
    if (EXEC_TASK_ON_PERIOD($execTaskNum$) > EXEC_TASK_MAX_PERIOD($execTaskNum$)) {
      EXEC_TASK_MAX_PERIOD($execTaskNum$) = EXEC_TASK_ON_PERIOD($execTaskNum$);
      computeMeanFlag = 0;
    }
#endif

    EXEC_TASK_TIME_END_LOOP($execTaskNum$) = msecEnd;
    EXEC_TASK_DURATION_LOOP($execTaskNum$) = msecEnd-msecBegin;
    if (computeMeanFlag) {
      meanDuration = (int)
	(((double)((nbIter-1)*meanDuration + EXEC_TASK_DURATION_LOOP($execTaskNum$))
	  /((double)nbIter)));
      nbIter++;
    }

    /* display about time */
    if (GENOM_PRINT_TIME_FLAG) {
      printf("$module$$execTaskName$: d %4ld mean %4ld p %4ld max %4ld th %4d\n",
	     EXEC_TASK_DURATION_LOOP($execTaskNum$),
	     meanDuration,
	     EXEC_TASK_ON_PERIOD($execTaskNum$),
	     EXEC_TASK_MAX_PERIOD($execTaskNum$),
	     (int)(EXEC_TASK_PERIOD($execTaskNum$)*1000.0)); 
    }

    /* The result changed: inform the control task */
    if (prevExecTaskBilan != EXEC_TASK_BILAN($execTaskNum$)) {
      prevExecTaskBilan =  EXEC_TASK_BILAN($execTaskNum$);
      wakeUpCntrlTask = TRUE;
    }

    if(wakeUpCntrlTask)
      h2evnSignal(CNTRL_TASK_ID);

    /* Wake-up others tasks ? */
    for (i=0; i<CNTRL_NB_EXEC_TASKS; i++) {
      if (EXEC_TASK_WAKE_UP_FLAG(i)) {
	h2evnSignal(EXEC_TASK_ID(i));
	EXEC_TASK_WAKE_UP_FLAG(i) = FALSE;
      }
    }

    if (commonStructGive ($module$DataStrId) != OK ||
	commonStructGive ($module$CntrlStrId) != OK) {
      logMsg("$module$$execTaskName$: commonStructGive error\n");
      $module$$execTaskName$Suspend (FALSE);
    }

    /* relinquish CPU to give a chance to other tasks to run in heavily
     * contended situations (small period) */
    taskDelay(0);
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

#if ($csServersFlag$) /* Client */
  /* Creation de la boite aux lettres de reception des repliques */
  if (csMboxInit ("$module$$execTaskName$", 0,  
                  $MODULE$_$EXECTASKNAME$_MBOX_REPLY_SIZE) != OK) {
    h2perror("$module$$execTaskName$InitTaskFunc: csMboxInit");
    return (ERROR); 
  }

  /* S'initialiser comme client */
  $listServerClientInit$
#endif

#if ($periodFlag$) /* Periodic */

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

  /* Record errors */
/*   $module$RecordH2errMsgs(); */

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
  char str[64];

  /* Indiquer qu'une erreur a ete detectee */
  EXEC_TASK_STATUS($execTaskNum$) = ERROR;
  if (EXEC_TASK_BILAN($execTaskNum$) == OK)
    EXEC_TASK_BILAN($execTaskNum$) = errnoGet();

  logMsg("Suspend $module$$execTaskName$: %s\n", 
	  h2getErrMsg(EXEC_TASK_BILAN($execTaskNum$), str, 64));

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

#if (!$periodFlag$) /* tache non periodique */
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
