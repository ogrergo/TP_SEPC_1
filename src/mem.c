/*****************************************************
* Copyright Grégory Mounié 2008-2013                *
* This code is distributed under the GLPv3 licence. *
* Ce code est distribué sous la licence GPLv3+.     *
*****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "mem.h"

/** squelette du TP allocateur memoire */

void *zone_memoire = 0;

uintptr_t TZL[WBUDDY_MAX_INDEX];

unsigned long SIZE[WBUDDY_MAX_INDEX];
int SUBBUDDY[WBUDDY_MAX_INDEX];


void affiche_bloc()
{
	unsigned int size = 0;
	for(int i = 0; i < WBUDDY_MAX_INDEX; i++) {
		int c = 0;
		uintptr_t *ptr = (uintptr_t*)TZL[i];
		while(ptr != NULL) {
			c++;
			ptr = (uintptr_t*)*ptr;
		}
		size += c * SIZE[i];
		printf("|%02d|%5d|\n", i, c);
	}
	printf("Espace disponible %u\n", size);
}

int mem_init()
{
	if (! zone_memoire)
		zone_memoire = (void *) malloc(ALLOC_MEM_SIZE);
	if (zone_memoire == 0)
	{
		perror("mem_init:");
		return -1;
	}

	memset(zone_memoire, 0, ALLOC_MEM_SIZE);

	/*TZL*/
	memset(TZL, 0, WBUDDY_MAX_INDEX * sizeof(uintptr_t));
	TZL[WBUDDY_MAX_INDEX - 1] = (uintptr_t)zone_memoire;

	/*SIZE and SUBBUDDY*/
	SIZE[0] = 1;
	SIZE[1] = 2;
	SIZE[2] = 3;
	SIZE[3] = 4;
	for(int i = 4; i < WBUDDY_MAX_INDEX; i = i + 2) {
		SIZE[i]     = 3 * SIZE[i - 3];
		SIZE[i + 1] = 1 << (i/2 + 1);
		SUBBUDDY[i]     = i - 3;
		SUBBUDDY[i + 1] = i - 3;
//		printf("%d, S:%lu + B:%d\n",i, SIZE[i], SUBBUDDY[i]);
//		printf("%d, S:%lu + B:%d\n",i + 1, SIZE[i+1], SUBBUDDY[i+1]);
	}

	return 0;
}

int split(int index)
{
	if(find_bloc_by_index(index))
		return -1;

	uintptr_t * ptr = (uintptr_t*)TZL[index];
	TZL[index] = *ptr;
	
	//splitting
	/*petit*/
	printf("Destruction de %d\n", index);
	*ptr = TZL[SUBBUDDY[index]];
	TZL[SUBBUDDY[index]] = (uintptr_t)ptr;
	printf("Création de index %d\n", SUBBUDDY[index]);
	/*gros*/
	ptr = (uintptr_t*) ((uintptr_t)ptr + SIZE[SUBBUDDY[index]]);
	*ptr = TZL[index - 1];
	TZL[index - 1] = (uintptr_t)ptr;
	printf("Création de index %d\n", index - 1);

	return 0;
}

int find_bloc_by_index(int index)
{
	if(index >= WBUDDY_MAX_INDEX)
		return -1;
	
	printf(" Recherche d'un bloc de taille %lu, d'indice %d\n",SIZE[index], index);	
	
	if(TZL[index])
		return 0;
	
	//on doit decoupper un bloc plus grand
	if(index % 2 == 1) {
		if(!split(index + 4)) //de la forme 2k -> 3*2(k-2) + 2(k-2)
			return 0;
		if(!split(index + 1)) //de la forme 3*2k -> 2^k + 2^(k+1)
			return 0;
		if(!split(index + 3)) //la meme mais inverse
			return 0;
	} else {
		if(!split(index + 1))
			return 0;
	}
	
	printf("pas de bloc candidat pour l'index %d\n", index);
	return -1;

}

void * mem_alloc(unsigned long size)
{
	if(size < sizeof(uintptr_t) || size > ALLOC_MEM_SIZE)
		return NULL;

	int min = 0;
	int max = WBUDDY_MAX_INDEX - 1;
	while(min != max - 1) {
		if(SIZE[(min + max) / 2] <= size)
			min = (min + max) / 2;
		else
			max = (min + max) / 2;
	}
	//indice = max
	if(find_bloc_by_index(max))
		return NULL;
	
	uintptr_t * ptr = (uintptr_t*) TZL[max];
	TZL[max] = *ptr;

	return (void*)ptr;  
}

int mem_free(void *ptr, unsigned long size)
{
	/* ecrire votre code ici */
	return 0;
}


int mem_destroy()
{
	memset(TZL, 0, ALLOC_MEM_SIZE);
	free(zone_memoire);
	zone_memoire = 0;
	return 0;
}

