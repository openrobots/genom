/*	$LAAS$ */

/* 
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

/* --- Message-based communication for module $module$ --------------- */

#ifdef VXWORKS
# include <vxWorks.h>
#else
# include <portLib.h>
#endif

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/kernel.h>
#else
# include <stdio.h>
# include <stddef.h>
#endif

#include <errnoLib.h>
#include "$module$MsgLib.h"
#include "$module$Error.h"
 
/*========================================================================
 *
 *  $module$ClientInit  -  Routine d'initialisation d'un client de $module$
 *
 *  Description:
 *  Cette routine  doit etre appelee par un client du module, pour pouvoir 
 *  l'utiliser. Attention: la fonction csMboxInit doit etre appelee avant.
 *
 *  Retourne: OK ou ERROR
 */
 
STATUS $module$ClientInit (CLIENT_ID *pClientId)

{
  STATUS status;

  /* Appeler la routine d'initialisation d'un client CS */
  status = csClientInit ($MODULE$_MBOX_NAME, $MODULE$_MAX_RQST_SIZE,
			 $MODULE$_MAX_INTERMED_REPLY_SIZE, 
			 $MODULE$_MAX_REPLY_SIZE, 
			pClientId);
  if (status != OK) 
    h2perror("$module$ClientInit");

  $module$RecordH2errMsgs();

  return status;
}
 
/*-------------------------------------------------------------------------
 *
 *  $module$ClientEnd - Routine de fin d'un client de $module$
 *
 *  Description:
 *  Cette routine libere les objets alloues par le client.
 *
 *  Retourne : OK ou ERROR
 */

STATUS $module$ClientEnd (CLIENT_ID clientId)         /* Id. du client */
     
{
  STATUS status;
  /* Appeler la routine de liberation du client */
  status = csClientEnd (clientId);
  if (status != OK) 
    h2perror("$module$ClientEnd");
  return status;
}


/*========================================================================
 *
 *  $module$AbortRqstSend  -  Emission d'une requete de controle
 *
 *  Description: Arret de l'activite specifie'e en parametre
 *
 *  Retourne : OK ou ERROR
 */
 
int $module$AbortRqstSend (CLIENT_ID clientId, 
			   int *pRqstId,
			   int *activity,
			   int replyTimeOut)
{
  errnoSet(0);

  /* Emettre la requete */
  if (csClientRqstSend (clientId, $abortRequestNum$, (void *) activity,
			sizeof(int), (FUNCPTR) NULL, FALSE, 0, replyTimeOut, 
			pRqstId) == ERROR)
    return(ERROR);

  return(OK);
}

/*-------------------------------------------------------------------------
 *
 *  $module$AbortReplyRcv  -  Reception de la replique finale
 *
 *  Description:
 *
 *  Retourne : ERROR ou FINAL_REPLY_TIMEOUT ou FINAL_REPLY_OK ou
 *             WAITING_FINAL_REPLY
 */
 
int $module$AbortReplyRcv (CLIENT_ID clientId, 
			   int rqstId, 
			   int block,  /* NO_BLOCK BLOCK_ON_FINAL_REPLY */
			   int *bilan)
{
  int status;    /* ERROR ou FINAL_REPLY_TIMEOUT ou FINAL_REPLY_OK */

  /* Verifier le flag de type de blocage */
  if (block != NO_BLOCK && block != BLOCK_ON_FINAL_REPLY) {
    errnoSet (S_$module$_stdGenom_BAD_BLOCK_TYPE);
    return (ERROR);
  }
  
  status = csClientReplyRcv (clientId, rqstId, block, (void *) NULL, 0,  
			     (FUNCPTR) NULL, (void *) NULL,
			     0, (FUNCPTR) NULL);
  if (status == ERROR) {
    *bilan = errnoGet();

    /* Filtrage type d'erreur */
    if (H2_MODULE_ERR_FLAG(*bilan))
      return(FINAL_REPLY_OK);
  }
  
  return(status);
}
 
/*-------------------------------------------------------------------------
 *
 *  $module$AbortRqstAndRcv  -  Requete de controle
 *
 *  Description: Arret de l'activite specifie'e en parametre
 *  Cette fonction emet une requete et attend la replique finale.
 *
 *  Retourne : ERROR ou FINAL_REPLY_TIMEOUT ou FINAL_REPLY_OK
 */
 
int $module$AbortRqstAndRcv (CLIENT_ID clientId, 
			     int *activity,
			     int *bilan)

{
  int rqstId;                  /* Ou` mettre l'id de la requete */
  int status;
  
  errnoSet(0);
  *bilan = OK;
  
  /* Emettre la requete */
  if (csClientRqstSend (clientId, $abortRequestNum$, (void *) activity,
			sizeof(int), (FUNCPTR) NULL, FALSE, 0, 
			TIME_WAIT_REPLY, 
			&rqstId) == ERROR) {
    *bilan = errnoGet();
    return(ERROR);
  }
  
  /* Reception de la replique */
  status = csClientReplyRcv (clientId, rqstId, BLOCK_ON_FINAL_REPLY, 
			     (void *) NULL, 0, (FUNCPTR) NULL, 
			     (void *) NULL, 0, 
			     (FUNCPTR) NULL);
  if (status == ERROR) {
    *bilan = errnoGet();
    
    /* Filtrage type d'erreur */
    if (H2_MODULE_ERR_FLAG(*bilan))
      return(FINAL_REPLY_OK);
  }
  
  return(status);
}


/* liste des requetes/repliques de controle : msgCntrl.c */

/* liste des requetes/repliques d'execution : msgExec.c */



