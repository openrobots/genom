/*	$LAAS$ */

/* 
 * Copyright (c) 1993-2006 LAAS/CNRS
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

/***
 *** Modules generation (GenoM)
 ***
 *** Syntaxic analysis
 ***
 *** Matthieu Herrb & Sara Fleury -- Juillet 1993
 ***/

%{
#include "genom-config.h"
__RCSID("$LAAS$");

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <errno.h>

#include "extern_processes.h"

#include "genom.h"
#include "typeGen.h"
#include "errorGen.h"
#include "msgLibGen.h"
#include "printGen.h"
#include "printXMLGen.h"
#include "endianGen.h"
#include "scanGen.h"
#include "scopeGen.h"
#include "posterLibGen.h"
#include "headerGen.h"
#include "cntrlTaskGen.h"
#include "execTaskGen.h"
#include "moduleInitGen.h"
#include "testGen.h"
#include "initGen.h"

#include "openprsGen.h"

#include "tclGen.h"
#include "tclClientGen.h"

#include "userCntrlCodelsGen.h"
#include "userExecCodelsGen.h"
#include "userStartGen.h"

#include "configureGen.h"

/* compile time configuration */
const char *includeDir = INCLUDEDIR;
const char *libraryDir = LIBRARYDIR;
const char *protoDir = CANVASDIR;

extern int yylex(void);

#define YYDEBUG 1
#define YYERROR_VERBOSE

#ifdef LEXDEBUG
#  define DEBUG(x) printf x
#else
#  define DEBUG(x)
#endif

#define MAXDIM 10		/* nombre max de dimensions d'un tableau */

int keyword = 1;
int num_ligne = 1;
char nomfic[MAXPATHLEN];        /* nom du fichier en cours de traitement */
char *genfile;
FILE *sortie;
int enumValue = 0;

static int import_flag = 0;

static void yyerror(char *s);

#ifndef GENOM_VERSION
# error GENOM_VERSION not defined!
#endif
const char genomVersion[] = GENOM_VERSION;

MODULE_STR *module;
RQST_LIST *requetes = NULL;
int nbRequest = 0;
int initRequest = 0;
int maxRqstSize = 0, maxReplySize = 0;
EXEC_TASK_LIST *taches = NULL;
int nbExecTask = 0;
DCL_NOM_LIST *typedefs = NULL;
TYPE_LIST *types = NULL;
POSTER_LIST *posters = NULL;
ID_LIST *includeFiles = NULL; /* Fichiers inclus dans .gen qui contiennent des definitions de type */
ID_LIST *allIncludeFiles = NULL; /* Tous les fichiers inclus (trouves par cpp) */
ID_LIST *imports = NULL;
ID_LIST *requires = NULL;
ID_LIST *codels_requires = NULL;
ID_LIST *externPath = NULL;

char *cppOptions[MAX_CPP_OPT];
int nCppOptions = 0;

%}

/*----------------------------------------------------------------------*/

/**
 ** The grammar
 **/

%start liste_declaration_top

%union {
    int                  ival;
    double               dval;
    LIBTOOL_VERSION     *libtool_version;
    ID_LIST 		*idList;
    ID_STR 		*idStr;
    RQST_STR 		*rqstStr;
    RQST_AV_STR 	*rqstAV;
    RQST_LIST 		*rqstList;
    MODULE_STR 		*moduleStr;
    MODULE_AV_STR 	*moduleAV;
    EXEC_TASK_STR 	*taskStr;
    EXEC_TASK_AV_STR 	*taskAV;
    EXEC_TASK_LIST 	*taskList;
    TYPE_STR 		*typeStr;
    DCL_NOM_STR         *dclNomStr;
    DCL_NOM_LIST        *dclNomList;
    TYPE_LIST 		*typeList;
    STR_REF_STR 	*strRefStr;
    STR_REF_LIST        *strRefList;
    POSTER_LIST         *posterList;
    RQST_INPUT_INFO_LIST *rqstInputInfoList;
}

%token <idStr> IDENTIFICATEUR QUOTED_STRING PACKAGENAME
%token <ival> CONSTANTE_ENTIERE
%token <dval> CONSTANTE_FLOTTANTE
%token <ival>  AUTO STATIC EXTERN REGISTER TYPEDEF
%token <ival> CHAR SHORT INT LONG UNSIGNED FLOAT DOUBLE
%token <ival> STRUCT UNION ENUM
%token <ival> MEMBER N_WHITE

%token <ival> REQUIRE CODELS_REQUIRE
%token <ival> MODULE REQUEST EXEC_TASK IMPORT_TYPE FROM
%token <ival> INTERNAL_DATA
%token <ival> VERSION
%token <ival> IFACE_VERSION
%token <ival> EMAIL
%token <ival> LANG USE_CXX
%token <ival> NUMBER CODEL_FILES
%token <ival> TYPE INPUT OUTPUT INTERRUPT_ACTIVITY
%token <ival> POSTERS_INPUT
%token <ival> CODEL_CONTROL CODEL_MAIN
%token <ival> CODEL_START CODEL_END CODEL_FAIL CODEL_INTER
%token <ival> PERIOD PRIORITY STACK_SIZE
%token <ival> CODEL_TASK_MAIN CODEL_TASK_START CODEL_TASK_END
%token <ival> FAIL_REPORTS RESOURCES T_DELAY
%token <ival> CONTROL EXEC ALL G_NONE INIT
%token <ival> POSTER DATA UPDATE ADDRESS 
%token <idStr>  POSTER_LOCAL POSTER_SM_MEM POSTER_VME24 POSTER_VME32
%token <ival> USER
%token <ival> CODEL_POSTER_CREATE
%token <ival> CS_CLIENT_FROM POSTER_CLIENT_FROM
%token <ival> RQST_NUM RQST_DOC RQST_INPUT_INFO

%type <ival> error 

%type <libtool_version> libtool_version
%type <idStr> identificateur quoted_string
%type <idList> quoted_string_list
%type <ival> expression_constante
%type <dval> expression_constante_flottante
%type <ival> liste_declaration_top declaration_top
%type <dclNomList> liste_declaration_de_typedef
%type <rqstList> liste_declaration_de_requete
%type <taskList> liste_declaration_de_tache
%type <moduleStr> declaration_de_module
%type <rqstStr> declaration_de_requete 
%type <taskStr> declaration_de_tache
%type <ival> list_packages
%type <moduleStr> liste_av_module attributs_de_module 
%type <moduleAV> paire_av_module av_module
%type <rqstStr> attributs_de_requete liste_av_requete 
%type <rqstAV> paire_av_requete av_requete
%type <ival> type_requete
%type <rqstList> liste_requete liste_requete_simple
%type <strRefStr> ref_membre_struct 
%type <idList> sdi_ref membre_sdi 
%type <idList> liste_echecs liste_ressources
%type <rqstInputInfoList> liste_input_info input_info

%type <posterList> liste_declaration_de_posters declaration_de_poster
%type <idStr> bus_space

%type <taskStr> attributs_de_tache liste_av_tache 
%type <taskAV> paire_av_tache av_tache
%type <ival> valeur_periode
%type <idList> liste_modules liste_posters
%type <strRefList> liste_ref_membre_struct
%type <typeList> liste_indicateur_de_type
%type <dclNomList> named_type_list
%type <dclNomStr> named_type

