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

/*------------------  Fichier g�n�r� automatiquement ------------------*/
/*------------------  Ne pas �diter manuellement !!! ------------------*/
 
/****************************************************************************
 *   LABORATOIRE D'AUTOMATIQUE ET D'ANALYSE DE SYSTEMES - LAAS / CNRS       
 *   PROJET HILARE II - ROUTINE D'ESSAY INTERACTIF       
 *   FICHIER SOURCE: $module$Essay.c                                          
 ****************************************************************************/

/* VERSION ACTUELLE / HISTORIQUE DES MODIFICATIONS :
 */

/* DESCRIPTION :
   Cette routine doit etre appelee dans le shell de vxWorks ou UNIX.
   */

/*----------------------------- INCLUSIONS ---------------------------------*/

/* Inclusions generales */
#ifdef VXWORKS
#include <vxWorks.h>
#else
#include "portLib.h"
#include "h2initGlob.h"
#endif
#include <taskLib.h>

#ifdef VXWORKS
#include "h2sysLibProto.h"
#endif
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include "xes.h"

/* Inclusions pour acceder au module */
#include "$module$MsgLib.h"
#include "$module$PosterShowLib.h" 
#include "$module$Print.h"
#include "$module$Scan.h"

#include "genom/genomReportsLib.h"
/* #include "$module$Reports.h" */

#include "genom/essayLib.h"


void $module$Essay (int essayNumber);

/*------------------- PROTOTYPES DES FONCTIONS LOCALES --------------------*/

static void $module$EssayInitTask (ESSAY_STR* essayStr);

#define $MODULE$_NAME "$module$"

$requestFuncTabDeclare$
  $requestNameTabDeclare$

  $posterNameTabDeclare$
  $posterShowFuncTabDeclare$


/*--------------------------------------------------------------------------*/

#ifndef VXWORKS
int
main(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "usage: $module$Essay <numMBox>\n");
    exit(1);
  }
  if (h2initGlob(0) == ERROR) {
      exit(2);
  }
  $module$Essay(atoi(argv[1]));
  /*NOTREACHED*/
  return 0;
} /* main */
#endif /* UNIX */



/**
 **   $module$Essay - Fonction de essay du module $module$
 **/

void
$module$Essay (int essayNumber)     
{
  ESSAY_STR *essayStr;
  
  /* Allocation de la structure */
  if ((essayStr = essayInit(essayNumber, $MODULE$_NAME,
			  $MODULE$_CLIENT_MBOX_REPLY_SIZE,
			  $MODULE$_ABORT_RQST,
			  $nbRequest$,
			  $module$EssayRequestNameTab,
			  $module$EssayRqstFuncTab,
			  $nbPosterData$, 
			  $module$EssayPosterNameTab,
			  $module$EssayPosterShowFuncTab)) == NULL)
    return;

  /* Init specifiques */
  $module$EssayInitTask (essayStr);
  
  /* Fonction principale */
  essayMain(essayStr);
}

/*----------------------- ROUTINES LOCALES ---------------------------------*/


/**
 **  $module$EssayInitTask - Routine d'initialisation de la tache d'essai
 **/

static void $module$EssayInitTask (ESSAY_STR *essayStr)
     
{
  /* S'initialiser comme client */
  printf ("client init ...");
  if (csClientInit ($MODULE$_MBOX_NAME, $MODULE$_MAX_RQST_SIZE,
		    $MODULE$_MAX_INTERMED_REPLY_SIZE, 
		    $MODULE$_MAX_REPLY_SIZE, &ESSAY_CID(essayStr)) != OK) {
      (void) h2perror("Client init failed");
      essayEnd(essayStr);
  }
  
  /* S'initialiser comme client des posters */
  printf ("ok.   Poster init ...");
  if ($module$PosterInit () != OK)
    h2perror ("Warning: not found all $module$'s posters");

  printf ("ok.");
}
 
