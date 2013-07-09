/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <gtest/gtest.h>

#include "../src/mem.h"

TEST( Variantes, cff ) {
  int multi = 0;
  int i;
  int S8 = ALLOC_MEM_SIZE/8;
  void *tab[8]={};
  
#ifndef CFF
  return;
#else
  multi=1;
#endif

  ASSERT_GT(multi, 1);

  ASSERT_EQ( mem_init(), 0 );

  for(i=0; i < 8; i++)
    {
      tab[i] = mem_alloc(S8);
      ASSERT_NE( tab[i], 0 );
      memset(tab[i], 3, S8);
    }

  for(i=1; i < 8; i+= 2)
    ASSERT_EQ( mem_free( tab[i], S8 ), 0 );

  ASSERT_EQ( mem_alloc(3*S8), 0 );
  ASSERT_EQ( mem_free( tab[2], S8 ), 0 );
  tab[2] = mem_alloc(3*S8);
  ASSERT_NE( tab[2], 0 );
  memset(tab[2], 3, 3*S8);

  ASSERT_EQ( mem_free( tab[6], S8 ), 0 );
  tab[6] = mem_alloc(3*S8);
  ASSERT_NE( tab[6], 0 );
  memset(tab[6], 3, 3*S8);

  ASSERT_EQ( mem_free( tab[2], 3*S8 ), 0 );
  ASSERT_EQ( mem_free( tab[6], 3*S8 ), 0 );
  ASSERT_EQ( tab[2] = mem_alloc(7*S8) );

  ASSERT_EQ( mem_free( tab[4], S8 ), 0 );
  tab[4] = mem_alloc(7*S8);
  ASSERT_NE( tab[4], 0);
  memset(tab[4], 3, 7*S8);

  ASSERT_EQ( mem_free( tab[4], 7*S8 ), 0 );
  ASSERT_EQ( mem_free( tab[0], S8 ), 0 );

  ASSERT_EQ( mem_destroy(), 0);
}

