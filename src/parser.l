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
%{
#include "config.h"
__RCSID("$LAAS$");
%}

%option noyywrap

C	[A-Z_a-z]
D	[0-9]
H       [0-9a-fA-F]
A	[0-9A-Z_a-z]
W       [ \t\n]
WC      {W}*\:

 #include <string.h>
 #include <stdlib.h>

 #include "genom.h"
 #include "parser.tab.h"

%%

"struct"   {yylval.ival = STRUCT; return(STRUCT); }
"union"    {yylval.ival = UNION; return(UNION); }
"enum"     {yylval.ival = ENUM; return(ENUM); }

"auto"     return(AUTO);
"static"   return(STATIC);
"extern"   return(EXTERN);
"register" return(REGISTER);
"typedef"  {yylval.ival = TYPEDEF; return(TYPEDEF); }


"int"	   {yylval.ival = INT; return(INT); }
"char"     {yylval.ival = CHAR; return(CHAR); }
"short"    {yylval.ival = SHORT; return(SHORT); }
"long"     {yylval.ival = LONG; return(LONG); }
"double"   {yylval.ival = DOUBLE; return(DOUBLE); }
"float"    {yylval.ival = FLOAT; return(FLOAT); }
"unsigned" {yylval.ival = UNSIGNED; return(UNSIGNED); }

"<<"       {yylval.ival = LSH; return(LSH); }
">>"       {yylval.ival = RSH; return(RSH); }
"=="       {yylval.ival = EQUAL; return(EQUAL); }
"!="       {yylval.ival = NOTEQUAL; return(NOTEQUAL); }
"<="       {yylval.ival = LEQ; return(LEQ); }
">="       {yylval.ival = GEQ; return(GEQ); }
"&&"       {yylval.ival = AND; return(AND); }
"||"       {yylval.ival = OR; return(OR); }

"::"       {yylval.ival = MEMBER; return(MEMBER); }

"module"          {yylval.ival = MODULE; return(MODULE); }
"request"         {yylval.ival = REQUEST; return(REQUEST); }
"poster"          {yylval.ival = POSTER; return(POSTER); }
"exec_task"       {yylval.ival = EXEC_TASK; return(EXEC_TASK); }

"import"          {yylval.ival = IMPORT_TYPE; return(IMPORT_TYPE); }
"from"            {yylval.ival = FROM; return(FROM); }

"internal_data"/{WC}  {yylval.ival = INTERNAL_DATA; return(INTERNAL_DATA); }
"number"/{WC}         {yylval.ival = NUMBER; return(NUMBER); }
"max_rqst_size"/{WC}  {yylval.ival = MAX_RQST_SIZE; return(MAX_RQST_SIZE); }
"max_reply_size"/{WC} {yylval.ival = MAX_REPLY_SIZE; return(MAX_REPLY_SIZE); }
"codel_files"/{WC}    {yylval.ival = CODEL_FILES; return(CODEL_FILES); }

"type"/{WC}            {yylval.ival = TYPE; return(TYPE); }
"input"/{WC}           {yylval.ival = INPUT; return(INPUT); }
"output"/{WC}          {yylval.ival = OUTPUT; return(OUTPUT); }
"c_control_func"/{WC}  {yylval.ival = C_CONTROL_FUNC; return(C_CONTROL_FUNC); }
"c_exec_func"/{WC}     {yylval.ival = C_EXEC_FUNC; return(C_EXEC_FUNC); }
"c_exec_func_start"/{WC} {yylval.ival = C_EXEC_FUNC_START; 
                     return(C_EXEC_FUNC_START); }
"c_exec_func_end"/{WC} {yylval.ival = C_EXEC_FUNC_END; return(C_EXEC_FUNC_END); }
"c_exec_func_fail"/{WC} {yylval.ival = C_EXEC_FUNC_FAIL; return(C_EXEC_FUNC_FAIL); }
"c_exec_func_inter"/{WC} {yylval.ival = C_EXEC_FUNC_INTER; return(C_EXEC_FUNC_INTER); }
"incompatible_with"/{WC} {yylval.ival = INCOMPATIBLE_WITH; return(INCOMPATIBLE_WITH); }
"fail_msg"/{WC}        {yylval.ival = FAIL_MSG; return(FAIL_MSG); }
"resources"/{WC}       {yylval.ival = RESOURCES; return(RESOURCES); }
"doc"/{WC}             {yylval.ival = RQST_DOC; return(RQST_DOC); }
"input_info"/{WC}    {yylval.ival = RQST_INPUT_INFO; return(RQST_INPUT_INFO); }

