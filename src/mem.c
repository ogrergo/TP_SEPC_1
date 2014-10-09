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

int index_min = 0;

uintptr_t TZL[WBUDDY_MAX_INDEX];

unsigned long SIZE[WBUDDY_MAX_INDEX];
int SUBBUDDY[WBUDDY_MAX_INDEX];

unsigned long get_max_bloc_size()
{
	int i = WBUDDY_MAX_INDEX - 1;
	while(i > 0 && !TZL[i])
		i--;
	return SIZE[i];
}


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
		printf("|%02d|%5d|%7luo|\n", i, c, SIZE[i]);
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
		//printf("%d, S:%lu + B:%d\n",i, SIZE[i], SUBBUDDY[i]);
		//printf("%d, S:%lu + B:%d\n",i + 1, SIZE[i+1], SUBBUDDY[i+1]);
	}

	index_min = 0;
	while(sizeof(uintptr_t) > SIZE[index_min])
		index_min++;

	index_min += 1 - index_min%2; //pour rendre l'index impair (on ne peut créer de bloc de taille 3*2^k trop petit sans générer un bloc de taille 2^i plus petit)
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
	*ptr = TZL[SUBBUDDY[index]];
	TZL[SUBBUDDY[index]] = (uintptr_t)ptr;
	/*gros*/
	ptr = (uintptr_t*) ((uintptr_t)ptr + SIZE[SUBBUDDY[index]]);
	*ptr = TZL[index - 1];
	TZL[index - 1] = (uintptr_t)ptr;

	return 0;
}

int find_bloc_by_index(int index)
{
	if(index >= WBUDDY_MAX_INDEX)
		return -1;

	if(index < index_min)
		printf("erreur trop petit bloc a obtenir !!!!! %d vs %dmin", index, index_min);
	//printf(" Recherche d'un bloc de taille %lu, d'indice %d\n",SIZE[index], index);

	if(TZL[index])
		return 0;

	//on doit decoupper un bloc plus grand
	if(index % 2 == 1) {
		if(!split(index + 4)) //de la forme 2k -> 3*2(k-2) + 2(k-2)
			return 0;
		if(SUBBUDDY[index] >= index_min) //pour eviter de créer des blocs trop petit
			if(!split(index + 1)) //de la forme 3*2k -> 2^k + 2^(k+1)
				return 0;
		if(!split(index + 3)) //la meme mais inverse
			return 0;
	} else {
		if(SUBBUDDY[index] >= index_min) //pour eviter de créer des blocs trop petit
			if(!split(index + 1))
				return 0;
	}

	//printf("pas de bloc candidat pour l'index %d\n", index);
	return -1;

}
int find_indice(unsigned long size)
{
	if(size <= sizeof(uintptr_t))
	{
		return index_min;
	}
	int min = 0;
	int max = WBUDDY_MAX_INDEX - 1;
	while(min != max - 1) {
		//printf("(min:%d,max%d) ->", min, max);
		if(SIZE[(min + max) / 2] < size)
			min = (min + max) / 2;
		else
			max = (min + max) / 2;
	}//printf(" (max:%d)\n", max);

	if(SUBBUDDY[max] < index_min && max%2 == 0)
		max++;

	return max;
}

void * mem_alloc(unsigned long size)
{
	if(size == 0 || size > ALLOC_MEM_SIZE)
		return NULL;

	int indice = find_indice(size);

	if(find_bloc_by_index(indice))
		return NULL;

	uintptr_t * ptr = (uintptr_t*) TZL[indice];
	TZL[indice] = *ptr;

	return (void*)ptr;
}

void find_buddy(uintptr_t ptr, int indice, uintptr_t *ptr_buddy, int *ind_buddy)
{
	uintptr_t min = (uintptr_t) zone_memoire;
	uintptr_t max = min + (uintptr_t)ALLOC_MEM_SIZE;
	int ind_cur = WBUDDY_MAX_INDEX - 1;

	*ptr_buddy = (uintptr_t) zone_memoire;
	unsigned long buddysize = ALLOC_MEM_SIZE;

	while(ind_cur > indice - 3) {
		//printf("++ min:%p, max:%p, ind_curr:%2d   ->", (uintptr_t*) min, (uintptr_t*) max, ind_cur);
		if(min == ptr && ind_cur == indice)
			break;
		if((min + SIZE[SUBBUDDY[ind_cur]]) <= ptr) {
			//printf("passage dans le plus grand\n");
			*ptr_buddy = min;
			buddysize = SIZE[SUBBUDDY[ind_cur]];
			min = min + SIZE[SUBBUDDY[ind_cur]];
			ind_cur--;
		} else {
		//	printf("passage dans le buddy(plus petit)\n");
			buddysize = SIZE[ind_cur - 1];
			max = min + SIZE[SUBBUDDY[ind_cur]];
			*ptr_buddy = max;
			ind_cur = SUBBUDDY[ind_cur];
		}
	}
	*ind_buddy = find_indice(buddysize);
}

uintptr_t* check_used(uintptr_t ptr, int indice)
{
	uintptr_t i = TZL[indice];
	if(!i)
		return NULL;

	if(i == ptr)
		return &(TZL[indice]);

	while(*(uintptr_t*) i) {
		if(*(uintptr_t*)i == ptr) {
			return (uintptr_t*) i;
		}
		i = *(uintptr_t*)i;
	}

	return NULL;
}

void liberation(uintptr_t ptr, int indice) {


	int ind_buddy;
	uintptr_t ptr_buddy;
	find_buddy(ptr, indice, &ptr_buddy, &ind_buddy);

	uintptr_t * container = check_used(ptr_buddy, ind_buddy);

	/*on efface ptr*/
	*(uintptr_t*) ptr = (uintptr_t) NULL;

	if(container) {
		/*le buddy est libre*/
		*container = *(uintptr_t*) ptr_buddy;
		*(uintptr_t*) ptr_buddy = (uintptr_t) NULL;
	//	printf("(%d, %d) -> %d;",ind_buddy,indice ,find_indice(SIZE[ind_buddy] + SIZE[indice]));
		liberation(
			(ptr_buddy < ptr ? ptr_buddy: ptr),
			find_indice(SIZE[ind_buddy] + SIZE[indice]));
	} else {
		*(uintptr_t*) ptr = TZL[indice];
		TZL[indice] = ptr;
	}
}

int mem_free(void *ptr, unsigned long size)
{
	if((uintptr_t) ptr >= (uintptr_t) zone_memoire
		&& (uintptr_t)ptr < (uintptr_t) zone_memoire + ALLOC_MEM_SIZE
		&& size <= ALLOC_MEM_SIZE)
	{
		int indice = find_indice(size);
		liberation((uintptr_t) ptr, indice);
		return 0;
	} else {
		return -1;
	}
}


int mem_destroy()
{
	memset(TZL, 0, WBUDDY_MAX_INDEX * sizeof(uintptr_t));
	free(zone_memoire);
	zone_memoire = 0;
	return 0;
}
