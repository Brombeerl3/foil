#include <stdio.h>
#include "defns.i"

extern int	VERBOSITY;


	/*  Protected memory allocation routines on call for memory which
	    is not allocated rather than let program continue erroneously */

void  *pmalloc(unsigned arg)
/*     -------  */
{
    void *p;

    p = (void *) malloc(arg);
    if( p || !arg ) return p;
    printf("\n*** malloc erroneously returns NULL\n");
    exit(1);
}



void  *prealloc(void * arg1, unsigned arg2)
/*     --------  */
{
    void *p;

    if ( arg1 == Nil ) return pmalloc(arg2);

    Verbose(10) printf("\n@%x realloc %d\n", arg1, arg2);
    p = (void *)realloc(arg1,arg2); 
    if( p || !arg2 ) return p;
    printf("\n*** realloc erroneously returns NULL\n");
    exit(1);
}



void  *pcalloc(unsigned arg1, unsigned arg2)
/*     -------  */
{
    void *p;

    p = (void *)calloc(arg1,arg2);
    if( p || !arg1 || !arg2 ) return p;
    printf("\n*** calloc erroneously returns NULL\n");
    exit(1);
}



void  pfree(void *arg)
{
    free(arg);
}



float  CPUTime()
{
    struct tms usage;

    times(&usage);

    return (usage.tms_utime + usage.tms_stime) / 60.0;
}
