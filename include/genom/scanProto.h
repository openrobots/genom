/* scan.c */
extern int scan_type(FILE *in, FILE *out, char *format, void *ptr);
extern int scan_char(FILE *in, FILE *out, char *x, int indent, int nDim, int *dims);
extern int scan_unsigned_char(FILE *in, FILE *out, unsigned char *x, int indent, int nDim, int *dims);
extern int scan_short_int(FILE *in, FILE *out, short int *x, int indent, int nDim, int *dims);
extern int scan_int(FILE *in, FILE *out, int *x, int indent, int nDim, int *dims);
extern int scan_long_int(FILE *in, FILE *out, long int *x, int indent, int nDim, int *dims);
extern int scan_unsigned_short_int(FILE *in, FILE *out, unsigned short *x, int indent, int nDim, int *dims);
extern int scan_unsigned_int(FILE *in, FILE *out, unsigned int *x, int indent, int nDim, int *dims);
extern int scan_unsigned_long_int(FILE *in, FILE *out, unsigned long *x, int indent, int nDim, int *dims);
extern int scan_float(FILE *in, FILE *out, float *x, int indent, int nDim, int *dims);
extern int scan_double(FILE *in, FILE *out, double *x, int indent, int nDim, int *dims);
extern int scan_string(FILE *in, FILE *out, char *x, int indent, int nDim, int *dims);
extern int scan_string_len(FILE *in, FILE *out, char *x, int max_str_len, int indent, int nDim, int *dims);
