/*	$LAAS$ */

/* 
 * Copyright (c) 1999-2003 LAAS/CNRS
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
__RCSID("$LAAS$");

#include <stdio.h>
#include <stdlib.h>
#include <strings.h> 
#include <ctype.h>
#include "genom.h"
#include "parser.tab.h"

static char * posterExecTaskNameTab(void);
static char * posterRequestNameTab(int *nbRqst);

/*****************************************************************************/
int spyStructGen(FILE *out)
{
     int etat;
     int nbRqsts;
     int nbTasks;
     char * ptstr=NULL;
     char * dumpstr=NULL;
     char * proto=NULL;
     char * type;
     char * var;
     char * posterUpper;
     POSTER_LIST *p;
     TYPE_LIST *l;
     TYPE_STR *t;
     DCL_NOM_LIST *m;
     EXEC_TASK_LIST * task_list;

     script_open(out);
     cat_begin(out);

     /***
      *** Ge'ne'ration de l'entete de MODULESpyStructProto.h
      ***/
     bufcat(&proto,
	    "#ifndef _%sSpyStructProto\n"
	    "#define _%sSpyStructProto\n\n",
	    module->name,
	    module->name);

     /***
      *** Ge'ne'ration des includes
      ***/
     fprintf(out,
	     "#include <malloc.h>\n"
	     "#include <stdio.h>\n"
	     "#include \"portLib.h\"\n"

	     "#include \"%sPosterLib.h\"\n"
	     "#include \"%sSpyStructProto.h\"\n"
	     "#include \"spyDialog.h\"\n",

	     module->name,
	     module->name);

     /***
      *** Ge'ne'ration de ExecRqstNameTab
      ***/
     ptstr=posterRequestNameTab(&nbRqsts);
     fprintf(out,ptstr);
     fprintf(out,"\n");
     ptstr = posterExecTaskNameTab();
     fprintf(out,ptstr);
     fprintf(out,"\n\n");

     /***
      *** Ge'ne'ration de tous les send_struct_description
      *** et de tous les dump_STRUCT_to_message
      ***/
	  

     for (l = types; l != NULL; l = l->next)
     {
	  t = l->type;
	  if (t->used == 0 || (t->flags & TYPE_IMPORT) || (t->flags & TYPE_INTERNAL_DATA))
	  {
	       continue;
	  }
	  if(t->type==STRUCT || t->type==UNION)
	  {/* Entete de la fonction */
	       fprintf(out,
		       "int send_struct_description_%s(int sock, char * poster_name, unsigned char tab)\n"
		       "{\n"
		       " int etat;\n"
		       " etat=send_start_struct_description(sock,poster_name);\n"
		       " if(etat<0) return(etat);\n",
		       t->name);
	       /* entete fonction dump_STRUCT */
	       bufcat(&dumpstr,
		      "void dump_%s_to_message(%s x)\n"
		      "{\n",
		      t->name,
		      t->name);
	       /* enregistrement dans les proto */
	       bufcat(&proto,
		      "int send_struct_description_%s(int sock, char * poster_name, unsigned char tab);\n"
		      "void dump_%s_to_message(%s x);\n",
		      t->name,
		      t->name,
		      t->name);
	       /* parcours des variables internes a cette structure */
	       for(m=t->members;m!=NULL;m=m->next)
	       {
		    dcl_nom_decl(m->dcl_nom, &type, &var);
		    switch(m->dcl_nom->type->type)
		    {
		    case CHAR:
			 fprintf(out,
				 " etat=send_data_description(sock,\"%s\",SPY_CHAR,1);\n"
				 " if(etat<0) return(etat);\n",
				 var);
			 bufcat(&dumpstr,
				" dump_char_to_message(x.%s);\n",
				var);
			 break;
		    case SHORT:
			 fprintf(out,
				 " etat=send_data_description(sock,\"%s\",SPY_SHORT,1);\n"
				 " if(etat<0) return(etat);\n",
				 var);
			 bufcat(&dumpstr,
				" dump_short_to_message(x.%s);\n",
				var);
			 break;
		    case INT:
			 fprintf(out,
				 " etat=send_data_description(sock,\"%s\",SPY_INT,1);\n"
				 " if(etat<0) return(etat);\n",
				 var);
			 bufcat(&dumpstr,
				" dump_int_to_message(x.%s);\n",
				var);
			 break;
		    case FLOAT:
			 fprintf(out,
				 " etat=send_data_description(sock,\"%s\",SPY_FLOAT,1);\n"
				 " if(etat<0) return(etat);\n",
				 var);
			 bufcat(&dumpstr,
				" dump_float_to_message(x.%s);\n",
				var);
			 break;
		    case ENUM:
			 etat=-1;
			 break;
		    case DOUBLE:
			 fprintf(out,
				 " etat=send_data_description(sock,\"%s\",SPY_DOUBLE,1);\n"
				 " if(etat<0) return(etat);\n",
				 var);
			 bufcat(&dumpstr,
				" dump_double_to_message(x.%s);\n",
				var);
			 break;
		    case UNION:
		    case STRUCT:
			 fprintf(out,
				 " etat=send_struct_description_%s(sock,\"%s\",1);\n"
				 " if(etat<0) return(etat);\n",
				 type,
				 var);
			 bufcat(&dumpstr,
				" dump_%s_to_message(x.%s);\n",
				type,
				var);
			 break;
		    case TYPEDEF:
		    default:
			 fprintf(stderr, "Error: taille_obj: type inconnu %s\n", t->name);
		
		    }
	       }/* for(...) */
	       fprintf(out," etat=send_end_struct_description(sock);\n");
	       fprintf(out," if(etat<0) return(etat);\n");
	       fprintf(out," return(flush_dump(sock));\n}\n\n");
	       bufcat(&dumpstr,"}\n\n");

	  }

     }


     /***
      *** Ge'ne'ration de tous les send_poster_data
      ***/
     for(p=posters;p!=NULL;p=p->next)
     {
	  posterUpper=(char *)malloc(sizeof(char)*(strlen(p->name)+1));
	  strcpy(posterUpper,p->name);
	  posterUpper[0]=toupper(posterUpper[0]);
	  bufcat(&proto,"int send_poster_data_%s_%s(int sock);\n",module->NAME,p->NAME);
	  fprintf(out,
		  "int send_poster_data_%s_%s(int sock)\n"
		  "{\n"
		  " POSTER_ID poster_id;\n"
		  " %s * poster;\n"
		  " STATUS retour;\n"
		  " int etat;\n",
		  module->NAME,
		  p->NAME,
		  p->type->name);
	  fprintf(out,
		  " retour=posterFind(\"%s%s\",&poster_id);\n"
		  " if(retour!=OK)\n"
		  " {\n"
		  "  fprintf(stderr,\"Poster %s%s introuvable !\\n\");\n"
		  "  send_error_message(sock,\"Poster %s%s introuvable !\");\n"
		  "  return(-1);\n"
		  " }\n\n",
		  module->name,
		  posterUpper,
		  module->name,
		  posterUpper,
		  module->name,
		  posterUpper);
	  fprintf(out,
		  " poster=(%s *)malloc(sizeof(%s));\n"
		  " etat=posterRead(poster_id,0,poster,sizeof(%s));\n",
		  p->type->name,
		  p->type->name,
		  p->type->name);

	  fprintf(out,
		  " if(etat!=sizeof(%s))\n"
		  " {\n"
		  "  fprintf(stderr,\"Poster %s%s illisible !\\n\");\n"
		  "  send_error_message(sock,\"Poster %s%s introuvable !\");\n"
		  "  return(-1);\n"
		  " }\n\n",
		  p->type->name,
		  module->name,
		  posterUpper,
		  module->name,
		  posterUpper);
		
	  fprintf(out,
		  " dump_uchar_to_message(START_POSTER_DATA);\n"
		  " dump_uchar_to_message(strlen(\"%s\"));\n"
		  " dump_string_to_message(\"%s\");\n"
		  " dump_int_to_message(sizeof(%s));\n",
		  posterUpper,
		  posterUpper,
		  p->type->name);
	  fprintf(out,
		  " dump_%s_to_message(*poster);\n",
		  p->type->name);
	  fprintf(out," etat=flush_message_and_dump(sock);\n"
		  " free(poster);\n"
		  " return(etat);\n}\n\n");
	  free(posterUpper);
     }

     /***
      *** Ge'ne'ration de module_send_requests_description
      ***/
     /* entete de la fonction */
     bufcat(&proto,"int %s_send_requests_description(int sock);\n",module->name);
     fprintf(out,
	     "int %s_send_requests_description(int sock)\n"
	     "{\n"
	     " int i;\n\n",
	     module->name);
     fprintf(out,
	     " dump_uchar_to_message(START_REQUESTS_DESCRIPTION);\n"
	     " dump_int_to_message(%d);\n",
	     nbRqsts);
     fprintf(out,
	     " for(i=0;i<%d;i++)\n"
	     " {\n"
	     "  dump_int_to_message(%sExecRqstNumTab[i]);\n"
	     "  dump_uchar_to_message(strlen(%sExecRqstNameTab[i]));\n"
	     "  dump_string_to_message(%sExecRqstNameTab[i]);\n"
	     " }\n",
	     nbRqsts,
	     module->name,
	     module->name,
	     module->name);
     fprintf(out," return(flush_message_and_dump(sock));\n}\n\n");


     /***
      *** Ge'ne'ration de module_send_tasks_description
      ***/
     /* comptage des taches */
     task_list=taches;
     nbTasks=0;
     while(task_list!=NULL)
     {
	  nbTasks++;
	  task_list=task_list->next;
     }
     /* entete de la fonction */
     bufcat(&proto,"int  %s_send_tasks_description(int sock);\n",module->name);
     fprintf(out,
	     "int  %s_send_tasks_description(int sock)\n"
	     "{\n"
	     " int i;\n"
	     " dump_uchar_to_message(START_TASKS_DESCRIPTION);\n"
	     " dump_int_to_message(%d);\n",
	     module->name,
	     nbTasks);
     fprintf(out,
	     " for(i=0;i<%d;i++)\n"
	     " {\n"
	     "  dump_uchar_to_message(strlen(%sExecTaskNameTab[i]));\n"
	     "  dump_string_to_message(%sExecTaskNameTab[i]);\n"
	     " }\n",
	     nbTasks,
	     module->name,
	     module->name);
     fprintf(out," return(flush_message_and_dump(sock));\n}\n");

     /***
      *** ecriture des dump_STRUCT_to_message
      ***/
     fprintf(out,dumpstr);

     cat_end(out);
     script_close(out,"spy/%sSpyStruct.c",module->name);
     free(dumpstr);

     bufcat(&proto,"\n#endif\n");
     script_open(out);
     cat_begin(out);
     fprintf(out,proto);
     cat_end(out);
     script_close(out,"spy/%sSpyStructProto.h",module->name);
     free(proto);
     return(0);
}