"period"/{WC}          {yylval.ival = PERIOD; return(PERIOD); }
"delay"/{WC}           {yylval.ival = T_DELAY; return(T_DELAY); }
"priority"/{WC}        {yylval.ival = PRIORITY; return(PRIORITY); }
"stack_size"/{WC}      {yylval.ival = STACK_SIZE; return(STACK_SIZE); }
"c_init_func"/{WC}     {yylval.ival = C_INIT_FUNC; return(C_INIT_FUNC); }
"c_end_func"/{WC}      {yylval.ival = C_END_FUNC; return(C_END_FUNC); }
"c_func"/{WC}          {yylval.ival = C_FUNC; return(C_FUNC); }
"cs_client_from"/{WC}  {yylval.ival = CS_CLIENT_FROM; return(CS_CLIENT_FROM); }
"poster_client_from"/{WC} {yylval.ival = POSTER_CLIENT_FROM; 
                                    return(POSTER_CLIENT_FROM); }

"data"/{WC}            {yylval.ival = DATA; return(DATA); }
"update"/{WC}          {yylval.ival = UPDATE; return(UPDATE); }
"address"/{WC}         {yylval.ival = ADDRESS; return(ADDRESS); }

"local"/{WC}           {yylval.ival = POSTER_LOCAL; return(POSTER_LOCAL); }
"sm"/{WC}              {yylval.ival = POSTER_SM_MEM; return(POSTER_SM_MEM); }
"vme32"/{WC}           {yylval.ival = POSTER_VME32; return(POSTER_VME32); }
"vme24"/{WC}           {yylval.ival = POSTER_VME24; return(POSTER_VME24); }

"c_create_func"/{WC}   {yylval.ival = C_CREATE_FUNC; return(C_CREATE_FUNC); }

"control"         {yylval.ival = CONTROL; return(CONTROL); }
"exec"            {yylval.ival = EXEC; return(EXEC); }
"init"            {yylval.ival = INIT; return(INIT); }
"all"             {yylval.ival = ALL; return(ALL); }
"none"            {yylval.ival = G_NONE; return(G_NONE); }

"user"            {yylval.ival = USER; return(USER); }

"martha_rqst_num"/{WC} {yylval.ival = RQST_NUM; return(RQST_NUM); }

{C}+{A}* { yylval.idStr = (ID_STR *)malloc(strlen(yytext) + 1);
          strcpy((char *)yylval.idStr, yytext);
          return(IDENTIFICATEUR); }

0x{H}+ { yylval.ival = strtol(yytext, (char **)NULL, 16); 
           return(CONSTANTE_ENTIERE); }

{D}+ { yylval.ival = atoi(yytext); return(CONSTANTE_ENTIERE); }


{D}+\.{D}* { sscanf(yytext, "%lf", &yylval.dval); return(CONSTANTE_FLOTTANTE); }
{D}*\.{D}+ { sscanf(yytext, "%lf", &yylval.dval); return(CONSTANTE_FLOTTANTE); }


"/*"  { char c; loop:
         while ((c = input()) != '*')
             if (c == '\n') 
                 num_ligne++ ;
         switch (input()) {
            case '/': break;
            case '\n': num_ligne++; goto loop;
            case '*': unput('*');
            default: goto loop;
	  }
     }

^\#.*$ { sscanf(yytext, "# %d \"%s", &num_ligne, nomfic); 
         /* cpp retourne le numero de la ligne suivante */
         num_ligne--;
         /* Ecrase le guillemet fermant, 
            car le format %s ci-dessus l'a lu  */
         nomfic[strlen(nomfic) - 1] = '\0';
         /* on stocke ce nom de fichier dans allIncludeFiles */
         {
            ID_LIST *il;
       	    /* Le fichier est-il deja dans la liste ? */
            for (il = allIncludeFiles; il != NULL; il = il->next) {
                if (!strcmp(il->name, nomfic)) {
            	break;
                }
            } /* for */
            if (il == NULL) {
                /* pas trouve' */
                il = STR_ALLOC(ID_LIST);
                il->name = strdup(nomfic);
                il->next = allIncludeFiles;
                allIncludeFiles = il;
            }
         }
        }


     /* Une chaîne entre guillements (")  
        Elle peut contenir des backslash-guillements (\") */
\"([^\\\"]|\\.)*\" { 
  yylval.idStr = (char *)malloc(strlen(yytext) - 2 + 1);
  strncpy((char *)yylval.idStr, yytext + 1, strlen(yytext)-2);
  yylval.idStr[strlen(yytext)-2] = '\0';
  return QUOTED_STRING;
}


[\[\]\.\,\;\:\{\}\*\&\(\)\?\|\^\=\<\<\+\-\/\%\@] {
	return(yytext[0]);
	}

\n      num_ligne++;

[ \t]	;


%%
