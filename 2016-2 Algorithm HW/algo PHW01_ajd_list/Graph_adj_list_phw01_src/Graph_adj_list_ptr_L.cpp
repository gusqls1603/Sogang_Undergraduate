#include "Graph_adj_list.h"
#include "Graph_adj_list_extern.h"
#include "Graph_adj_list_function.h"

// ptr_L management routine

void pushQ_ptr_L ( ptr_L **Q, int i ) {
  ptr_L *L;

  L    = Alloc_ptr_L();
  L->i = i;

  L->p = *Q;
  *Q   =  L;

  return;
}

int popQ_ptr_L ( ptr_L **Q ) {
  int i;
  ptr_L   *tQ;

  i    = (*Q)->i;
  tQ   = *Q;
  if ( (*Q)->p == NULL )
    *Q = NULL;
  else
    *Q = (*Q)->p;
  Free_ptr_L( tQ );
  return ( i );
}

ptr_L *addFIFO_ptr_L ( ptr_L **Q, ptr_L **Q_end, int sym ) {
  ptr_L *L;

  L    = Alloc_ptr_L();
  L->i = sym;
  if ( *Q == NULL ) 
    *Q = L;
  else 
    (*Q_end)->p = L;
  *Q_end = L;
  return ( L );
}

int getFIFO_ptr_L ( ptr_L **Q, ptr_L **Q_end ) {
  int sym;
  ptr_L *tQ;

  sym  = (*Q)->i;
  tQ   = *Q;
  if ( (*Q)->p == NULL ) {
    *Q = *Q_end = NULL;
  }
  else 
    *Q = (*Q)->p;
  Free_ptr_L( tQ );
  return ( sym );
}

ptr_L *Alloc_ptr_L( void ) {
  ptr_L *ptr;

  if ( ptr_L_pool == NULL ) {
    ptr = (ptr_L *)malloc(sizeof(ptr_L));
    if ( ptr == NULL ) {
      Error_Exit("No Memory in Alloc_ptr_L");
    }
    Used_Memory_for_ptr += sizeof(ptr_L);
    ptr->i = NONE;
  }
  else {
    ptr        = ptr_L_pool;
    ptr_L_pool = ptr->p;
  }
  ptr->p       = NULL;
  ++ptr_L_cnt;
  return(ptr);
}

void Free_ptr_L( ptr_L *ptr ) {
  if ( ptr->i == NONE ) {
    Error_Exit("This storage is already freed(Free_ptr_L).");
  }
  ptr->i     = NONE;
  ptr->p     = ptr_L_pool;
  ptr_L_pool = ptr;
  --ptr_L_cnt;
}

void Free_ptr_L_pool ( void ) {
  ptr_L *p;
  p = ptr_L_pool;
  while ( p != NULL ) {
    ptr_L_pool = p->p;
    free(p);
	Used_Memory_for_ptr -= sizeof(ptr_L);
    p = ptr_L_pool;
  }
  if ( ptr_L_cnt != 0 ) {
    Error_Exit("Non-zero ptr_L_cnt after deallocate ptr_L_pool.");
  }
  ptr_L_pool = NULL;
}