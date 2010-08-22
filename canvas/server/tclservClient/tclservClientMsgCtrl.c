$commentbegin$
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
$commentend$
/*=========================================================================
 *
 *  $module$TclservClient$request$RqstSend  -  Emit control request
 *
 *  Return: 
 *    -1 in failure case
 *    0 in success case. pRqstId has a sensible value.
 */
 
int $module$TclservClient$request$RqstSend (
				  TCLSERV_CLIENT_ID clientId, 
				  ssize_t *pRqstId $hasInput$,
				  $input$
		)
{
  buf_t buf = buf_init();
  ssize_t res;
  buf_cat(buf, "$module$::$request$ ");
$hasInput$  buf_add_$type_input$(buf, $inputName$, 0, NULL);

  /* Emit the msg */
  res = tclserv_client_rqst(clientId, buf->data);
  buf_destroy(&buf);
  if (res != -1) {
	  *pRqstId = res;
	  return 0;
  }

  return -1;
}

/*-------------------------------------------------------------------------
 *  $module$TclservClient$request$ReplyRcv  
 *
 *  Wait for answer associated to the rqst defined by rqstId
 *
 *  Returns : 
 *    -2 in internal failure case
 *    -1 in rqst failure case, bilan store the error msg, and mut be freed
 *    0 in success case, output contains some sensible value
 */
 
int $module$TclservClient$request$ReplyRcv (
				   TCLSERV_CLIENT_ID clientId, 
			       ssize_t rqstId, 
			       $output$
			       char** bilan)
{
  int status;    
  char* res;

  status = tclserv_client_wait(clientId, rqstId, &res);
  if (status == -2)
	  return -2;

  if (status == 0) {
	  char* begin = res + 3; // SKIP "ok "
$has_output$    if (scan_buf_$output_type$(&begin, $outputName$, 0, NULL) == -1) {
$has_output$      free(res);
$has_output$      return -2;
$has_output$    }
	 free(res);
  } else {
	  *bilan = res;
  }
  
  return status;
}
 
/*-------------------------------------------------------------------------
 *  $module$TclservClient$request$RqstAndRcv  
 *
 *  Emit a request and wait for its termination
 *
 *  Returns : 
 *    -2 in internal failure case
 *    -1 in rqst failure case, bilan store the error msg, and mut be freed
 *    0 in success case, output contains some sensible value
 */
 
int $module$TclservClient$request$RqstAndRcv (
				 TCLSERV_CLIENT_ID clientId, 
				 $input$ $hasInput$, 
				 $output$
				 char** bilan)
{
  int status;
  char * res;

  buf_t buf = buf_init();
  buf_cat(buf, "$module$::$request$ ");
$hasInput$  buf_add_$type_input$(buf, $inputName$, 0, NULL);

  /* Emit the msg */
  status = tclserv_client_rqst_wait(clientId, buf->data, &res);
  buf_destroy(&buf);

  if (status == -2)
	  return -2;

  if (status == 0) {
	  char* begin = res + 3; // SKIP "ok "
$has_output$    if (scan_buf_$output_type$(&begin, $outputName$, 0, NULL) == -1) {
$has_output$      free(res);
$has_output$      return -2;
$has_output$    }
	 free(res);
  } else { // status == 1, method term but not ok
	  *bilan = res;
  }

  return status;
}


