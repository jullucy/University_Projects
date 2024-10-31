/**
 * "Pętlik"
 *
 * Program zaliczeniowy na zajęcia ze Wstępu do programowania.
 *
 * Kompilacja poleceniem:
 *
 * gcc @opcje pentlik.c -o petlik
 *
 * Program umożliwia realizajcję języka Pętlik.
 *
 * Czyta i wykonuje polecenie:
 * wypisania wartości zmiennej;
 * wykonania programu w języku Pętlik.
 */


#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>


/* manipulacje tablicami */

/**
 * Tworzy tablicę zmiennych.
 */
int **stworz_tablice(void) {
  int **tab;
  tab = (int**) malloc(26 * sizeof(int*));
  for (int i = 0; i < 26; ++i) {
    tab[i] = (int*) calloc(3, sizeof(int));
  }
  return tab;
}

/**
 * Tworzy tablicę rozmiarów.
 */
int *tab_rozmiarow(void) {
  int *tab_r;
  tab_r = (int*) malloc(26 * sizeof(int));
  for (int i = 0; i < 26; ++i) {
    tab_r[i] = 3;
  }
  return tab_r;
}

/**
 * Zwalnia pamięć zaalokowaną na tablicę 'tab'.
 */
void free_tab(int **tab) {
  for (int i = 0; i < 26; ++i) {
    free(tab[i]);
  }
  free(tab);
}

/**
 * O ile jest to możliwe zwiększa dwukrotnie wielkość tablicy.
 *
 * Jeśli nie jest to możliwe zmienia wiekość tablicy na INT_MAX.
 */
void zwieksz(char **tab, int *size) {
  int pom = *size;
  if (2 * (*size) < INT_MAX) {
    *size = 2 * (*size);
  }
  else {
    *size = INT_MAX;
  }
  *tab = (char*) realloc (*tab, (unsigned int) *size * sizeof(char));
  while (pom < *size) {
    (*tab)[pom] = 0;
    ++pom;
  }
}

/**
 * Zwiększa o jeden wielkość tablicy 'tab'.
 */
void zwieksz_o_jeden(int **tab, int *size) {
  *size = *size + 1;
  *tab = (int*) realloc (*tab, (unsigned int) *size * sizeof(int));
  (*tab)[*size - 1] = 0;
}

/**
 * Zmienia wielkość tablicy 'tab' z 'size' na 'size_2'.
 */
void zmien_rozmiar(int **tab, int *size, int size_2) {
  int pom = *size;
  *tab = (int*) realloc (*tab, (unsigned int) size_2 * sizeof(int));
  while (pom < size_2) {
    (*tab)[pom] = 0;
    ++pom;
  }
  *size = size_2;
}


/* działania na zmiennych */

/**
 * Dodaje jeden do zmiennej zapisanej w tablicy 'zmienna'.
 *
 * 'size' to wielkość tablicy 'zmienna'.
 */
void dodanie_jeden(int **zmienna, int *size) {
  int i = 0;
  int dodatek = 1;
  while (dodatek != 0) {
    if (i == *size) {
      zwieksz_o_jeden(zmienna, size);
    }
    (*zmienna)[i] = (*zmienna)[i] + 1;
    dodatek = 0;
    if ((*zmienna)[i] == 10) {
      (*zmienna)[i] = 0;
      dodatek = 1;
    }
    ++i;
  }
}

/**
 * Przekazuje odpowiednią zmienną do funkcji 'dodanie_jeden'.
 */
int dodawanie(int ***zmienne, int **ile, char **polecenie, int i) {
  char zmienna = (*polecenie)[i];
  ++i;
  int ktora = zmienna - 'a';
  dodanie_jeden(&(*zmienne)[ktora], &(*ile)[ktora]);
  return i;
}

/**
 * Zmniejsza o jeden wartość zmiennej 'zmienna'.
 */
void zmniejsz(int **zmienna) {
  int i = 0;
  while ((*zmienna)[i] == 0) {
    (*zmienna)[i] = 9;
    ++i;
  }
  (*zmienna)[i] = (*zmienna)[i] - 1;
}

/**
 * Zeruje zmienną 'zmienna'.
 */
void wyzeruj(int **zmienna, int *size) {
  *zmienna = realloc (*zmienna, 3 * sizeof(int));
  (*zmienna)[0] = 0;
  (*zmienna)[1] = 0;
  (*zmienna)[2] = 0;
  *size = 3;
}

/**
 * Dodaje wartość z tablicy 'zmienna' do tablicy 'zmienna_zw'.
 *
 * 'ile' to wielkość tablicy 'zmienna'.
 *
 * 'ile_zw' to wielkość tablicy 'zmienna_zw'.
 */
void przekaz(int **zmienna, int **zmienna_zw, int *ile, int *ile_zw) {
  int dodatek = 0;
  int i = 0;
  if (*ile > *ile_zw) {
    zmien_rozmiar(zmienna_zw, ile_zw, *ile);
  }
  while (i < *ile) {
    (*zmienna_zw)[i] = (*zmienna_zw)[i] + (*zmienna)[i] + dodatek;
    if ((*zmienna_zw)[i] >= 10) {
      (*zmienna_zw)[i] = (*zmienna_zw)[i] - 10;
      dodatek = 1;
    }
    else {
      dodatek = 0;
    }
    ++i;
  }
  while (dodatek > 0) {
    if (i == *ile_zw) {
      zwieksz_o_jeden(zmienna_zw, ile_zw);
    }
    (*zmienna_zw)[i] = (*zmienna_zw)[i] + 1;
    if ((*zmienna_zw)[i] >= 10) {
      (*zmienna_zw)[i] = (*zmienna_zw)[i] - 10;
      dodatek = 1;
    }
    else {
      dodatek = 0;
    }
    ++i;
  }
}


