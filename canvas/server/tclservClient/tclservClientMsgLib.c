
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

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/kernel.h>
#else
# include <stdio.h>
# include <stddef.h>
# include <stdlib.h>
# include <sys/types.h>
#endif

#include <tclserv_client/buf.h>
#include <tclserv_client/tclserv_client.h>

#include "$module$TclservClientDecode.h" 
#include "$module$TclservClientEncode.h" 
#include "$module$TclservClientMsgLib.h"
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
 
int $module$TclservClientInit (TCLSERV_CLIENT_ID clientId)

{
  int status;

  /* Appeler la routine d'initialisation d'un client CS */
  status = tclserv_client_load_module(clientId , "$module$"); 

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

int $module$TclservClientEnd (TCLSERV_CLIENT_ID clientId)         /* Id. du client */
     
{
  return tclserv_client_unload_module(clientId, "$module$");
}


/*========================================================================
 *
 *  $module$AbortRqst : abort a request
 */
 
int $module$AbortRqst (TCLSERV_CLIENT_ID clientId, 
		ssize_t rqstId)
{
  return tclserv_client_abort(clientId, rqstId);
}

/* liste des requetes/repliques de controle : msgCntrl.c */

/* liste des requetes/repliques d'execution : msgExec.c */