%type <dclNomList> declaration_de_typedef
%type <typeStr> indicateur_de_declaration 
%type <typeStr> indicateur_de_type type_complexe indicateur_structure_union
%type <typeStr> indicateur_enum
%type <typeStr> type_simple type_entier
%type <ival> type_entier_simple type_flottant attributs_type_entier
%type <dclNomList> liste_elt_enum 
%type <dclNomList> declaration_struct  liste_decl_struct 
%type <dclNomStr> mem_struct elt_enum
%type <dclNomList> liste_mem_struct 
%type <dclNomList> liste_decl;
%type <dclNomStr>  elt_declarable
%type <ival> indicateur_classe_memorisation 
%type <idStr>  nom_typedef 

%right '?' ':'
%left ','
%left OR
%left AND
%left '|'
%left '^'
%left '&'
%left EQUAL NOTEQUAL
%left '<' '>' LEQ GEQ
%left LSH RSH
%left '+' '-'
%left '*' '/' '%'
%right UNARY

%%

/*
 * A .gen file is a list of declaration
 */

liste_declaration_top: declaration_top
    | declaration_top liste_declaration_top 
    | error liste_declaration_top
    ;

declaration_top: declaration_de_module ';' { $$ = 0; }
    | liste_declaration_de_typedef { typedefs = $1; $$ = 0; }
    | liste_declaration_de_requete { requetes = $1; $$ = 0; }
    | liste_declaration_de_tache { taches = $1; $$ = 0; }
    | liste_declaration_de_posters { posters = $1; $$ = 0; }
    ;

liste_declaration_de_typedef: declaration_de_typedef 
	{ $$ = $1;  
	  typedefs = $$; }
    | liste_declaration_de_typedef declaration_de_typedef  
	{ DCL_NOM_LIST *t;
	  if ($1 == NULL) {
              $$ = $2;
	      typedefs = $$;
	  } else {
	      for (t=$1; t->next != NULL; t=t->next);
	      t->next = $2;
	      $$ = $1;
	     typedefs = $$;
	  }
        }
;

liste_declaration_de_requete: declaration_de_requete ';'
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->rqst = $1; 
	  $$->name = $1->name;
	  $$->flags = 0;
	  $$->next = NULL; }
    | declaration_de_requete ';' liste_declaration_de_requete    
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->rqst = $1;
	  $$->name = $1->name;
	  $$->flags = 0; 
	  $$->next = $3; }
;

liste_declaration_de_tache: declaration_de_tache ';'
	{ $$ = STR_ALLOC(EXEC_TASK_LIST);
	  $$->exec_task = $1;
	  $$->next = NULL; }
    | declaration_de_tache ';' liste_declaration_de_tache 
	{ $$ = STR_ALLOC(EXEC_TASK_LIST);
	  $$->exec_task = $1;
	  $$->next = $3; }
;

liste_declaration_de_posters: declaration_de_poster ';'
	{ $$ = $1; } 
    | declaration_de_poster ';' liste_declaration_de_posters
        { $1->next = $3;
          $$ = $1; }
;

/*----------------------------------------------------------------------*/

list_packages: 
	     PACKAGENAME { $$ = 0; }
             | IDENTIFICATEUR { $$ = 0; }
             | list_packages ',' PACKAGENAME
             | list_packages ',' IDENTIFICATEUR
             ;

declaration_de_module: MODULE identificateur attributs_de_module  
        { $3->name = $2; module = $3; $$ = $3; }
;

attributs_de_module: '{' liste_av_module '}' { $$ = $2; }
;

liste_av_module: paire_av_module
        { $$ = STR_ALLOC(MODULE_STR);
	  $$->lang = MODULE_LANG_DEFAULT; /* default value */
          $$ = ajout_av_module($1, $$); 
	  free($1); } 
    | paire_av_module liste_av_module 
	{ $$ = ajout_av_module($1, $2); 
	  free($1); }
    ;

paire_av_module: av_module ';' ;

av_module: INTERNAL_DATA ':' indicateur_de_type
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.internal_data = $3; }
    | NUMBER ':' expression_constante
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.number = $3; }
    | CODEL_FILES ':' quoted_string_list
	{ $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.codel_files = $3; }
    | VERSION ':' quoted_string
        { $$ = STR_ALLOC(MODULE_AV_STR);
          $$->attribut = $1; $$->value.version = $3; }
    | IFACE_VERSION ':' libtool_version
        { $$ = STR_ALLOC(MODULE_AV_STR);
          $$->attribut = $1; $$->value.iface_version = $3; }
    | EMAIL ':' quoted_string
        { $$ = STR_ALLOC(MODULE_AV_STR);
          $$->attribut = $1; $$->value.email = $3; }
    | REQUIRE ':' list_packages
        { $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.number = $3; }
    | CODELS_REQUIRE ':' list_packages
        { $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = $1; $$->value.number = $3; }
    | USE_CXX ':' expression_constante
        { $$ = STR_ALLOC(MODULE_AV_STR);
	  $$->attribut = LANG; $$->value.lang = MODULE_LANG_CXX;
	  fprintf(stderr,
		  "%s:%d: usage of use_cxx is deprecated. Use lang: \"c++\".\n",
		  nomfic, num_ligne);
	}
    | LANG ':' quoted_string
        { $$ = STR_ALLOC(MODULE_AV_STR);
          $$->attribut = $1;
	  if (!strcmp($3, "c"))
	    $$->value.lang = MODULE_LANG_C;
	  else if (!strcmp($3, "cxx"))
	    $$->value.lang = MODULE_LANG_CXX;
	  else if (!strcmp($3, "c++"))
	    $$->value.lang = MODULE_LANG_CXX;
	  else {
	    $$->value.lang = MODULE_LANG_C;
	    fprintf(stderr,
		    "%s:%d: unknown language %s. Valid languages are c, c++.\n",
		    nomfic, num_ligne, $3);
	  }
	}
    ;

/*----------------------------------------------------------------------*/

libtool_version: expression_constante ':' expression_constante ':' expression_constante
    { $$ = STR_ALLOC(LIBTOOL_VERSION);
      $$->current=$1;
      $$->revision=$3;
      $$->age=$5; }
;

declaration_de_requete: REQUEST identificateur attributs_de_requete  
    { $3->name = $2; $$ = $3; }
;

attributs_de_requete: '{' liste_av_requete '}' { $$ = $2; } 
;

liste_av_requete: paire_av_requete 
	{ $$ = STR_ALLOC(RQST_STR);
          $$->rqst_num = -1;
	  $$ = ajout_av_requete($1, $$); 
	  free($1); }
    | paire_av_requete liste_av_requete 
	{ $$ = ajout_av_requete($1, $2); 
	  free($1); }
    ;

paire_av_requete: av_requete ';' { $$ = $1; }
;	