/*****************************************************************************/
/***** recupere' dans posterLibGen.c tel quel *****/
static char * posterExecTaskNameTab(void) 
{
     EXEC_TASK_LIST *lt;
     EXEC_TASK_STR *t;
     char *msg = NULL;

     bufcat(&msg, "\nstatic char *%sExecTaskNameTab[] = {\n",
	    module->name); 

     for (lt = taches; lt != NULL; lt = lt->next) {
	  t = lt->exec_task;
	  bufcat(&msg, "\"%s\"", t->name);
	  if (lt->next != NULL) {
	       bufcat(&msg, ",\n");
	  }
     } /* for */
     bufcat(&msg, "\n};");
     return(msg);
}
/*****************************************************************************/
/***** recupere' dans posterLibGen.c tel quel *****/
static char * posterRequestNameTab(int *nbRqsts) 
{
     RQST_LIST *l;
     RQST_STR *r;
     char *msg = NULL;
     int nb=0;

     bufcat(&msg, "\nstatic char *%sExecRqstNameTab[] = {\n",
	    module->name); 

     for (l = requetes; l != NULL; l = l->next) {
	  r = l->rqst;
	  if(r->type == EXEC || r->type == INIT) {
	       bufcat(&msg, "\"%s\"", r->name);
	       if (l->next != NULL)
		    bufcat(&msg, ",\n");
	  }
     } /* for */
     bufcat(&msg, "\n};");

     bufcat(&msg, "\nstatic int %sExecRqstNumTab[] = {\n",
	    module->name);
     for (l = requetes; l != NULL; l = l->next) {
	  r = l->rqst;
	  if(r->type == EXEC || r->type == INIT) {
	       nb++;
	       bufcat(&msg, "%d", r->rqst_num);
	       if (l->next != NULL)
		    bufcat(&msg, ", ");
	  }
     }
     bufcat(&msg, "};\n");
    
     *nbRqsts = nb;
     return(msg);
}

