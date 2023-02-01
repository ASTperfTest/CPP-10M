/* $Id: dynstuff.c 11259 2010-02-17 04:47:22Z borkra $ */

/*
*	(C) Copyright 2001-2003 Wojtek Kaniewski <wojtekka@irc.pl>
*							Dawid Jarosz <dawjar@poczta.onet.pl>
*
*	This program is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License Version 2 as
*	published by the Free Software Foundation.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program; if not, write to the Free Software
*	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "gg.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
* list_add_sorted()
*
* dodaje do listy dany element. przy okazji moїe teї skopiowaж zawarto¶ж.
* je¶li poda siк jako ostatni parametr funkcjк porуwnuj±c± zawarto¶ж
* elementуw, moїe posortowaж od razu.
*
*	- list - wskaјnik do listy,
*	- data - wskaјnik do elementu,
*	- alloc_size - rozmiar elementu, je¶li chcemy go skopiowaж.
*
* zwraca wskaјnik zaalokowanego elementu lub NULL w przpadku bікdu.
*/
void *list_add_sorted(list_t *list, void *data, int alloc_size, int(*comparision)(void *, void *))
{
	if (!list) {
		errno = EFAULT;
		return nullptr;
	}

	list_t newlist = (list_t)malloc(sizeof(struct list));

	newlist->data = data;
	newlist->next = nullptr;

	if (alloc_size) {
		newlist->data = malloc(alloc_size);
		memcpy(newlist->data, data, alloc_size);
	}

	list_t tmp;
	if (!(tmp = *list)) {
		*list = newlist;
	}
	else {
		if (!comparision) {
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = newlist;
		}
		else {
			list_t prev = nullptr;

			while (comparision(newlist->data, tmp->data) > 0) {
				prev = tmp;
				tmp = tmp->next;
				if (!tmp)
					break;
			}

			if (!prev) {
				tmp = *list;
				*list = newlist;
				newlist->next = tmp;
			}
			else {
				prev->next = newlist;
				newlist->next = tmp;
			}
		}
	}

	return newlist->data;
}

/*
* list_add()
*
* wrapper do list_add_sorted(), ktуry zachowuje poprzedni± skіadniк.
*/
void *list_add(list_t *list, void *data, int alloc_size)
{
	return list_add_sorted(list, data, alloc_size, nullptr);
}

/*
* list_remove()
*
* usuwa z listy wpis z podanym elementem.
*
*	- list - wskaјnik do listy,
*	- data - element,
*	- free_data - zwolniж pamiкж po elemencie.
*/
int list_remove(list_t *list, void *data, int free_data)
{
	list_t tmp, last = nullptr;

	if (!list || !*list) {
		errno = EFAULT;
		return -1;
	}

	tmp = *list;
	if (tmp->data == data) {
		*list = tmp->next;
	}
	else {
		for (; tmp && tmp->data != data; tmp = tmp->next)
			last = tmp;
		if (!tmp) {
			errno = ENOENT;
			return -1;
		}
		last->next = tmp->next;
	}

	if (free_data)
		free(tmp->data);
	free(tmp);

	return 0;
}

/*
* list_count()
*
* zwraca ilo¶ж elementуw w danej li¶cie.
*
*	- list - lista.
*/
int list_count(list_t list)
{
	int count = 0;

	for (; list; list = list->next)
		count++;

	return count;
}

/*
* list_destroy()
*
* niszczy wszystkie elementy listy.
*
*	- list - lista,
*	- free_data - czy zwalniaж bufor danych?
*/
int list_destroy(list_t list, int free_data)
{
	list_t tmp;

	while (list) {
		if (free_data)
			free(list->data);

		tmp = list->next;

		free(list);

		list = tmp;
	}

	return 0;
}

/*
* string_realloc()
*
* upewnia siк, їe w stringu bкdzie wystarczaj±co duїo miejsca.
*
*	- s - ci±g znakуw,
*	- count - wymagana ilo¶ж znakуw (bez koсcowego '\0').
*/
static void string_realloc(string_t s, int count)
{
	char *tmp;

	if (s->str && count + 1 <= s->size)
		return;

	tmp = (char*)realloc(s->str, count + 81);
	if (!s->str)
		*tmp = 0;
	tmp[count + 80] = 0;
	s->size = count + 81;
	s->str = tmp;
}