av_requete: TYPE {keyword = 0;} ':' type_requete
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; $$->value.type = $4; keyword = 1;
	  if($4==INIT) {
	    initRequest++;
	    if(initRequest>1) 
	      fprintf(stderr, "%s: error: too many init requests.\n", nomfic);}}
    | INPUT ':' ref_membre_struct
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.input = $3; }
    | POSTERS_INPUT ':' liste_indicateur_de_type
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.posters_input_types = $3; }
    | OUTPUT ':' ref_membre_struct
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.output = $3; }
    | CODEL_CONTROL ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_control = $3; }
    | CODEL_MAIN ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_main = $3; }
    | CODEL_START ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_start = $3; }
    | CODEL_END ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_end = $3; }
    | CODEL_FAIL ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_fail = $3; }
    | CODEL_INTER ':' identificateur
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_inter = $3; }
    | INTERRUPT_ACTIVITY ':' liste_requete
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.interrupt_activity = $3; }
    | EXEC_TASK ':' identificateur
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1; 
	  $$->value.exec_task_name = $3; }
    | FAIL_REPORTS ':' liste_echecs
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.fail_reports = $3; }
    | RESOURCES ':' liste_ressources
	{ $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.resource_list = $3; }
    | RQST_NUM ':' expression_constante
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.rqst_num = $3; }
    | RQST_DOC ':' quoted_string
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.doc = $3; }
    | RQST_INPUT_INFO ':' liste_input_info
        { $$ = STR_ALLOC(RQST_AV_STR);
	  $$->attribut = $1;
	  $$->value.input_info = $3; }
    ;

type_requete: EXEC
    | CONTROL
    | INIT
    ;

ref_membre_struct: indicateur_de_type 
        { TYPE_STR *t = trouve_type($1);
	  if (t == NULL) {
	    fprintf(stderr, "%s:%d: warning: type inconnu %s\n",
		    nomfic, num_ligne, $1->name);
	  } else if ($1 != t) {
	    free($1->name);
	    free($1->filename);
	    free($1);
	    markUsed(t);
	    $1 = t;
	  }
	}
	sdi_ref elt_declarable
        {
	  fprintf(stderr, "%s:%d: warning syntaxe obsolete. "
		  "Utilisez `variable::sdi_ref'\n", nomfic, num_ligne);
	  $$ = STR_ALLOC(STR_REF_STR);
	  $4->type = $1;
	  $$->dcl_nom = $4;
	  $$->sdi_ref = $3; }
    | elt_declarable MEMBER sdi_ref
        {
	  $$ = STR_ALLOC(STR_REF_STR);
	  $$->sdi_ref = $3;
	  $$->dcl_nom = $1;
	}
;

sdi_ref:  membre_sdi
	{ $$ = $1; }
    |  ALL 
	{ $$ = NULL; }
    ;

membre_sdi: identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = NULL; }
    |  identificateur '.' membre_sdi
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = $3; }
    ;

liste_requete: G_NONE { $$ = NULL; }
    | ALL 
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->name = NULL;	
	  $$->rqst = NULL;
	  $$->flags = $1;
	  $$->next = NULL; }
    | liste_requete_simple
    ;

liste_requete_simple: identificateur 
	{ $$ = STR_ALLOC(RQST_LIST);
	  $$->name = $1;
	  $$->rqst = NULL;
	  $$->flags = 0;
	  $$->next = NULL; }
    | identificateur ',' liste_requete_simple
	{  $$ = STR_ALLOC(RQST_LIST);
	  $$->name = $1;
	  $$->rqst = NULL;
	  $$->flags = 0;
	  $$->next = $3; }
    ;

liste_echecs: identificateur
	{ $$ = STR_ALLOC(ID_LIST); 
	  $$->name = $1;
	  $$->next = NULL; } 
    | identificateur ',' liste_echecs
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = $3; }
    ;

liste_ressources: identificateur
	{ $$ = STR_ALLOC(ID_LIST); 
	  $$->name = $1;
	  $$->next = NULL; } 
    | identificateur ',' liste_ressources
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = $3; }
    ;

liste_input_info: input_info
    { $$ = $1;}
    | input_info ',' liste_input_info
    { $$ = $1;
      $$->next = $3;}
    ;

input_info: expression_constante MEMBER quoted_string
    { $$ = STR_ALLOC(RQST_INPUT_INFO_LIST);
      $$->default_val.i_val = $1;
      $$->doc = $3;
      $$->type = INT;
      $$->next = NULL;}
    | expression_constante_flottante MEMBER quoted_string
    { $$ = STR_ALLOC(RQST_INPUT_INFO_LIST);
      $$->default_val.d_val = $1;
      $$->doc = $3;
      $$->type = DOUBLE;
      $$->next = NULL;}
    | quoted_string MEMBER quoted_string
    { $$ = STR_ALLOC(RQST_INPUT_INFO_LIST);
      $$->default_val.str_val = $1;
      $$->doc = $3;
      $$->type = CHAR;
      $$->next = NULL;}
    ;

quoted_string:
    QUOTED_STRING 
    {$$ = $1;}
    | QUOTED_STRING  quoted_string
      { $1 = xrealloc($1, strlen($1) + strlen($2) + 1);
        strcat($1, $2);
	free($2);
        $$ = $1;
      }
   ;

quoted_string_list:
    QUOTED_STRING 
    {
       $$ = STR_ALLOC(ID_LIST);
       $$->name = $1;
       $$->next = NULL;
    }
    | quoted_string_list ',' quoted_string
    {
       ID_LIST *l;

       $$ = STR_ALLOC(ID_LIST);
       $$->name = $3;
       $$->next = NULL;

       /* keep the files ordered... just in case */
       for(l=$1; l->next; l=l->next);
       l->next = $$;

       $$ = $1;
    }
    ;

/*----------------------------------------------------------------------*/

declaration_de_tache: EXEC_TASK identificateur attributs_de_tache  
	{ $3->name = $2; $$ = $3; }
;

attributs_de_tache: '{' liste_av_tache '}' { $$ = $2; }
;

liste_av_tache: paire_av_tache
	{ $$ = STR_ALLOC(EXEC_TASK_STR);
	  $$ = ajout_av_tache($1, $$); 
	  free($1); }
    | paire_av_tache liste_av_tache
	{ $$ = ajout_av_tache($1, $2); 
	  free($1); }
    ;

paire_av_tache: av_tache ';' ;

av_tache: PERIOD ':' valeur_periode
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.period = $3; }
    | T_DELAY ':' valeur_periode
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.delay = $3; }
    | PRIORITY ':' expression_constante
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.priority = $3; }
    | STACK_SIZE ':' expression_constante
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.stack_size = $3; }
    | CODEL_TASK_START ':' identificateur
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_task_start = $3; }
    | CODEL_TASK_END ':' identificateur
      { $$ = STR_ALLOC(EXEC_TASK_AV_STR);
        $$->attribut = $1;
        $$->value.codel_task_end = $3; }
    | CODEL_TASK_MAIN ':' identificateur
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.codel_task_main = $3; }
    | POSTERS_INPUT ':' liste_indicateur_de_type
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1; 
	  $$->value.posters_input_types = $3; }
    | CS_CLIENT_FROM ':' liste_modules
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.cs_client_from = $3; }
    | POSTER_CLIENT_FROM ':' liste_posters
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.poster_client_from = $3; }
    | RESOURCES ':' liste_ressources
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.resource_list = $3; }
    | FAIL_REPORTS ':' liste_echecs
	{ $$ = STR_ALLOC(EXEC_TASK_AV_STR);
	  $$->attribut = $1;
	  $$->value.fail_reports = $3; }
    ;


