#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if in an SP, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int f_hd, r_hd; // adj list header(-1 if none)
} vertex;

void Error_Exit ( char *s );