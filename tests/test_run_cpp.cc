/*****************************************************
 * Copyright Grégory Mounié 2008-2013                *
 * This code is distributed under the GLPv3 licence. *
 * Ce code est distribué sous la licence GPLv3+.     *
 *****************************************************/

#include <unistd.h>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>
#include <functional>

#include "test_run.H"


#ifndef ALLOC_MEM_SIZE
#error "#define ALLOC_MEM_SIZE XXX n'est pas defini dans mem.h ! Interruption de la compilation"
#endif


/* options */
static bool be_verbose = false; 

/* manipulation de la liste des allocations */


void doAlloc ::operator() (allocat &t)
{
  static int nb=0;

  assert (t.size > 0);
  
  t.adr = mem_alloc (t.size);


  if ( t.adr == (char *)0 )
    {
      /* l'allocation a echoue */
      printf ("Une allocation a echoue ! taille a aloué|taille max des blocs dispo : |%9d|%9lu| \n", t.size ,get_max_bloc_size());
      
      t.adr = 0;
    }
  else
    {
      t.idx = nb;
      nb ++;
      if (be_verbose)
	printf ("alloc %d (taille: %d): %p\n", t.idx, t.size, t.adr);
      
      /* ecrire dans le bloc */
      memset (t.adr, 0, t.size );
    }
  
  
}

void doLiberer ::operator() (allocat &t)
{
  assert (t.size > 0);
  
  if (t.adr)
    {
      /* ecrire dans le bloc */
      memset (t.adr, 0, t.size );

      if (be_verbose)
	printf ("libere %d (taille: %d): %p\n", t.idx, t.size, t.adr);
      
      mem_free (t.adr, t.size);
      t.adr = 0;
    }
}

void random_run_cpp(int nb=100, bool verbose=false)
{
  be_verbose = verbose;
  
  
  vector<allocat> liste_allocation;
    
  /* choisir les allocations */
  fillList_fibo<ALLOC_MEM_SIZE> ( liste_allocation );
    
  /* afficher l'etat de la memoire */
    
  /* faire les allocations */
  for_each (liste_allocation.begin(), liste_allocation.end(), doAlloc());
  
  /* melanger la liste */
  random_shuffle (liste_allocation.begin (), liste_allocation.end ());
  
  /* faire les deallocations */
  for_each ( liste_allocation.begin (), liste_allocation.end (), doLiberer ());
  
}