valeur_periode: expression_constante
    | G_NONE { $$ = 0; }
    ;

liste_ref_membre_struct: ref_membre_struct
        { $$ = STR_ALLOC(STR_REF_LIST);
	  $$->str_ref = $1;
	  $$->next = NULL; }
    | ref_membre_struct ',' liste_ref_membre_struct
        { $$ = STR_ALLOC(STR_REF_LIST);
	  $$->str_ref = $1;
	  $$->next = $3;
         }
;

liste_modules: identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  $$->next = NULL; }
    | liste_modules ',' identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $3;
	  $$->next = $1; }
    ;

liste_posters: identificateur MEMBER identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $1;
	  free($3);
	  $$->next = NULL; }
    | liste_posters ',' identificateur MEMBER identificateur
	{ $$ = STR_ALLOC(ID_LIST);
	  $$->name = $3;
	  free($5);
	  $$->next = $1; }
    ;

/*----------------------------------------------------------------------*/

declaration_de_poster: POSTER identificateur '{' 
                 UPDATE ':' AUTO ';'
                 DATA ':' liste_ref_membre_struct ';'
		 EXEC_TASK ':' identificateur ';'
              '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = $10;
	  $$->type = NULL;
	  $$->types = NULL;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->address = NULL; 
	  $$->codel_poster_create = NULL; }
    | POSTER identificateur '{' 
                 UPDATE ':' USER ';'
                 TYPE ':' named_type_list ';'
		 EXEC_TASK ':' identificateur ';'
              '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = NULL;
	  $$->type = NULL;
	  $$->types = $10;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->address = NULL; 
	  $$->codel_poster_create = NULL; }
    | POSTER identificateur '{' 
                 UPDATE ':' USER ';'
                 DATA ':' liste_ref_membre_struct ';'
		 EXEC_TASK ':' identificateur ';'
              '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = $10;
	  $$->type = NULL;
	  $$->types = NULL;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->address = NULL; 
          $$->codel_poster_create = NULL; }
   | POSTER identificateur '{'
                 UPDATE ':' USER ';'
                 TYPE ':' named_type_list ';'
		 EXEC_TASK ':' identificateur ';'
                 ADDRESS ':' bus_space MEMBER expression_constante ';'
      '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = NULL;
	  $$->type = NULL;
	  $$->types = $10;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->bus_space = $18;
	  $$->address = (void *)$20; 
	  $$->codel_poster_create = NULL; }
   | POSTER identificateur '{'
                 UPDATE ':' USER ';'
                 TYPE ':' named_type_list ';'
		 EXEC_TASK ':' identificateur ';'
                 CODEL_POSTER_CREATE ':' identificateur ';'
      '}'
	{ $$ = STR_ALLOC(POSTER_LIST);
	  $$->update = $6;
	  $$->next = NULL;
	  $$->name = $2;
	  $$->data = NULL;
	  $$->type = NULL;
	  $$->types = $10;
	  $$->exec_task = STR_ALLOC(EXEC_TASK_STR);
	  $$->exec_task->name = $14;
	  $$->bus_space = 0;
	  $$->address = NULL;
	  $$->codel_poster_create = $18; }
    ;

bus_space:
	POSTER_LOCAL { $$ = "POSTER_LOCAL_MEM"; }
        | POSTER_SM_MEM { $$ = "POSTER_SM_MEM"; } 
        | POSTER_VME24 { $$ = "POSTER_VME_A24"; } 
        | POSTER_VME32 { $$ = "POSTER_VME_A32"; } 
        ;


/*----------------------------------------------------------------------*/

declaration_de_typedef: 
    TYPEDEF indicateur_de_declaration liste_decl ';' 
	{ DCL_NOM_LIST *l;
	  for (l=$3; l!= NULL; l=l->next) {
	      l->dcl_nom->type = $2;
	      /* Identification of STRING and ARRAY types */
	      if(IS_STRING(l->dcl_nom)) {
		l->dcl_nom->flags = STRING;
		/* Is it a good idea  ? */
		/* l->dcl_nom->ndimensions--;
		l->dcl_nom->max_str_len = l->dcl_nom->dimensions[l->dcl_nom->ndimensions]; */
		printf ("type STRING: %s\n", l->dcl_nom->name);
		fprintf(stderr, "%s:%d: warning: forme de déclaration de "
			"typedef pour %s non encore autorisée.\n", 
			nomfic, num_ligne, l->dcl_nom->name); 
	      }
	      if(IS_ARRAY(l->dcl_nom)) {
		l->dcl_nom->flags |= ARRAY;
		printf ("type ARRAY: %s\n", 
			l->dcl_nom->name);
		fprintf(stderr, "%s:%d: warning: forme de déclaration de "
			"typedef pour %s non encore autorisée.\n", 
			nomfic, num_ligne, l->dcl_nom->name);
	      }
	  }
	  $$ = $3; 
        }
    | IMPORT_TYPE { import_flag = 1; } '{' liste_declaration_de_typedef '}'  ';'
        { DCL_NOM_LIST *l;
	  for (l = $4; l!= NULL; l=l->next) {
	      l->dcl_nom->type->flags |= TYPE_IMPORT;
	  }
	  import_flag = 0;
	  $$ = $4; 
         }
| IMPORT_TYPE FROM identificateur { import_flag = 1; }
       '{' liste_declaration_de_typedef '}'  ';'
        { DCL_NOM_LIST *l;
	  ID_LIST *ld;
	  for (l = $6; l!= NULL; l=l->next) {
	      l->dcl_nom->type->flags |= TYPE_IMPORT;
	  }
	  ld = STR_ALLOC(ID_LIST);
	  ld->name = $3;
	  ld->next = imports;
	  imports = ld;
	  import_flag = 0;
	  $$ = $6; 
         } 
;

indicateur_de_declaration:
    indicateur_de_type
          { $$ = $1; }
    /* | indicateur_de_type indicateur_de_declaration 
          { $$ = $1; } */
    | indicateur_classe_memorisation indicateur_de_declaration
          { $$ = $2; } 
    ;

indicateur_classe_memorisation:
    AUTO
    | STATIC
    | EXTERN
    | REGISTER
    ;


liste_indicateur_de_type: indicateur_de_type
    { $$ = STR_ALLOC(TYPE_LIST); 
      $$->next = NULL; $$->type = $1; }
    | indicateur_de_type ',' liste_indicateur_de_type
    { $$ = STR_ALLOC(TYPE_LIST); 
      $$->next = $3; $$->type = $1; }
;

named_type_list:
    named_type
    {
       $$ = STR_ALLOC(DCL_NOM_LIST);
       $$->dcl_nom = $1;
       $$->next = NULL;
    }
    | named_type ',' named_type_list
    {
       $$ = STR_ALLOC(DCL_NOM_LIST);
       $$->dcl_nom = $1;
       $$->next = $3;
    }
;

