#define NONE      -1

typedef struct ptr_list{
  int i;
  struct ptr_list *p;
} ptr_L;

void pushQ_ptr_L ( ptr_L **Q, int i );
int popQ_ptr_L ( ptr_L **Q );
ptr_L *addFIFO_ptr_L ( ptr_L **Q, ptr_L **Q_end, int sym );
int getFIFO_ptr_L ( ptr_L **Q, ptr_L **Q_end );
ptr_L *Alloc_ptr_L( void );
void Free_ptr_L( ptr_L *ptr );
void Free_ptr_L_pool ( void );