/*
 * contrib/seg/segdata.h
 */
typedef struct SEG
{
	float4		lower;
	float4		upper;
	char		l_sigd;
	char		u_sigd;
	char		l_ext;
	char		u_ext;
} SEG;

/* in seg.c */
extern int	significant_digits(char *str);

/* in segscan.l */
extern int	seg_yylex(void);
extern void seg_yyerror(SEG *result, const char *message) mdb_attribute_noreturn();
extern void seg_scanner_init(const char *str);
extern void seg_scanner_finish(void);

/* in segparse.y */
extern int	seg_yyparse(SEG *result);
