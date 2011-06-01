
/* 
 * Copyright (c) 1994-2004 LAAS/CNRS
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
#include "genom-config.h"

/**
 ** Sara Fleury 
 **
 ** Obtention de la chaine de caracteres associe a un etat ou un evenement
 **
 **/

#include <portLib.h>

#if defined(__RTAI__) && defined(__KERNEL__)
# include <linux/kernel.h>
#else
# include <stdio.h>
#endif

#include "genom/modules.h"
#include "genom/printStateProto.h"

static char evnStateString[32];

char *genomActivityStateStr[] = {
  "ETHER",                    /* pas d'activite */
  "INIT",                     /* en attente de demarrage */
  "START",                    /* demande/execution du start */
  "EXEC",                     /* demande/execution */
  "END",                      /* demande/execution du end */
  "FAIL",                     /* demande/execution du fail (end) */
  "INTER",                    /* demande/execution de l'interuption (end) */
  "ZOMBIE",                   /* suspendue apres echec */
  "NO_EVENT",                 /* pas d'evenement */
  "SLEEP"                     /* en attente d'un evenement externe */
};


/*****************************************************************************
*
*  h2GetEvnStateString - Affiche la chaine correspondante au nom de l'evn
*
*  Description :
*               
*
*  Retourne: la chaine
*/
char const * h2GetEvnStateString(int num)
{
  if (num < 0 || num >= GENOM_NB_ACTIVITY_STATE)
    sprintf(evnStateString, "Unknown state %d", num);

  else
    sprintf(evnStateString, genomActivityStateStr[num]);

  return(evnStateString);
}

