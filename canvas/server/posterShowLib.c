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
/*   PROJET HILARE II - INTERACTION PAR POSTER AVEC LE MODULE  $MODULE$     */
/*   FICHIER SOURCE : "$module$PosterLib.c"                                 */
/****************************************************************************/

/* DESCRIPTION :
   Bibliotheque de fonctions qui permettent la lecture ou l'affichage
   du contenu des posters du module
*/


#include <stdio.h>
#include <stdlib.h>
#ifdef VXWORKS
#include <vxWorks.h>
#else
#include "portLib.h"
#endif

#include "$module$PosterShowLib.h"
#include "$module$Print.h"


  $execTaskNameTabDeclare$

/*  prototypes de la bibliotheque printState */
extern char const * h2GetEvnStateString(int num);

static void $module$ActivityShow ($MODULE$_CNTRL_STR *sdic);


/* ---------------- LE POSTER DE CONTROLE ------------------------------ */

/* ----------------------------------------------------------------------
 *
 *  $module$CntrlPosterShow - Affichage du poster de controle du module
 *
 */

STATUS $module$CntrlPosterShow ()
{
  $MODULE$_CNTRL_STR *sdic;
  int i;

  /* Lecture de la SDI de controle */
  sdic = ($MODULE$_CNTRL_STR *)malloc(sizeof($MODULE$_CNTRL_STR));
  if (sdic == NULL) {
    h2perror ("$module$CntrlPosterShow");
    return ERROR;
  }
  if ($module$CntrlPosterRead (sdic) != OK) {
    free(sdic);
    return ERROR;
  }

  /* 
   * Affichage 
   */

  printf ("** TASKS           (status)      (last duration)         (bilan)\n");
  /* Tache de controle */
  printf (" Control Task        %-5s                               %s\n",  
	  M_CNTRL_TASK_STATUS(sdic)==OK ? "OK":"ERROR",
	  h2getMsgErrno(M_CNTRL_TASK_BILAN(sdic)));
  
  /* Taches d'execution */
  for (i=0; i<$MODULE$_NB_EXEC_TASK; i++) {
    printf (" %d:%-15s   %-5s ",  
	    i, $module$ExecTaskNameTab[i],
	    M_EXEC_TASK_STATUS(sdic,i) == OK ? "OK": "ERROR");
    if (M_EXEC_TASK_PERIOD(sdic,i) != 0) {
      printf ("    %3lums ", M_EXEC_TASK_ON_PERIOD(sdic,i));
      printf ("(max %lu, ", M_EXEC_TASK_MAX_PERIOD(sdic,i));
      printf ("th %d)", (int)(M_EXEC_TASK_PERIOD(sdic,i)*1000.));
    }
    else {
      printf ("    %3lums ", M_EXEC_TASK_ON_PERIOD(sdic,i));
      printf ("(max %lu)        ", M_EXEC_TASK_MAX_PERIOD(sdic,i));      
    }
    printf ("\t %s\n", h2getMsgErrno(M_EXEC_TASK_BILAN(sdic,i)));

  }
  printf ("\n");

  /* Activites */
  $module$ActivityShow (sdic);

  free(sdic);
  return OK;
}

STATUS $module$CntrlPosterActivityShow ()
{
  $MODULE$_CNTRL_STR *sdic;

  /* Lecture de la SDI de controle */
  sdic = ($MODULE$_CNTRL_STR *)malloc(sizeof($MODULE$_CNTRL_STR));
  if (sdic == NULL) {
    h2perror ("$module$CntrlPosterActivityShow");
    return ERROR;
  }
  if ($module$CntrlPosterRead (sdic) != OK) {
    free(sdic);
    return ERROR;
  }

  $module$ActivityShow (sdic);

  free(sdic);
  return OK;
}

/* Fonciton locale d'affichage des activitées */
static void $module$ActivityShow ($MODULE$_CNTRL_STR *sdic)
{
#define $MODULE$_NB_RQST_EXEC $nbExecRqst$
#if $MODULE$_NB_RQST_EXEC != 0

  $requestNameTabDeclare$
  int i,oneActivity=FALSE;
  ACTIVITY_EVENT evn;
  ACTIVITY_STATE status;
  int bilan;
  int rqst;
  
  /*  printf (" ACTIVITY  REQUEST         TASK        FROM -> TO    BILAN\n");*/
  printf ("** ACTIVITIES\n");
  for (i=0; i<MAX_ACTIVITIES; i++) {
    status = M_ACTIVITY_STATUS(sdic,i);
    bilan = M_ACTIVITY_BILAN(sdic,i);

    /* Affichage */
    if (status != ETHER || bilan != OK) {
      evn = M_ACTIVITY_EVN(sdic,i);
      
      /* Identification de la requete */
      rqst=0;
      while($module$ExecRqstNumTab[rqst] != M_ACTIVITY_RQST_TYPE(sdic,i) 
	    && rqst<$MODULE$_NB_RQST_EXEC)
	rqst++;

      if (status != ETHER) 
	oneActivity = TRUE;

      printf ("%s%2d  %2d:%-10s    %-10s\t%5s %s%s\n", 
	      status == ETHER ? " (" : "  ",

	      M_ACTIVITY_ID(sdic,i), 

	      rqst == $MODULE$_NB_RQST_EXEC ? 
	      M_ACTIVITY_RQST_TYPE(sdic,i) : $module$ExecRqstNumTab[rqst],

	      rqst == $MODULE$_NB_RQST_EXEC?
	      "Unknown" : $module$ExecRqstNameTab[rqst], 
	      
	      M_ACTIVITY_TASK_NUM(sdic,i) == -1 ? 
	      "not exec" : $module$ExecTaskNameTab[M_ACTIVITY_TASK_NUM(sdic,i)], 
	      
	      evn == NO_EVENT ?
	      h2GetEvnStateString(status) : h2GetEvnStateString(evn), 

	      h2getMsgErrno(bilan),
	      
	      status == ETHER ? ")" : "");
    }

  }	/* for */
  if (!oneActivity) printf ("  No activity\n");
#endif /* $MODULE$_NB_RQST_EXEC != 0 */
}

/* ---------------- LES POSTERS FONCTIONNELS ------------------------------ */

