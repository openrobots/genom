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

#ifndef $module$_MSG_LIB_H
#define $module$_MSG_LIB_H

#include "portLib.h"
#include "csLib.h"
#include "posterLib.h"
 
#include "$module$Type.h"

#define TIME_WAIT_REPLY           /*2000*/ 0
 

/*---------------- PROTOTYPES DES FONCTIONS EXTERNES ------------------*/

#include "$module$MsgLibProto.h"


/*----------- Tailles des messages et boites aux lettres  ----------*/

/* Taille du buffer utilise dans la bal pour un mesage de taille size
 * (cf servMbox pour UNIX et h2rgnLib pour VXWORKS )
 */


#define BUF_SIZE(size) \
   (size + 4 - (size & 3) + sizeof(LETTER_HDR) + 8)

/**
 ** Boite aux lettres reception requetes 
 **/
#define  $MODULE$_MBOX_NAME                      "$module$"

#define  $MODULE$_MAX_RQST_SIZE              $maxRequestSize$

#define  $MODULE$_MBOX_RQST_SIZE       \
 (BUF_SIZE($MODULE$_MAX_RQST_SIZE) * SERV_NMAX_RQST_ID)

/** 
 ** Taille max repliques (serveur)
 **/
#define  $MODULE$_MAX_INTERMED_REPLY_SIZE   (sizeof(int))

#define  $MODULE$_MAX_REPLY_SIZE             $maxReplySize$

/** 
 ** Taille bal clients pour reception repliques 
 **/
#define  $MODULE$_CLIENT_MBOX_REPLY_SIZE   \
((BUF_SIZE($MODULE$_MAX_REPLY_SIZE) \
   + BUF_SIZE($MODULE$_MAX_INTERMED_REPLY_SIZE)) \
 * CLIENT_NMAX_RQST_ID)

/*-------------------------- Liste des requetes  -------------------------*/

$listRequests$

/* Fin de chargement du fichier */
#endif
