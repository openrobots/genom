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

/* creation du fichier $module$Description.c */

/***
 *** Ge'ne'ration de la description du module
 ***/


#include <stdio.h>
#include <stdlib.h>
#include "genom.h"
#include "parser.tab.h"

int spyDescriptionGen(FILE *out)
{
     POSTER_LIST *p;
     STR_REF_LIST * l;
     STR_REF_STR * m;
     DCL_NOM_STR * n;


     script_open(out);
     cat_begin(out);
     /***
      *** Ge'ne'ration des includes
      ***/
     fprintf(out,
	     "#include \"spyDialog.h\"\n"
	     "#include \"portLib.h\"\n"
	     "#include \"posterLib.h\"\n"
	     "#include \"%sPosterLib.h\"\n"
	     "#include \"%sSpyStructProto.h\"\n",
	     module->name, module->name);

     /***
      *** Ge'ne'ration de send_struct_description
      ***/
     fprintf(out,
	     "int %s_send_struct_description(int sock,char * struct_name)\n{\n"
	     " int etat=-1;\n\n",
	     module->name);
     for(p=posters;p!=NULL;p=p->next)
     {
	  fprintf(out,
		  " if(strcmp(struct_name,\"%s\")==0)\n"
		  " {\n"
		  "  etat=send_struct_description_%s(sock,\"%s\",1);\n"
		  "  if(etat<0) return(etat);\n"
		  " }\n",
		  p->name,
		  p->type->name,
		  p->name);
	  for(l=p->data;l!=NULL;l=l->next)
	  {
	       m=l->str_ref;
	       n=m->dcl_nom;
	       if(n->type->type==STRUCT || n->type->type==UNION)
	       {
		    fprintf(out,
			    " if(strcmp(struct_name,\"%s\")==0)\n"
			    " {\n"
			    "  etat=send_struct_description_%s(sock,\"%s\",1);\n"
			    "  if(etat<0) return(etat);\n"
			    " }\n",
			    n->name,
			    n->type->name,
			    n->name);
	       }
	  }
     }
     fprintf(out," return(etat);\n}\n\n");
  
     /***
      *** Ge'ne'ration de send_poster_struct
      ***/
     fprintf(out,
	     "int %s_send_poster_struct(int sock, char * struct_name)\n"
	     "{\n"
	     " int etat;\n",
	     module->name);
     for(p=posters;p!=NULL;p=p->next)
     {
	  fprintf(out,
		  "  etat=send_struct_description_%s(sock,\"%s\",1);\n"
		  "  if(etat<0) return(etat);\n",
		  p->type->name,
		  p->name);
     }
     fprintf(out,"return(etat);\n}\n\n");

     /***
      *** Ge'ne'ration de send_poster_data
      ***/
     fprintf(out,
	     "int %s_send_poster_data(int sock, char * poster_name)\n"
	     "{\n"
	     " int etat=-1;\n",
	     module->name);
     for(p=posters;p!=NULL;p=p->next)
     {
	  fprintf(out,
		  " if(strcmp(poster_name,\"%s\")==0)\n"
		  " {\n"
		  "  etat=send_poster_data_%s_%s(sock);\n"
		  "  if(etat<0) return(etat);\n"
		  " }\n",
		  p->name,
		  module->NAME,
		  p->NAME);
     }
     fprintf(out,
	     " if(strcmp(poster_name,\"CntrlPoster\")==0)\n"
	     " etat=send_control_posters(sock,\"%sCntrlPoster\");\n\n",
	     module->name);
     fprintf(out,
	     " if(etat==0) return(1);\n"
	     " else return(etat);\n"
	     "}\n");


     cat_end(out);
     script_close(out,"spy/%sDescription.c",module->name);
     return(0);
}