named_type:
    indicateur_de_type
    {
       $$ = STR_ALLOC(DCL_NOM_STR);
       if ($1->name)
	  $$->name = strcpytolower($1->name);
       else
	  $$->name = new_name();
       $$->type = $1;
       $$->pointeur = 0;
       $$->dimensions = NULL;
       $$->ndimensions = 0;
       $$->flags = 0;
    }
    | identificateur MEMBER indicateur_de_type
    {
       $$ = STR_ALLOC(DCL_NOM_STR);
       $$->name = $1;
       $$->type = $3;
       $$->pointeur = 0;
       $$->dimensions = NULL;
       $$->ndimensions = 0;
       $$->flags = 0;
    }
;

indicateur_de_type: 
     type_simple 
     | type_complexe
     ;

type_simple:
    type_entier
    | type_flottant
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = NULL;
	  $$->used = 0;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    ;

type_flottant:
    FLOAT
    | DOUBLE
    ;

type_entier:
    type_entier_simple
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = NULL;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    | attributs_type_entier type_entier_simple
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $2;
	  $$->flags |= $1;
	  $$->name = NULL;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    | attributs_type_entier
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = INT;
	  $$->flags |= $1;
	  $$->name = NULL;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    ;

type_entier_simple:
    CHAR 
    | INT 
    ;

attributs_type_entier:
    SHORT { $$ = SHORT_INT; }
    | LONG { $$ = LONG_INT; }
    | UNSIGNED { $$ = UNSIGNED_TYPE; }
    | UNSIGNED SHORT { $$ = UNSIGNED_TYPE | SHORT_INT; } 
    | UNSIGNED LONG { $$ = UNSIGNED_TYPE | LONG_INT; }
    | LONG LONG { $$ = LONG_LONG_INT; }
    | UNSIGNED LONG LONG { $$ = UNSIGNED_TYPE | LONG_LONG_INT; }
    ;

type_complexe:
    indicateur_structure_union 
	{ $$ = $1; }
    | indicateur_enum
        { $$ = $1; }
    | nom_typedef 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = TYPEDEF;
	  $$->name = $1;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }
    ;

/*
 * Declared elements or names in typedef definition
 */
liste_decl:
    elt_declarable 
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->next = NULL; $$->dcl_nom = $1; }
    | elt_declarable ',' liste_decl 
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->next = $3; $$->dcl_nom = $1; }
    ;

/*
 * Structures and unions declarations
 */
indicateur_structure_union:
    STRUCT '{' liste_decl_struct '}' 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = new_name();
	  $$->members = $3; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }

    /* A named structure */
    | STRUCT identificateur '{' liste_decl_struct '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->members = $4;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }

    /* reference to a non-defined structure */
    | STRUCT identificateur 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->members = NULL; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
        }

    /* unnamed union */
    | UNION '{' liste_decl_struct '}' 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = new_name();
	  $$->members = $3; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }

    /* non-defined union */
    | UNION identificateur 
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  $$->members = NULL; }
    
    /* named union */
    | UNION identificateur '{' liste_decl_struct '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = $1;
	  $$->name = $2;
	  $$->members = $4; 
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
        }
    ;

liste_decl_struct:
    declaration_struct
    | declaration_struct liste_decl_struct
	{ DCL_NOM_LIST *l;
	  for (l=$1; l!= NULL && l->next != NULL; l=l->next)
	      ;
	  $$ = $1; l->next = $2; 
        }
    ;

declaration_struct:
    indicateur_de_type liste_mem_struct ';' 
        { /* variable/type associations */
	  DCL_NOM_LIST *l;
	  TYPE_STR *t = trouve_type($1);

	  /* corresponding type not found */
	  if (t == NULL) {
	      fprintf(stderr, "%s:%d: warning: type inconnu %s\n",
		      nomfic, num_ligne, $1->name);
	  } else if ($1 != t) {
	      free($1->name);
	      free($1->filename);
	      free($1);
	      markUsed(t);
	      $1 = t;
	  }
	  for (l = $2; l != NULL; l = l->next) {
	      l->dcl_nom->type = $1;

	      /* Identification of ARRAY and STRING */
	      if(IS_STRING(l->dcl_nom)) {
		l->dcl_nom->flags = STRING;
		/* l->dcl_nom->ndimensions--;
		l->dcl_nom->max_str_len = l->dcl_nom->dimensions[l->dcl_nom->ndimensions]; */
	      }
	      if(IS_ARRAY(l->dcl_nom)) {
		l->dcl_nom->flags |= ARRAY;
	      }

	  } /* for */
          $$ = $2; 
        }
;

liste_mem_struct:
    mem_struct 
        { $$ = STR_ALLOC(DCL_NOM_LIST);
          $$->next = NULL; $$->dcl_nom = $1; }
    | mem_struct ',' liste_mem_struct
        { $$ = STR_ALLOC(DCL_NOM_LIST);
          $$->next = $3; $$->dcl_nom = $1; }
    ;

/*
 * structure membre analysis
 */
mem_struct:
    elt_declarable
    | elt_declarable ':' expression_constante
    ;


elt_declarable:
    identificateur 
	{ $$ = STR_ALLOC(DCL_NOM_STR);
	  $$->name = $1;
	  $$->type = NULL;
	  $$->pointeur = 0;
	  $$->ndimensions = 0;
	  $$->dimensions = NULL;
	}
    | '(' elt_declarable ')'  
	{ $$ = $2; }
    | '*' elt_declarable 
	{$2->pointeur++; $$=$2; 
	fprintf(stderr, "%s:%d: WARNING: pointer %s not recommended in SDI\n",
		nomfic, num_ligne, $2->name);}
    | elt_declarable '(' ')'
    | elt_declarable '[' ']' 
	{ $1->pointeur++; $$ = $1; }
    | elt_declarable '[' expression_constante ']' 
	{ $1->dimensions = ($1->dimensions == NULL ?
	      xalloc(sizeof(int)) :
	      xrealloc($1->dimensions, ($1->ndimensions + 1) * sizeof(int)));
	  $1->dimensions[($1->ndimensions)++] = $3; 
          $$ = $1; }
    ;

nom_typedef: 
    identificateur 
    ;

/*
 * enum analysis
 */
indicateur_enum:
    ENUM '{' liste_elt_enum '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = ENUM;
	  $$->name = new_name();
	  $$->members = $3;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
	}
    | ENUM identificateur '{' liste_elt_enum '}'
	{ $$ = STR_ALLOC(TYPE_STR);
	  $$->type = ENUM;
	  $$->name = $2;
	  $$->members = $4;
	  $$->linenum = num_ligne;
	  $$->filename = strdup(nomfic);
	  if (import_flag) $$->flags |= TYPE_IMPORT;
	  ajout_type($$);
	}
    ;

liste_elt_enum:
    elt_enum
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->dcl_nom = $1; 
	  $$->next = NULL;
	  /* first element */
	  if ($1->flags == ENUM_NO_VALUE) {
	      $1->pointeur = 0;
	      $1->flags = ENUM_VALUE;
	  }
	  enumValue = $1->pointeur + 1;
	}
    | liste_elt_enum ',' elt_enum
	{ $$ = STR_ALLOC(DCL_NOM_LIST);
	  $$->dcl_nom = $3;
	  $$->next = $1; 
	  if ($3->flags == ENUM_NO_VALUE) {
	      $3->pointeur = enumValue;
	      $3->flags = ENUM_VALUE;
	  }
	  enumValue = $3->pointeur + 1;
	}
    ;

