#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>

#define NONE      -1

typedef struct ptr_list{
  int i;
  struct ptr_list *p;
} ptr_L;

typedef struct vertex {
  int   name; // may have more information
  bool  flag; 
  ptr_L *p;   // adjacent list
} vertex;

typedef struct edge {
  int  name;  // may have more information
  int  cost;	
  bool flag;
  int  vf, vr;
} edge;