/* operacje na kodzie */

/**
 * Sprawdza czy wartość zmiennej jest równa 0.
 */
bool czy_koniec(int **zmienne, int ile) {
  bool signal = false;
  int i = 0;
  while (i < ile && !signal) {
    if ((*zmienne)[i] > 0) {
      signal = true;
    }
    ++i;
  }
  return signal;
}

/**
 * Szuka końca wyrażenia nawiasowego i przekazuje indeks bezpośrednio za nim.
 */
int koniec_w_nawiasowego(char **polecenie, int i) {
  int liczba_naw = 1;
  while (liczba_naw > 0) {
    if ((*polecenie)[i] == ')') {
      --liczba_naw;
    }
    else if ((*polecenie)[i] == '(') {
      ++liczba_naw;
    }
    ++i;
  }
  return --i;
}

/**
 * Sprawdza jaki kod wygenerować.
 *
 * Przekazuje wartość true jeśli ma zostać wygenerowany kod zoptymalizowany.
 *
 * Przekazuje wartość false jeśli ma zostać wygenerowany kod zwykły.
 */
bool jaki_kod(char **polecenie, int indeks) {
  bool signal = true;
  int i = indeks;
  while ((*polecenie)[i] != ')' && signal)  {
    if ((*polecenie)[i] == '(') {
      signal = false;
    }
    ++i;
    if ((*polecenie)[i] == (*polecenie)[indeks]) {
      signal = false;
    }
  }
  return signal;
}


/* wykonanie programu pętlik */

/**
 * Wykonuje kod zoptymalizowany.
 */
void kod_zoptymalizowany(int ***zmienne, int **ile, char **polecenie, int i) {
    char pom = (*polecenie)[i];
    int indeks = pom - 'a';
    ++i;
    while ((*polecenie)[i] != ')') {
      przekaz(&(*zmienne)[indeks], &(*zmienne)[(*polecenie)[i] - 'a'],
      &(*ile)[indeks], &(*ile)[(*polecenie)[i] - 'a']);
      ++i;
    }
    wyzeruj(&(*zmienne)[indeks], &(*ile)[indeks]);

}

/**
 * Wykonuje instrukcję powtarzaj.
 */
int powtarzaj(int ***zmienne, int **ile, char **polecenie, int i) {
  int indeks = i;
  if (jaki_kod(polecenie, indeks)) {
    kod_zoptymalizowany(zmienne, ile, polecenie, i);
  }
  else {
    int koniec;
    while (czy_koniec(&((*zmienne)[(*polecenie)[i] - 'a']),
    (*ile)[(*polecenie)[i] - 'a'])) {
      zmniejsz(&((*zmienne)[(*polecenie)[i] - 'a']));
      koniec = koniec_w_nawiasowego(polecenie, i + 1);
      indeks = i + 1;
      while (indeks < koniec) {
        if ((*polecenie)[indeks] == '(') {
          ++indeks;
          indeks = powtarzaj(zmienne, ile, polecenie, indeks);
        }
        else {
          indeks = dodawanie(zmienne, ile, polecenie, indeks);
        }
      }
    }
  }
  i = koniec_w_nawiasowego(polecenie, i + 1) + 1;
  return i;
}

/**
 * Wypisuje zmienną po wczytaniu '='.
 */
void wypisz(int tab[], int size) {
   int i = size - 1;
   while (i >= 0 && tab[i] == 0) --i;
   while (i > 0) {
     printf("%d", tab[i]);
     --i;
   }
   printf("%d", tab[0]);
   printf("\n");
}

/**
 * Czyta polecenie.
 */
void czytaj(char **tab, int *r_tab) {
  int i = 0;
  int c = getchar();
  while (c != '\n') {
    if (i == *r_tab) {
      zwieksz(tab, r_tab);
    }
    (*tab)[i] = (char) c;
    ++i;
    c = getchar();
  }
  if (i == *r_tab) {
    zwieksz(tab, r_tab);
  }
  (*tab)[i] = (char) c;
}

/**
 * Realizuje polecenia języka pętlik.
 */
void rob(int ***zmienne, int **ile, char **polecenie) {
  int i = 0;
  while ((*polecenie)[i] != '\n') {
    if ((*polecenie)[i] == '(') {
      ++i;
      i = powtarzaj(zmienne, ile, polecenie, i);
    }
    else {
      i = dodawanie(zmienne, ile, polecenie, i);
    }
  }
}

/**
 * Wykonuje program pętlik.
 */
void wykonuj(void) {
  int r_tab = 3;
  char *polecenie = (char*) malloc((unsigned int) r_tab * sizeof(char));
  int **zmienne = stworz_tablice();
  int *ile = tab_rozmiarow();
  int c = getchar();
  while (c != EOF) {
    if (c == '=') {
      c = getchar();
      int indeks = c - 'a';
      int size = ile[indeks];
      wypisz(zmienne[indeks], size);
      c = getchar();
    }
    else {
      ungetc(c, stdin);
      czytaj(&polecenie, &r_tab);
      rob(&zmienne, &ile, &polecenie);
    }
    c = getchar();
    r_tab = 3;
    polecenie = (char*) realloc(polecenie,(unsigned int) r_tab * sizeof(char));
  }
  free(polecenie);
  free_tab(zmienne);
  free(ile);
}

int main(void) {
  wykonuj();
  return 0;
}
