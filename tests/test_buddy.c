/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <CUnit/CUnit.h>

#include "mem.h"

void test_buddy()
{
  void *m1=0, *m2=0, *mref=0;
  int multi = 0;
  
#ifndef BUDDY
  return;
#else
  multi = 1;
#endif

  CU_ASSERT_FATAL(multi > 0);

  CU_ASSERT_EQUAL_FATAL( mem_init(), 0 );

  CU_ASSERT_PTR_NOT_NULL_FATAL( mref = mem_alloc(ALLOC_MEM_SIZE) );
  memset(mref, 4, ALLOC_MEM_SIZE);
  CU_ASSERT_EQUAL_FATAL( mem_free( mref, ALLOC_MEM_SIZE ), 0 );

 
  CU_ASSERT_PTR_NOT_NULL_FATAL( m1 = mem_alloc(64) );
  memset( m1, 0, 1);

  CU_ASSERT_PTR_NOT_NULL_FATAL( m2 = mem_alloc(64) );
  memset( m2, 0, 1);

  CU_ASSERT_EQUAL_FATAL( (m1-mref)^(m2-mref), 64 );

  CU_ASSERT_EQUAL_FATAL( mem_free( m1, 64 ), 0 );
  CU_ASSERT_EQUAL_FATAL( mem_free( m2, 64 ), 0 );
  

  CU_ASSERT_PTR_NOT_NULL_FATAL( mref = mem_alloc(ALLOC_MEM_SIZE) );
  memset(mref, 4, ALLOC_MEM_SIZE);
  CU_ASSERT_EQUAL_FATAL( mem_free( mref, ALLOC_MEM_SIZE ), 0 );
  CU_ASSERT_EQUAL_FATAL( mem_destroy(), 0);

}
