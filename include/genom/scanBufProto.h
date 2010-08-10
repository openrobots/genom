
#ifndef _SCAN_BUF_PROTO_H_
#define _SCAN_buf_BUF_PROTO_H_

int scan_buf_char(char **buf, char *x, int nDim, int *dims);
int scan_buf_unsigned_char(char **buf, unsigned char *x, int nDim, int *dims);
int scan_buf_short_int(char **buf, short int *x, int nDim, int *dims);
int scan_buf_int(char **buf, int *x, int nDim, int *dims);
int scan_buf_long_int(char **buf, long int *x, int nDim, int *dims);
int scan_buf_unsigned_short_int(char **buf, unsigned short *x, int nDim, int *dims);
int scan_buf_unsigned_int(char **buf, unsigned int *x, int nDim, int *dims);
int scan_buf_unsigned_long_int(char **buf, unsigned long *x, int nDim, int *dims);
int scan_buf_float(char **buf, float *x, int nDim, int *dims);
int scan_buf_double(char **buf, double *x, int nDim, int *dims);
int scan_buf_string(char **buf, char *x, int nDim, int *dims);
int scan_buf_string_len(char **buf, char *x, int max_str_len, int nDim, int *dims);

#endif
