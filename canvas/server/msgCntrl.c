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

/*=========================================================================
 *
 *  $module$$request$RqstSend  -  Emission d'une requete de controle
 *
 *  Retourne : OK ou ERROR
 */
 
STATUS $module$$request$RqstSend (CLIENT_ID clientId, 
				  int *pRqstId,
				  $input$
				  int replyTimeOut)
{
  errnoSet(0);

  /* Emettre la requete */
  if (csClientRqstSend (clientId, $requestNum$, (void *) $inputName$,
			$inputSize$, (FUNCPTR) NULL, FALSE, 0, replyTimeOut, 
			pRqstId) == ERROR)
    return ERROR;
  return OK;
}

/*-------------------------------------------------------------------------
 *  $module$$request$ReplyRcv  -  Reception de la replique finale
 *
 *  Retourne : ERROR ou FINAL_REPLY_TIMEOUT ou FINAL_REPLY_OK ou
 *             WAITING_FINAL_REPLY
 */
 
int $module$$request$ReplyRcv (CLIENT_ID clientId, 
			       int rqstId, 
			       int block,  /* NO_BLOCK BLOCK_ON_FINAL_REPLY */
			       $output$
			       int *bilan)
{
  int status;    /* ERROR ou FINAL_REPLY_TIMEOUT ou FINAL_REPLY_OK */

  /* Verifier le flag de type de blocage */
  if (block != NO_BLOCK && block != BLOCK_ON_FINAL_REPLY) {
    errnoSet (S_$module$MsgLib_BAD_BLOCK_TYPE);
    return (ERROR);
  }
  
  if ((status = csClientReplyRcv (clientId, rqstId, block, (void *) NULL, 0,  
				  (FUNCPTR) NULL, (void *) $outputName$,
				  $outputSize$, (FUNCPTR) NULL)) == ERROR) {
    *bilan = errnoGet();
    if (H2_MODULE_ERR_FLAG(*bilan)) return(FINAL_REPLY_OK);
  }
  else *bilan = OK;
  
  return(status);
}
 
/*-------------------------------------------------------------------------
 *  $module$$request$RqstAndRcv  -  Emission/reception requete de controle
 *
 *  Retourne : ERROR ou FINAL_REPLY_TIMEOUT ou FINAL_REPLY_OK
 */
 
int $module$$request$RqstAndRcv (CLIENT_ID clientId, 
				 $input$ 
				 $output$
				 int *bilan)
{
  int rqstId;                  /* Ou` mettre l'id de la requete */
  int status;
  
  errnoSet(0);
  
  /* Emettre la requete */
  if (csClientRqstSend (clientId, $requestNum$, (void *) $inputName$,
			$inputSize$, (FUNCPTR) NULL, FALSE, 0, 
			TIME_WAIT_REPLY, &rqstId) == ERROR) {
    *bilan = errnoGet();
    return(ERROR);
  }
  
  /* Reception de la replique */
  if ((status = csClientReplyRcv (clientId, rqstId, BLOCK_ON_FINAL_REPLY, 
				  (void *) NULL, 0, (FUNCPTR) NULL, 
				  (void *) $outputName$, $outputSize$, 
				  (FUNCPTR) NULL)) == ERROR) {
    *bilan = errnoGet();
    if (H2_MODULE_ERR_FLAG(*bilan)) return(FINAL_REPLY_OK);
  }
  else *bilan = OK;
  
  return(status);
}


