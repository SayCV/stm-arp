#include <stdlib.h>
#include "arpfir.h"

firinfo * init_fir(int * coefs, int ncoefs, int skip, int skipoff)
{
	int i;
	firinfo * filter;
	
	filter = (firinfo *)malloc(sizeof(firinfo));
	
	filter->ncoefs = ncoefs;
	filter->skip = skip;
	filter->skipct = skip-skipoff;
	
	filter->coefs = (int *)malloc(ncoefs * sizeof(int));
	for (i=0; i<ncoefs; i++)
		filter->coefs[i] = coefs[ncoefs-1-i];
	
	filter->history = (int *)malloc(ncoefs * sizeof(int));
	for (i=0; i<ncoefs; i++)
		filter->history[i] = 0;
	
	filter->index = 0;
	
	return filter;
}

int calc_fir(firinfo * f, int x)
{
	int i;
	int y;
	int * hitr;
	
	f->history[f->index] = x;
	f->index++;
	if (f->index == f->ncoefs)
		f->index = 0;
	
	y = 0;
	
	hitr = f->coefs;
	
// 	if (f->skipct == f->skip)
// 	{
		for (i=f->index; i<f->ncoefs; i++)
			y += *(hitr++) * f->history[i];
	
		for (i=0; i<f->index; i++)
			y += *(hitr++) * f->history[i];
		
// 		f->skipct = 0;
// 	}
	
// 	f->skipct++;
		
	return (y>>FIR_FBITS);
}

void destroy_fir(firinfo * filter)
{
	free(filter->coefs);
	free(filter->history);
	free(filter);
}