/*
* string_append_c()
*
* dodaje do danego ci±gu jeden znak, alokuj±c przy tym odpowiedni± ilo¶ж
* pamiкci.
*
*	- s - ci±g znakуw.
*	- c - znaczek do dopisania.
*/
int string_append_c(string_t s, char c)
{
	if (!s) {
		errno = EFAULT;
		return -1;
	}

	string_realloc(s, s->len + 1);

	s->str[s->len + 1] = 0;
	s->str[s->len++] = c;

	return 0;
}

/*
* string_append_n()
*
* dodaje tekst do bufora alokuj±c odpowiedni± ilo¶ж pamiкci.
*
*	- s - ci±g znakуw,
*	- str - tekst do dopisania,
*	- count - ile znakуw tego tekstu dopisaж? (-1 znaczy, їe caіy).
*/
int string_append_n(string_t s, const char *str, int count)
{
	if (!s || !str) {
		errno = EFAULT;
		return -1;
	}

	if (count == -1)
		count = (int)mir_strlen(str);

	string_realloc(s, s->len + count);

	s->str[s->len + count] = 0;
	strncpy(s->str + s->len, str, count);

	s->len += count;

	return 0;
}

int string_append(string_t s, const char *str)
{
	return string_append_n(s, str, -1);
}

/*
* string_insert_n()
*
* wstawia tekst w podane miejsce bufora.
*
*	- s - ci±g znakуw,
*	- index - miejsce, gdzie mamy wpisaж (liczone od 0),
*	- str - tekst do dopisania,
*	- count - ilo¶ж znakуw do dopisania (-1 znaczy, їe wszystkie).
*/
void string_insert_n(string_t s, int index, const char *str, int count)
{
	if (!s || !str)
		return;

	if (count == -1)
		count = (int)mir_strlen(str);

	if (index > s->len)
		index = s->len;

	string_realloc(s, s->len + count);

	memmove(s->str + index + count, s->str + index, s->len + 1 - index);
	memmove(s->str + index, str, count);

	s->len += count;
}

void string_insert(string_t s, int index, const char *str)
{
	string_insert_n(s, index, str, -1);
}

/*
* string_init()
*
* inicjuje strukturк string. alokuje pamiкж i przypisuje pierwsz± warto¶ж.
*
*	- value - je¶li NULL, ci±g jest pusty, inaczej kopiuje tam.
*
* zwraca zaalokowan± strukturк `string'.
*/
string_t string_init(const char *value)
{
	string_t tmp = (string_t)malloc(sizeof(struct string));

	if (!value)
		value = "";

	tmp->str = _strdup(value);
	tmp->len = (int)mir_strlen(value);
	tmp->size = (int)mir_strlen(value) + 1;

	return tmp;
}

/*
* string_clear()
*
* czy¶ci zawarto¶ж struktury `string'.
*
*	- s - ci±g znakуw.
*/
void string_clear(string_t s)
{
	if (!s)
		return;

	if (s->size > 160) {
		s->str = (char*)realloc(s->str, 80);
		s->size = 80;
	}

	s->str[0] = 0;
	s->len = 0;
}

/*
* string_free()
*
* zwalnia pamiкж po strukturze string i moїe teї zwolniж pamiкж po samym
* ci±gu znakуw.
*
*	- s - struktura, ktуr± wycinamy,
*	- free_string - zwolniж pamiкж po ci±gu znakуw?
*
* je¶li free_string=0 zwraca wskaјnik do ci±gu, inaczej NULL.
*/
char *string_free(string_t s, int free_string)
{
	char *tmp = nullptr;

	if (!s)
		return nullptr;

	if (free_string)
		free(s->str);
	else
		tmp = s->str;

	free(s);

	return tmp;
}

/*
* _itoa()
*
* prosta funkcja, ktуra zwraca tekstow± reprezentacjк liczby. w obrкbie
* danego wywoіania jakiej¶ funkcji lub wyraїenia moїe byж wywoіania 10
* razy, poniewaї tyle mamy statycznych buforуw. lepsze to niї ci±gіe
* tworzenie tymczasowych buforуw na stosie i sprintf()owanie.
*
*	- i - liczba do zamiany.
*
* zwraca adres do bufora, ktуrego _NIE_NALEЇY_ zwalniaж.
*/

const char *ditoa(long int i)
{
	static char bufs[10][16];
	static int index = 0;
	char *tmp = bufs[index++];

	if (index > 9)
		index = 0;

	mir_snprintf(tmp, 16, "%ld", i);
	return tmp;
}