elt_enum:
    identificateur
 	{ $$ = STR_ALLOC(DCL_NOM_STR); 
          $$->name = $1;
	  $$->type = NULL;
	  $$->pointeur = 0;
	  $$->ndimensions = 0;
	  $$->dimensions = NULL; 
	  $$->flags = ENUM_NO_VALUE; }
    | identificateur '=' expression_constante
	{ $$ = STR_ALLOC(DCL_NOM_STR);
          $$->name = $1;
	  $$->type = NULL;
	  $$->pointeur = $3;
	  $$->ndimensions = 0;
	  $$->dimensions = NULL; 
	  $$->flags = ENUM_VALUE; }

    ;
/*
 * constant expressions analysis 
 */

expression_constante:
      '-' expression_constante %prec UNARY
	{ $$ = - $2; }
    | '!' expression_constante %prec UNARY
	{ $$ = ! $2; }
    | '+' expression_constante %prec UNARY
        { $$ = $2; }
    | '~' expression_constante %prec UNARY
	{ $$ = ~ $2; }
    | '(' expression_constante ')' 
   	{ $$ = $2; }
    | expression_constante '*' expression_constante
  	{ $$ = $1 * $3; }
    | expression_constante '/' expression_constante
	{ $$ = $1 / $3; }
    | expression_constante '%' expression_constante
	{ $$ = $1 % $3; }
    | expression_constante '+' expression_constante
	{ $$ = $1 + $3; }
    | expression_constante '-' expression_constante
	{ $$ = $1 - $3; }
    | expression_constante LSH expression_constante
	{ $$ = $1 << $3; }
    | expression_constante RSH expression_constante
  	{ $$ = $1 >> $3; }
    | expression_constante EQUAL expression_constante
	{ $$ = ($1 == $3); }
    | expression_constante NOTEQUAL expression_constante
  	{ $$ = ($1 != $3); }
    | expression_constante LEQ expression_constante
 	{ $$ = ($1 <= $3); }
    | expression_constante GEQ expression_constante
 	{ $$ = ($1 >= $3); }
    | expression_constante '<' expression_constante
	{ $$ = ($1 < $3); }
    | expression_constante '>' expression_constante
	{ $$ = ($1 > $3); }
    | expression_constante '&' expression_constante
	{ $$ = $1 & $3; }
    | expression_constante '^' expression_constante
	{ $$ = $1 ^ $3; }
    | expression_constante '|' expression_constante
	{ $$ = $1 | $3; }
    | expression_constante AND expression_constante
	{ $$ = $1 && $3; }
    | expression_constante OR expression_constante
	{ $$ = $1 || $3; }
    | expression_constante '?' expression_constante ':' expression_constante
	{ $$ = $1 ? $3 : $5; }
    | identificateur
        { if (trouve_value($1, &($$))) {
	    free($1);
	} else {
	    fprintf(stderr, "%s: %d: undefined symbol %s\n",
		    nomfic, num_ligne, $1);
	    $$=0;
	}
	}
    | CONSTANTE_ENTIERE
 	{ $$ = $1; }
    ;


expression_constante_flottante:
     '-' expression_constante_flottante %prec UNARY
	{ $$ = - $2; }
    | '!' expression_constante_flottante %prec UNARY
	{ $$ = ! $2; }
    | '+' expression_constante_flottante %prec UNARY
        { $$ = $2; }
    | '(' expression_constante_flottante ')' 
   	{ $$ = $2; }
    | expression_constante_flottante '*' expression_constante_flottante
  	{ $$ = $1 * $3; }
    | expression_constante_flottante '/' expression_constante_flottante
	{ $$ = $1 / $3; }
    | expression_constante_flottante '+' expression_constante_flottante
	{ $$ = $1 + $3; }
    | expression_constante_flottante '-' expression_constante_flottante
	{ $$ = $1 - $3; }
    | expression_constante_flottante EQUAL expression_constante_flottante
	{ $$ = ($1 == $3); }
    | expression_constante_flottante NOTEQUAL expression_constante_flottante
  	{ $$ = ($1 != $3); }
    | expression_constante_flottante LEQ expression_constante_flottante
 	{ $$ = ($1 <= $3); }
    | expression_constante_flottante GEQ expression_constante_flottante
 	{ $$ = ($1 >= $3); }
    | expression_constante_flottante '<' expression_constante_flottante
	{ $$ = ($1 < $3); }
    | expression_constante_flottante '>' expression_constante_flottante
	{ $$ = ($1 > $3); }
    | expression_constante_flottante AND expression_constante_flottante
	{ $$ = $1 && $3; }
    | expression_constante_flottante OR expression_constante_flottante
	{ $$ = $1 || $3; }
    | expression_constante_flottante '?' expression_constante_flottante ':' expression_constante_flottante
	{ $$ = $1 ? $3 : $5; }
    | CONSTANTE_FLOTTANTE
 	{ $$ = $1; }
    |  expression_constante
        { $$ =  $1; }
    ;

identificateur:
     IDENTIFICATEUR { $$ = $1; }
    | TYPE { $$ = "type"; }
    ;

%%
/*--------------------------------------------------------------------*/

/***
 *** The code C
 ***/

int yydebug = 0;
int verbose = 0;
/*----------------------------------------------------------------------*/

/**
 ** Main program
 **/

