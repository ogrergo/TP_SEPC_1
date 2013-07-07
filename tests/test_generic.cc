/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <sys/time.h>
#include <sys/resource.h>

#include <gtest/gtest.h>

#include "mem.h"
#include "test_run_cpp.h"
#include "test_generic.h"

TEST(Init, noinit) {
  ASSERT_EQ( mem_alloc(64), 0 );
}

TEST(Init, initdestroy) {
  ASSERT_EQ(  mem_init(), 0 );
  ASSERT_EQ( mem_destroy(), 0);
}

TEST(Init, oneallocloop) {
  for(int i=0; i< 100; i++) {
    void *m1=0;
    ASSERT_EQ( mem_init(), 0 );
    m1  = mem_alloc(64);
    ASSERT_NE( m1, 0 );
    memset(m1, i, 64);
    ASSERT_EQ( mem_free(m1, 64), 0 );
    ASSERT_EQ( mem_destroy(), 0);
  } 
}

class BaseMemTest : public ::testing::Test {
public:
  virtual void SetUp() {
    ASSERT_EQ(  mem_init(), 0 );
  }
  virtual void TearDown() {
    ASSERT_EQ( mem_destroy(), 0);
  }
}


TEST_F(BaseMemTest, allocnone) {
  ASSERT_EQ( mem_alloc(0), 0);
}

TEST_F(BaseMemTest, allocall) {
  void * m1 = mem_alloc(ALLOC_MEM_SIZE);
  ASSERT_NE( m1, 0 );
  memset(m1, 1, ALLOC_MEM_SIZE);
  void * m2 =  mem_alloc(1);
  ASSERT_EQ( m2, 0 );
  ASSERT_EQ(mem_free(m1, ALLOC_MEM_SIZE), 0 );
  void * m3 = mem_alloc(ALLOC_MEM_SIZE);
  ASSERT_EQ( m1, m3);
  memset(m1, 2, ALLOC_MEM_SIZE);
  ASSERT_EQ( mem_free(m2, ALLOC_MEM_SIZE), 0 );
}

TEST_F(BaseMemTest, boundaries) { 
  void * m1=  mem_alloc(ALLOC_MEM_SIZE);
  ASSERT_NE( m1, 0 );
  ASSERT_EQ( mem_free(m1, ALLOC_MEM_SIZE), 0 );
  
  ASSERT_NE( mem_free( (void *)-1, ALLOC_MEM_SIZE), 0 );
  void *m2= m1 -1;
  ASSERT_NE( mem_free( m2, ALLOC_MEM_SIZE), 0 );
  void *m3= m1 + ALLOC_MEM_SIZE + 1;
  ASSERT_NE( mem_free( m3, ALLOC_MEM_SIZE), 0 );
}

TEST_F(BaseMemTest, intervals) {
  void *mref = mem_alloc(ALLOC_MEM_SIZE);
  ASSERT_NE( mref, 0);
  memset(mref, 6, ALLOC_MEM_SIZE);
  ASSERT_EQ( mem_free( mref, ALLOC_MEM_SIZE ), 0 );

  void *m1 = mem_alloc(64);
  ASSERT_NE( m1, 0 );
  memset(m1, 6, 64);

  void *m2 = mem_alloc(64);
  ASSERT_NE( m2, 0 );
  memset(m2, 6, 64);

  ASSERT_GE( m1, mref );
  ASSERT_LT( m1, mref+ALLOC_MEM_SIZE );
  ASSERT_GE( m2, mref );
  ASSERT_LT( m2, mref+ALLOC_MEM_SIZE );

  ASSERT_EQ( mem_free( m1, 64 ), 0 );
  ASSERT_EQ( mem_free( m2, 64 ), 0 );
}

TEST_F(BaseMemTest, bouclefreepairimpair) {
  int nb=2*50;
  void *tab[nb];

  ASSERT_LT( 64*nb,  ALLOC_MEM_SIZE );
  
  for(int i=0; i < nb; i++)
    {
      tab[i] = mem_alloc(64);
      ASSERT_NE( tab[i], 0 );
      memset(tab[i], 3, 64);
    }
  for(int i=0; i < nb; i+=2)
    {
      ASSERT_EQ( mem_free( tab[i], 64 ), 0 );
    }
  for(int i=1; i < nb; i+=2)
    {
      ASSERT_EQ( mem_free( tab[i], 64 ), 0 );
    }

  void *m1 = mem_alloc(ALLOC_MEM_SIZE);
  ASSERT_NE( m1, 0 );
  memset(m1, 4, ALLOC_MEM_SIZE);
  ASSERT_EQ( mem_free( m1, ALLOC_MEM_SIZE ), 0 );
}

TEST_F(BaseMemTest, petitetaille) {
  int multi= 0;

#ifdef BUDDY
  multi=1;
#endif
#ifdef CFF
  multi=2;
#endif
#ifdef BF
  multi=2;
#endif

  ASSERT_GT(multi, 0);

  

void test_small()
{
  void *m1, *m2, *m3;
  int multi = 0;
  
#ifdef BUDDY
  multi=1;
#endif
#ifdef CFF
  multi=2;
#endif
#ifdef BF
  multi=2;
#endif

  CU_ASSERT(multi > 0);

  CU_ASSERT_EQUAL_FATAL( mem_init(), 0 );
  CU_ASSERT_PTR_NOT_NULL_FATAL( m1 = mem_alloc(1) );
  memset( m1, 0, 1);

  CU_ASSERT_PTR_NOT_NULL_FATAL( m2 = mem_alloc(1) );
  memset( m2, 0, 1);

  if (m2 > m1)
    { CU_ASSERT_FATAL( (m2-m1) >= multi*sizeof(void*) ); }
  else
    { CU_ASSERT_FATAL( (m1-m2) >= multi*sizeof(void*) ); }

  CU_ASSERT_EQUAL_FATAL( mem_free( m1, 1 ), 0 );
  CU_ASSERT_EQUAL_FATAL( mem_free( m2, 1 ), 0 );
  

  CU_ASSERT_PTR_NOT_NULL_FATAL( m3 = mem_alloc(ALLOC_MEM_SIZE) );
  memset(m3, 4, ALLOC_MEM_SIZE);
  CU_ASSERT_EQUAL_FATAL( mem_free( m3, ALLOC_MEM_SIZE ), 0 );

  CU_ASSERT_EQUAL_FATAL( mem_destroy(), 0);  
}

void test_random()
{
  int i;
  void *mref;

  CU_ASSERT_EQUAL_FATAL( mem_init(), 0 );
  CU_ASSERT_PTR_NOT_NULL_FATAL( mref = mem_alloc(ALLOC_MEM_SIZE) );
  memset(mref, 4, ALLOC_MEM_SIZE);
  CU_ASSERT_EQUAL_FATAL( mem_free( mref, ALLOC_MEM_SIZE ), 0 );

  for(i=0; i< 10; i++)
    {
      void *m1;
      random_run_cpp(100, false);
      CU_ASSERT_PTR_NOT_NULL_FATAL( m1 = mem_alloc(ALLOC_MEM_SIZE) );
      memset(m1, 4, ALLOC_MEM_SIZE);
      CU_ASSERT_EQUAL_FATAL( mem_free( m1, ALLOC_MEM_SIZE ), 0 );
    }

  CU_ASSERT_EQUAL_FATAL( mem_destroy(), 0);  
}
