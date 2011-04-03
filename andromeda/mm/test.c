#ifdef MMTEST
#include <mm/heap.h>

struct small
{
  int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p;
};
struct large
{
  int a[1024];
};

typedef struct small small_t;
typedef struct large large_t;

void wait()
{
  int i;
  for (i = 0; i < 0x1FFFFFFF; i++)
  {
  }
}

void testAlloc()
{
  large_t* a = alloc(sizeof(large_t), FALSE);
  large_t* b = alloc(sizeof(large_t), TRUE);
  
  small_t* c = alloc(sizeof(small_t), FALSE);
  small_t* d = alloc(sizeof(small_t), TRUE);
  
  examineHeap();
  wait();
  
  free(a);
  free(c);
  
  examineHeap();
  wait();
  
  a = alloc(sizeof(large_t), TRUE);
  c = alloc(sizeof(small_t), TRUE);
  
  examineHeap();
  wait();
  
  free(a);
  free(b);
  free(c);
  free(d);
  
  examineHeap();
  wait();
}

#endif