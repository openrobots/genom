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

/*                               -*- Mode: C -*- 
 * spyServerLib.c -- 
 * 
 * Author          : Laurent Goncalves
 * Created On      : Wed Oct  6 13:39:31 1999
 * Last Modified By: Francois Felix Ingrand
 * Last Modified On: Thu Oct  7 11:56:09 1999
 * Update Count    : 4
 * Status          : OK
 */

#include "spyServerLib.h"

/********* Fichier synthetise automatiquement, ne pas modifier.  ********/

/********* ce fichier requiert l'appel a un fichier de configuration  ********/
/********* selon la machine qui fournira la variable BYTE_ORDER       ********/

/*****************************************************************************/
int handle_client_message(int sock)
{
     int etat;
     unsigned char * ud=NULL;
     char * name;
     int i;

     etat=sock_read(sock,&ud);
     if(etat<=0) return(etat);
#ifdef DEBUG_MODE
     printf("demande recue:%d\n",ud[0]);
#endif
     switch(ud[0])
     {
     case ASK_STRUCT_DESCRIPTION:
	  name=get_string_from_message_with_malloc(&ud[1]);
	  etat=$module$_send_struct_description(sock,name);
	  free(name);
	  break;
     case ASK_POSTER_DESCRIPTION:
	  etat=$module$_send_poster_struct(sock);
	  break;
     case ASK_POSTER_DATA:
	  name=get_string_from_message_with_malloc(&ud[1]);
#ifdef DEBUG_MODE
	  printf("demande donnees pour poster %s\n",name);
#endif
	  etat=$module$_send_poster_data(sock,name);
	  free(name);
	  break;
     case ASK_REQUESTS:
	  etat=$module$_send_requests_description(sock);
	  break;
     case ASK_TASKS:
	  etat=$module$_send_tasks_description(sock);
	  break;
     case ASK_REFRESH_FOR_POSTER:
	  insert_event_from_ud(sock,ud);
	  break;
     case STOP_REFRESH_FOR_POSTER:
	  delete_event_from_ud(sock,ud);
	  break;
     case CLOSE_CONNECTION:
	  etat=-1;
	  break;
     default :
	  fprintf(stderr,"Unknown message from socket %d",sock);
	  send_error_message(sock,"Unknown message : The client has executed an unknown command.");
	  etat=0;
	  break;
     }
     free(ud);
#ifdef DEBUG_MODE
     printf("Fin du traitement de la requete client avec code retour :%d\n",etat);
#endif
     return(etat);
}
/*****************************************************************************/
int handle_event(int nb_event_to_force)
{
     struct timeval now;
     int bcle=1;
     int etat;
     evenement * courant=queue;

#ifdef DEBUG_MODE
     printf("Etat de la queue:\n");
     while(courant!=NULL)
     {
	  printf("rafraichissement sur le poster %s cadence a %d ms\n",courant->poster,courant->rate);
	  courant=courant->suiv;
     }
     courant=queue;
#endif
     if(queue==NULL)
	  return(0);/* retourne 0 quand la queue est vide */
     gettimeofday(&now,NULL);
     while(queue!=NULL & bcle)
     {
	  if(spyTimecmp(queue->next_launch,now)<=0 || nb_event_to_force>0)
	  {/* on doit traiter l'evenement */
	       etat=$module$_send_poster_data(queue->sockfd,queue->poster);
#ifdef DEBUG_MODE
	       printf("emmission de %s, code retour:%d\n",queue->poster,etat);
#endif
	       nb_event_to_force --;
	       courant=queue;
	       queue=queue->suiv;/*quoiqu'il en soit, on traite l'evenement */
	       if(etat>=0)
		    /* si ca s'est bien passe on prepare le futur appel */
		    insert_event(courant->sockfd,courant->rate,courant->poster);
	       free(courant->poster);
	       free(courant);
	  }
	  else
	       bcle=0;
     }
     return(1);/* retourne 1 si on a traiter au moins 1 evenement et donc si la queue n'est pas vide */
}