/*
* array_make()
*
* tworzy tablicк tekstуw z jednego, rozdzielonego podanymi znakami.
*
*	- string - tekst wej¶ciowy,
*	- sep - lista elementуw oddzielaj±cych,
*	- max - maksymalna ilo¶ж elementуw tablicy. je¶li rуwne 0, nie ma
*			ograniczeс rozmiaru tablicy.
*	- trim - czy wiкksz± ilo¶ж elementуw oddzielaj±cych traktowaж jako
*			 jeden (na przykіad spacje, tabulacja itp.)
*	- quotes - czy pola mog± byж zapisywane w cudzysіowiach lub
*			   apostrofach z escapowanymi znakami.
*
* zaalokowan± tablicк z zaalokowanymi ci±gami znakуw, ktуr± naleїy
* zwolniж funkcj± array_free()
*/
char **array_make(const char *string, const char *sep, int max, int trim, int quotes)
{
	const char *p, *q;
	char **result = nullptr;
	int items = 0, last = 0;

	if (!string || !sep)
		goto failure;

	for (p = string; ; ) {
		int len = 0;
		char *token = nullptr;

		if (max && items >= max - 1)
			last = 1;

		if (trim) {
			while (*p && strchr(sep, *p))
				p++;
			if (!*p)
				break;
		}

		if (!last && quotes && (*p == '\'' || *p == '\"')) {
			char sep = *p;

			for (q = p + 1, len = 0; *q; q++, len++) {
				if (*q == '\\') {
					q++;
					if (!*q)
						break;
				}
				else if (*q == sep)
					break;
			}

			if ((token = (char*)calloc(1, len + 1))) {
				char *r = token;

				for (q = p + 1; *q; q++, r++) {
					if (*q == '\\') {
						q++;

						if (!*q)
							break;

						switch (*q) {
						case 'n':
							*r = '\n';
							break;
						case 'r':
							*r = '\r';
							break;
						case 't':
							*r = '\t';
							break;
						default:
							*r = *q;
						}
					}
					else if (*q == sep) {
						break;
					}
					else
						*r = *q;
				}

				*r = 0;
			}

			p = (*q) ? q + 1 : q;

		}
		else {
			for (q = p, len = 0; *q && (last || !strchr(sep, *q)); q++, len++);
			token = (char*)calloc(1, len + 1);
			strncpy(token, p, len);
			token[len] = 0;
			p = q;
		}

		result = (char**)realloc(result, (items + 2) * sizeof(char*));
		result[items] = token;
		result[++items] = nullptr;

		if (!*p)
			break;

		p++;
	}

failure:
	if (!items)
		result = (char**)calloc(1, sizeof(char*));

	return result;
}

/*
* array_count()
*
* zwraca ilo¶ж elementуw tablicy.
*/
int array_count(char **array)
{
	int result = 0;

	if (!array)
		return 0;

	while (*array) {
		result++;
		array++;
	}

	return result;
}

/*
* array_add()
*
* dodaje element do tablicy.
*/
void array_add(char ***array, char *string)
{
	int count = array_count(*array);

	*array = (char**)realloc(*array, (count + 2) * sizeof(char*));
	(*array)[count + 1] = nullptr;
	(*array)[count] = string;
}

/*
* array_join()
*
* і±czy elementy tablicy w jeden string oddzielaj±c elementy odpowiednim
* separatorem.
*
*	- array - wskaјnik do tablicy,
*	- sep - seperator.
*
* zwrуcony ci±g znakуw naleїy zwolniж.
*/
char *array_join(char **array, const char *sep)
{
	if (!array)
		return _strdup("");

	string_t s = string_init(nullptr);
	for (int i = 0; array[i]; i++) {
		if (i)
			string_append(s, sep);

		string_append(s, array[i]);
	}

	return string_free(s, 0);
}

/*
* array_contains()
*
* stwierdza, czy tablica zawiera podany element.
*
*	- array - tablica,
*	- string - szukany ci±g znakуw,
*	- casesensitive - czy mamy zwracaж uwagк na wielko¶ж znakуw?
*
* 0/1
*/
int array_contains(char **array, const char *string, int casesensitive)
{
	int i;

	if (!array || !string)
		return 0;

	for (i = 0; array[i]; i++) {
		if (casesensitive && !mir_strcmp(array[i], string))
			return 1;
		if (!casesensitive && !strcasecmp(array[i], string))
			return 1;
	}

	return 0;
}

/*
* array_free()
*
* zwalnia pamieж zajmowan± przez tablicк.
*/
void array_free(char **array)
{
	char **tmp;

	if (!array)
		return;

	for (tmp = array; *tmp; tmp++)
		free(*tmp);

	free(array);
}
