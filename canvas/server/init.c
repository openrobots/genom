/*	$LAAS$ */

/* 
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

/*------------------  Fichier généré automatiquement ------------------*/
/*------------------  Ne pas éditer manuellement !!! ------------------*/
 
/****************************************************************************
 *   LABORATOIRE D'AUTOMATIQUE ET D'ANALYSE DE SYSTEMES - LAAS / CNRS       
 *   PROJET HILARE II - ROUTINE D'INIT INTERACTIF       
 *   FICHIER SOURCE: $module$Init.c                                          
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
#include "h2sysLibProto.h"
#else
#include "portLib.h"
#include "errnoLib.h"
#include "h2initGlob.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define $MODULE$_PROMPT "usage: $module$SendInitRqst ($inputUsage$)\n"

/* Inclusions pour acceder au module */
#include "$module$MsgLib.h"

/*------------------- PROTOTYPE DES FONCTIONS EXPORTEES -------------------*/

STATUS $module$SendInitRqst ( $inputFlat$ );

/*------------------- PROTOTYPES DES FONCTIONS LOCALES --------------------*/

#if $requestFlag$
static STATUS $module$InitInitTask (CLIENT_ID *clientId, int size);
static STATUS $module$InitEnd (CLIENT_ID clientId);
#endif /* Pas de requete d'init */

/*---------------------------   MAIN  ------------------------------------*/

#ifndef VXWORKS
int
main(int argc, char **argv)
{
$inputFlatDeclare$

  if (--argc != $nbInputParams$) {
    fprintf(stderr, $MODULE$_PROMPT);
    exit(1);
  }

  /* Conversion des parametres */
$inputA2Type$

  /* Appel de la fonction d'init */
  $module$SendInitRqst($inputFlatNamePtrC$);


  return 0;
} /* main */
#else
void 
$module$InitParamsShow(void)
{
  printf ($MODULE$_PROMPT);
}
#endif /* UNIX */


/**
 **   $module$Init - Emission de la requete d'init
 **/

STATUS $module$SendInitRqst ($inputFlat$)
{
#if $requestFlag$
  CLIENT_ID initCId;        /* Id client */
  int bilan=OK;
  int status;
  int rqstId;
  int activity;
  int size=$inputSize$;
  $inputDeclare$

#ifndef VXWORKS
  /* Defini la valeur d'un tic (en nombre de tic par seconde) */
  h2initGlob(20);
#endif

  /* Initialisation de la tache $module$Init */
  if ($module$InitInitTask (&initCId,size) == ERROR)
    return (ERROR);
	  
  /* Instanciation de la structure parametre de la requete */
$inputFillStruct$
  
  /* Affichage de la structure parametre de la requete */
$inputVerif$
  /* Pb: il faut la bibliotheque print.c (possible mais lourd)
     $inputShow$  */

  errnoSet(0);
  
  /* Emettre la requete */
  if ((status = csClientRqstSend (initCId, $requestNum$, 
				  (void *) $inputNamePtr$,
				  size, (FUNCPTR) NULL, 
				  TRUE, 0, 
				  TIME_WAIT_REPLY, &rqstId)) == ERROR) {
    bilan = errnoGet();
    printf("Error (status %d) ", status);
    h2printErrno(bilan);
    return ERROR;
  }
  
  /* Reception de la replique intermediaire */
  status = csClientReplyRcv (initCId, rqstId, 
			     BLOCK_ON_INTERMED_REPLY, 
			     (void *) &activity, sizeof(int), 
			     (FUNCPTR) NULL, 
			     (void *) NULL, 0, 
			     (FUNCPTR) NULL);

  /* On traite les status */
  switch(status) {
    case WAITING_FINAL_REPLY:
      printf ("-- Intermed reply: OK\n");
      if (csClientReplyRcv (initCId, rqstId, BLOCK_ON_FINAL_REPLY, 
			    (void *) NULL, 0, (FUNCPTR) NULL, 
			    (void *) NULL, 0, 
			    (FUNCPTR) NULL) == ERROR) {
          bilan = errnoGet();
          if (H2_MODULE_ERR_FLAG(bilan))
	    printf("-- Final reply: ");
	  else 
	    printf("** Error : ");
      }
      else
	printf("-- Final reply: ");
      break;

    case FINAL_REPLY_OK:
      printf("-- Final reply: ");
      break;

    case FINAL_REPLY_TIMEOUT:
      printf("** Final reply time out: \n");
      break;

    case ERROR:
      if (H2_MODULE_ERR_FLAG(bilan))
	printf("-- Final reply: ");
      else 
	printf("** Error : ");
      break;

    default:
      printf("** Error (status %d) ", status);
      break;
    } /* switch */
  bilan = errnoGet();
  h2printErrno(bilan);

  /* On netoie */
  $module$InitEnd(initCId);

  if (status != FINAL_REPLY_OK || bilan != OK)
    return ERROR;

  return OK;

#else  /* Pas de requete d'init */

  printf ("There is no init request\n");
  return OK;

#endif

}



/*----------------------- ROUTINES LOCALES ---------------------------------*/


#if $requestFlag$

/*****************************************************************************
 *
 *  $module$InitInitTask - Routine d'initialisation de la tache d'essai
 *
 *  Description:
 *
 *  Retourne : OK ou ERROR
 */

static STATUS $module$InitInitTask (CLIENT_ID *clientId, int size)
     
{
  char mboxName[30];                /* Nom de base de la boite aux lettres */

  /* Copier le nom de base des mboxes */
  (void) strcpy (mboxName, "t$module$Init");
    
  /* Initialisation des boites aux lettres */
  if (csMboxInit (mboxName, 0, $MODULE$_CLIENT_MBOX_REPLY_SIZE) != OK) {
    h2perror("$module$Init: csMboxInit failed\n");
    return (ERROR);
  }


  /* S'initialiser comme client */
  if (csClientInit($MODULE$_MBOX_NAME, 
		   size,
		   sizeof(int),
		   sizeof(int),
		   clientId) != OK) {
      h2perror("$module$Init: $module$ClientInit failed\n");
      csMboxEnd();
      return (ERROR);
    }

  /* Initialisation OK */
  return (OK);
}
 
 
/*****************************************************************************
 *
 *  $module$InitEnd  -  Termine proprement la tache
 *
 *  Description:
  *  Cette fonction termine les taches en-cours et desalloue les objets.
 *
 *  Retourne: OK ou ERROR
 */

static STATUS $module$InitEnd (CLIENT_ID initCId)

{
  /* Desalouer les objets */
  if (csClientEnd (initCId) != OK || csMboxEnd () != OK ) {
    printf ("$module$Init: pb liberation des objets alloue's\n");
    return (ERROR);
  }
  
  /* Bravo, c'est fini */
  return (OK);
}


#endif /* Pas de requete d'init */