int
main(int argc, char **argv)
{
    extern FILE *yyin;
    char nomout[MAXPATHLEN];
    char cmdout[MAXPATHLEN];
    
    char *nomTemp;
    int opt;
    int errFlag;
    int noExecFlag = 0;
    extern char *optarg;
    int nopt;
    int status, fatalError;

    char cwd[MAXPATHLEN];
    static const char *upDir = "../";
    char chDir[10] = "";              /* upDir ou "" */
    static const char *extention = ".gen";

    int genIfChange = 0;
    int genTcl = 0;
    int genOpenprs = 0;
    int genServer = 1;
    int upToDate;
    struct stat statfile, statstamp, statgen;
    static const char *nomstamp = "genom-stamp";
    FILE *stamp;
    ID_LIST *il;
    int installUserPart = 0;
    char *cmdLine=NULL;
    int i;

    char tempDir[MAXPATHLEN] = ".genom";
    char codelsDir[MAXPATHLEN] = "codels";
    char autoDir[MAXPATHLEN] = "server";
    char autoconfDir[MAXPATHLEN] = "autoconf";
    char tclDir[MAXPATHLEN] = "server/tcl";
    char openprsDir[MAXPATHLEN] = "server/openprs";

    static const char *usage = 
      "Usage: \n  genom [-i] [-c] [-d] [-n] [-p protoDir] [-s] [-t] "
      "[-u codelsDir] [-o]\n\t[-Ddefine] [-Ipath] [-Ppackage] module[.gen]\n"
      "with:\n"
      "     -P: declare a package on which this module is dependent.\n"
      "         Packages are defined used via pkg-config, so you should\n"
      "         have a pkg-config file installed for each -P\n"
      "     -I: path for included file. Use @PACKAGE_CFLAGS@\n"
      "         to reuse a prefix defined with -Ppackage\n"
      "     -i: installs new templates for codels and makefiles\n"
      "     -a: api part only. The module is not generated\n"
      "     -h|--help: this help text\n"
      "     -t: produces on-board tcl client code\n"
      "     -o: produces openprs interfaces\n\n"
      "     -u: specifies the name of the codels directory\n"
      "     -p: changes the path for prototype files (canvas) \n"
      "     -D: define a preprocessor symbol\n"
      "     -c: generates if changes only  \n"
      "     -d: debug \n"
      "     -v: verbose (for debugging pruposes)\n"
      "     -n: produces \".pl\" without execution (for debugging purposes)\n"
      "     --includes:  print path to libgenom includes\n"
      "     --libraries: print path to libgenom libraries\n";


    /* remember cwd */
    if (getcwd(cwd, MAXPATHLEN) == 0) {
       perror("couldn't compute current working directory");
       exit(2);
    }

    /* 
     * Parsing des arguments 
     */

    errFlag = 0;
    memset(cppOptions, 0, sizeof(cppOptions));
    nopt = argc;

    while ((opt = getopt(argc, argv, "D:I:P:dnp:ciu:toas-:v")) != -1) {

	switch (opt) {
	  case 'D':
	    sprintf(nomout, "-%c%s", opt, optarg);
	    bufcat(&cmdLine, "-D%s ", optarg);
	    if (nCppOptions < MAX_CPP_OPT - 1) {
		cppOptions[nCppOptions++] = strdup(nomout);
	    } else {
		fprintf(stderr, "%s: too many options for cpp\n", argv[0]);
		exit(-1);
	    }
	    break;
	  case 'I': /* add an include, may use $(PACKAGE_PREFIX) */
	    sprintf(nomout, "-%c%s", opt, optarg);
	    bufcat(&cmdLine, "-I%s ", optarg);
	    if (nCppOptions < MAX_CPP_OPT - 1) {
		cppOptions[nCppOptions++] = strdup(nomout);
	    } else {
		fprintf(stderr, "%s: too many options for cpp\n", argv[0]);
		exit(-1);
	    }

            /* Cannot add the element at the head of the list
             because it leads to different generated files (configure.ac.begin,
             config.mk.in, genom.mk) at each run */
	    il = STR_ALLOC(ID_LIST);
	    il->name = strdup(optarg);
	    il->next = 0;
            externPath = push_back(externPath, il);
	    break;

	  case 'P': /* define a package */
	    bufcat(&cmdLine, "-P%s ", optarg);

	    /* keep macro name and path for makefiles */
	    il = STR_ALLOC(ID_LIST);
	    il->name = strdup(optarg);
	    il->next = 0;
	    requires = push_back(requires, il);
	    break;

	  case 'v':
	    verbose = 1;
	    break;
	  case 'd':
	    yydebug = 1;
	    break;
	  case 'n':
	    noExecFlag = 1;
	    break;
	  case 'a':
	    bufcat(&cmdLine, "-a ", optarg);
	    printf ("genom: API only (without the module server)\n");
	    genServer = 0;
	    break;
	  case 'p':
	    bufcat(&cmdLine, "-p%s ", optarg);
	    protoDir = strdup(optarg);
	    break;
	  case 'c':
	    genIfChange = 1;
	    break;
	  case 'i':
	    installUserPart = 1;
	    break;
	  case 'u':
	    bufcat(&cmdLine, "-u%s ", optarg);
	    sprintf (codelsDir, "%s", optarg);
	    break;
	  case 't':
	    bufcat(&cmdLine, "-t ", optarg);
	    genTcl = 1;
	    break;
	  case 'o':
	    bufcat(&cmdLine, "-o ", optarg);
	    genOpenprs = 1;
	    break;
	  case 'h':
	    printf(usage);
	    exit(0);
	    
	  case '-':
	     if (!strcmp("includes", optarg)) {
		puts(includeDir);
		exit(0);
	     } else if (!strcmp("help", optarg)) {
	       printf(usage);
	       exit(0);
	     } else if (!strcmp("libraries", optarg)) {
		puts(libraryDir);
		exit(0);
	     } else if (!strcmp("version", optarg)) {
	        printf("genom version %s\n", genomVersion);
	        exit(0);
	     }

	     /* FALLTROUGH */
	  case '?':
	    errFlag++;
	    break;
	    
	}  /* switch */
	nopt--;
    } /* while */

    if (errFlag) {
      fprintf(stderr, usage);
      exit(2);
    }

    /* make sure we've got an input file */
    if (nopt == 0) {
	fprintf(stderr, "%s: error: no file specified\n", argv[0]);
	fprintf(stderr, usage);
	exit (2);
    } 

    /* Enregistrer le nom du fichier a analyser */
    strcpy(nomfic, argv[argc - 1]);

    /* add .gen extension if missing */
    if (strcmp(nomfic + (strlen(nomfic) - strlen(extention)), 
	       extention) != 0) {
	strcat(nomfic, extention);
    }

    /* pre-parse the .gen file to get requires and codels_requires */
    genom_get_requires(nomfic, cppOptions);

    for (il = requires; il; il = il->next)
    {
        /* get_pkgconfig_cflags calls exit() itself if there is too much options */
        nCppOptions += get_pkgconfig_cflags(il->name, cppOptions, nCppOptions);
    }

    if (genServer) {
      for (il = codels_requires; il; il = il->next)
	{
	  /* get_pkgconfig_cflags calls exit() itself if there is too much options */
	  nCppOptions += get_pkgconfig_cflags(il->name, cppOptions, nCppOptions);
	}
    }

    /*upCaseArguments();*/

    /* Traitement des options de cpp */
    for (i = 0; i<nCppOptions; i++) {
	if (cppOptions[i][0] == '-' && cppOptions[i][1] == 'I') {
	    if (cppOptions[i][2] != '/' && chDir[0] != '\0') {
		/* chemin relatif et on a changé de répertoire */
		sprintf(nomout, "-I%s%s", chDir, &cppOptions[i][2]);
		free(cppOptions[i]);
		cppOptions[i] = strdup(nomout);
	    }
	}
    } /* for */
    cppOptions[nCppOptions] = NULL;
    nomTemp = callCpp(nomfic, cppOptions, 0);
    genfile = strdup(nomfic);

    /*
     * Parse description file
     */

    if ((yyin = fopen(nomTemp, "r")) == NULL) {
	perror(nomTemp);
	exit(2);
    }
    if (yyparse() == 1) {
	exit(2);
    }
    if (yydebug == 0) {
	unlink(nomTemp);
    }
    if (module == NULL) {
	fprintf(stderr, "\"%s\": Error: can't find a MODULE declaration\n", 
	       nomfic);
	exit(2);
    }
    if (yynerrs != 0) {
      fprintf(stderr, "Parsing errors.\n");
      exit(2);
    }
    /* Fix missing email and/or version */
    if (! module->email)
        module->email = strdup("nobody@nowhere");
    if (! module->version)
        module->version = strdup("0.0");

    /*
     * Build data structures
     */
    strcpy(nomfic, genfile);

    upCaseNames();

    resolveRequests();
    resolveTypes();
    resolveTasks();
    construitIncludeList();

    /*
     * Change to temporary directory
     */
    if (strlen(cwd) < strlen(tempDir) ||
	(strcmp(cwd + strlen(cwd) - strlen(tempDir), autoDir) != 0)) {
	
	/* mkdir tempDir */
	if (stat(tempDir, &statfile) == -1 
	    || ((statfile.st_mode & S_IFDIR) == 0)) {
	   if (mkdir (tempDir, 0775) != 0) {
	      fprintf(stderr, "mkdir `%s'", tempDir);
	      perror("");
	      exit(2);
	   }
	}

	/* chdir tempDir */
	if (chdir (tempDir) == -1) {
	   fprintf(stderr, "chdir `%s'", tempDir);
	   perror("");
	   exit(2);
	}
	strcpy(chDir, upDir);
    }

    /* 
     * re-generation needed ?
     */

    if(genIfChange && fopen(nomstamp, "r") != NULL) {
       strcpy(nomfic, chDir);
       strcat(nomfic, genfile);
       if (stat(nomfic, &statfile) < 0) {
	  perror(nomfic);
	  exit(2);
       }
       /* argv[0] doesn't contain full path */
       if (stat(argv[0], &statgen) < 0) {
	  statgen.st_mtime = 0;
       }
       if (stat(nomstamp, &statstamp) == 0) {
	  /* File .gen up-to-date */
	  if (statstamp.st_mtime > statfile.st_mtime && 
	      statstamp.st_mtime > statgen.st_mtime) {
	     upToDate = 1;

	     /* Test date files included in .gen */
	     for (il = allIncludeFiles; il != NULL; il = il->next) {
		stat(il->name, &statfile);
		if (statstamp.st_mtime < statfile.st_mtime) {
		   upToDate = 0;
		   break;
		}
	     }

	     if (upToDate) {
		fprintf (stderr, "Your module is up to date.\n");
		exit(0);
	     }
	  }
       }
    }
    unlink(nomstamp);


    /*
     * Generate perl script
     */

    /* file header */
    sprintf(nomout, "./%s.pl", module->name);
    if ((sortie = fopen(nomout, "w")) == NULL) {
        perror(argv[0]);
        exit(2);
    }

    fprintf(sortie, 
	"#!/usr/bin/env perl -w\n"
	"use strict;\n"
	"my $commentMode;\n");
    fprintf(sortie, "# Generateur du module %s\n\n", module->name);

    /* Variables for perl */
    fprintf(sortie, "use vars qw($module $moduleOprs $genOpenprs $genTcl $codelsDir $autoconfDir $serverDir $installUserPart $openprsDir $tclDir);\n");
    fprintf(sortie, "$module=\"%s\";\n", module->name);
    fprintf(sortie, "$moduleOprs=\"%s-oprs\";\n", module->name);
    fprintf(sortie, "$genOpenprs=%d;\n", genOpenprs);
    fprintf(sortie, "$genTcl=%d;\n", genTcl);

    fprintf(sortie, "$codelsDir=\"%s\";\n\n", codelsDir);
    fprintf(sortie, "$autoconfDir=\"%s\";\n\n", autoconfDir);
    fprintf(sortie, "$serverDir=\"%s\";\n\n", autoDir);
    fprintf(sortie, "$openprsDir=\"%s\";\n\n", openprsDir);
    fprintf(sortie, "$tclDir=\"%s\";\n\n", tclDir);
    fprintf(sortie, "$installUserPart=%d;\n\n", installUserPart);

    fprintf(sortie, "use vars qw($OVERWRITE $ASK_IF_CHANGED $SKIP_IF_CHANGED);\n");
    fprintf(sortie, "($OVERWRITE, $ASK_IF_CHANGED, $SKIP_IF_CHANGED) = (0, 1, 2);\n");

    /* start perl fle */
    script_do(sortie, protoDir, "start.pl");

    /* the module */
    fatalError = 0;

    fatalError |= (configureServerGen(sortie, 
                                      cmdLine, argv[0], genfile,
				      genTcl, genOpenprs, genServer)!=0);
    fatalError |= (typeGen(sortie) != 0);
    fatalError |= (errorGen(sortie) != 0);
    fatalError |= (endianGen(sortie) != 0);
    fatalError |= (printGen(sortie) != 0);
    fatalError |= (printXMLGen(sortie) != 0);
    fatalError |= (scanGen(sortie) != 0);
    fatalError |= (scopeGen(sortie) != 0);

    /* even with no server, endianLin needs posterLib.h 
       and tclLib and openprs seem to need posterLib.c (?) */
    fatalError |= (posterLibGen(sortie) != 0);
    /* even with no server, tclLib and openprsib seem to need msgLib.c  (?) */
    fatalError |= (msgLibGen(sortie) != 0);
    
    /* useless if no server */
    if (genServer) {
      fatalError |= (headerGen(sortie) != 0);
      fatalError |= (cntrlTaskGen(sortie) != 0);
      fatalError |= (execTaskGen(sortie) != 0);
      fatalError |= (moduleInitGen(sortie) != 0);
      fatalError |= (testGen(sortie) != 0);
      fatalError |= (initGen(sortie) != 0);
    }

    /* useless without openprs */
    if (genOpenprs)
      fatalError |= (openprsGen(sortie) != 0); 

    /* useless without tcl */
    if (genTcl) {
       fatalError |= (tclGen(sortie) != 0);
       fatalError |= (tclClientGen(sortie) != 0);
    }

    /* codels templates */
    if (genServer) {
      fatalError |= (userExecCodelsGen(sortie) != 0);
      fatalError |= (userCntrlCodelsGen(sortie) != 0);
    }

    /* building environment */
    fatalError |= (configureGen(sortie,
				codelsDir, 
                                cmdLine, argv[0], genfile, 
                                cwd,
				genTcl, genOpenprs, genServer) != 0);
    /* pkgconfig data file */
    fatalError |= (pkgconfigGen(sortie, cmdLine, genfile, 
				genOpenprs, genServer, cppOptions) != 0);

    /* finishing up */
    script_do(sortie, protoDir, "end.pl");
    fclose(sortie);

    if (!fatalError) {
	/* 
	 * Execution of perl 
	 */
	
	if (chmod(nomout, 0755) < 0) {
	    perror("chmod");
	    exit(1);
	}
	if (!noExecFlag) {
	     sprintf(cmdout, "perl -w %s\n", nomout);
	     fprintf(stderr,"%s", cmdout);
	     status = system(cmdout);
	    unlink(nomout);
	} else {
	    fprintf(stderr, "File %s is written\n", nomout);
	    status = 0;
	}
    
	/* Update stamp files */
	if (status == 0) {
	    stamp = fopen(nomstamp, "w");
	    fclose(stamp);

	    fprintf(stderr, "Done.\n");
	    exit(0);
	} else {
	    /* error while executing script */
	    perror ("Fatal error");
	    exit(2);
	}
	    
    } else {
	/* error during generation */
	fprintf(stderr, "No code generated.\n");
	unlink(nomout);
	exit (2);
    }
    /* we should not be there */
    return(2);
} /* main */


/*------------------------------------------------------------------------*/

/**
 ** parsing errors treatment
 **/
static void 
yyerror(char *s)
{
    fprintf(stderr, "\"%s\", line %d: %s\n", nomfic, num_ligne, s);
} /* yyerror */

/*----------------------------------------------------------------------*/


