/* Costache Ovidiu-Stefan - 313CC */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct nod {
    int frecv;
    char *nume;
    struct nod *st;
    struct nod *dr;
    struct nod *parinte;
} Nod;

typedef struct heap {
    Nod **v;
    int nr; // Nr de elem in heap
    int cap; // Capacitatea vectorului
} Heap;

Nod* creareNod(int f, char *n) {
    Nod *nod = (Nod*)malloc(sizeof(Nod));
    if (nod == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru nod\n");
        return 0;
    }

    nod->frecv = f;
    nod->nume = (char*)malloc(strlen(n) + 1);
    if (nod->nume == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru nume\n");
        return 0;
    }

    strcpy(nod->nume, n);
    nod->st = NULL;
    nod->dr = NULL;
    nod->parinte = NULL;
    return nod;
}

Heap* creareHeap(int cap) {
    Heap *heap = (Heap*)malloc(sizeof(Heap));
    if (heap == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru heap\n");
        return 0;
    }

    heap->v = (Nod**)malloc(sizeof(Nod*) * cap);
    if (heap->v == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru vectorul heap");
        return 0;
    }

    heap->nr = 0;
    heap->cap = cap;
    return heap;
}

char* concatenare(char *n1, char *n2) {
    char *rez = (char*)malloc(strlen(n1) + strlen(n2) + 1);
    if (rez == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru concatenare\n");
        return 0;
    }

    strcpy(rez, n1);
    strcat(rez, n2);
    return rez;
}

// Functia compara doua noduri conform cerintei, dupa frecventa si nume
int cmp(Nod *a, Nod *b) {
    if (a->frecv < b->frecv)
        return -1;
    else
        if (a->frecv > b->frecv)
            return 1;
        else
            return strcmp(a->nume, b->nume);
}

// Pentru functiile de prelucrare a heap-ului m-am ajutat de ce am facut
// in laboratorul 7 si de documentatia de pe Geeks for Geeks

// Functia duce elementul de la indicele "index" in sus pana cand se respecta
// proprietatea heap-ului
void shiftUp(Heap *heap, int index) {
    while (index > 0) {
        int parinte = (index - 1) / 2;

        if (cmp(heap->v[index], heap->v[parinte]) < 0) {
            // Fac swap
            Nod *temp = heap->v[index];
            heap->v[index] = heap->v[parinte];
            heap->v[parinte] = temp;
            index = parinte;
        }
        else
            break;
    }
}

void minHeapify(Heap *heap, int i) {
    int st = 2 * i + 1;
    int dr = 2 * i + 2;
    int min = i;

    if (st < heap->nr && cmp(heap->v[st], heap->v[min]) < 0)
        min = st;
    if (dr < heap->nr && cmp(heap->v[dr], heap->v[min]) < 0)
        min = dr;
    if (min != i) {
        // Fac swap
        Nod *temp = heap->v[i];
        heap->v[i] = heap->v[min];
        heap->v[min] = temp;
        minHeapify(heap, min);
    }
}

void insertHeap(Heap *heap, Nod *nod) {
    if (heap->nr == heap->cap) {
        // Fac spatiu dubland capacitatea
        heap->cap = heap->cap * 2;

        heap->v = (Nod**)realloc(heap->v, sizeof(Nod*) * heap->cap);
        if (heap->v == NULL) {
            fprintf(stderr, "Nu s-a realocat memorie pentru heap\n");
            return;
        }
    }
    heap->v[heap->nr] = nod;
    shiftUp(heap, heap->nr);
    heap->nr++;
}

// Functia extrage nodul cu val min, adica radacina si ajusteaza heap-ul
// folosind functia minHeapify
Nod* extrageHeap(Heap *heap) {
    Nod *min = heap->v[0];
    heap->v[0] = heap->v[heap->nr - 1];
    heap->nr--;
    minHeapify(heap, 0);
    return min;
}

void freeHeap(Heap *heap) {
    free(heap->v);
    free(heap);
}

// Inserez fiecare satelit in heap, apoi combin primele doua noduri cu
// frecventa minima si creez nodul parinte, pana cand mai ramane un singur nod
Nod* creareArbore(Nod **noduri, int n) {
    int i, frecventa;
    Heap *heap = creareHeap(n);

    for (i = 0; i < n; i++)
        insertHeap(heap, noduri[i]);
    while (heap->nr > 1) {
        Nod *st = extrageHeap(heap);
        Nod *dr = extrageHeap(heap);

        frecventa = st->frecv + dr->frecv;
        char *nume = concatenare(st->nume, dr->nume);
        Nod *parinte = creareNod(frecventa, nume);
        free(nume); // Eliberez memoria pt concatenare

        // Actualizez legaturile
        parinte->st = st;
        parinte->dr = dr;
        st->parinte = parinte;
        dr->parinte = parinte;
        insertHeap(heap, parinte);
    }

    Nod *radacina = extrageHeap(heap);
    freeHeap(heap);
    return radacina;
}

// Am afisat parcurgand pe niveluri si folosindu-ma de o coada
void afisareBFS(Nod *radacina, int nrSateliti, FILE *fout) {
    if (radacina == NULL)
        return;

    Nod **coada = (Nod**)malloc(sizeof(Nod*) * (2 * nrSateliti - 1));
    if (coada == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru ccoada\n");
        return;
    }

    int p, u, cnt, i;
    p = u = 0;
    coada[u++] = radacina;
    while (p < u) {
        cnt = u - p;
        for (i = 0; i < cnt; i++) {
            Nod *nod = coada[p++];
            fprintf(fout, "%d-%s ", nod->frecv, nod->nume);
            if (nod->st != NULL)
                coada[u++] = nod->st;
            if (nod->dr != NULL)
                coada[u++] = nod->dr;
        }
        fprintf(fout, "\n");
    }
    free(coada);
}

void freeArbore(Nod *radacina, int nrSateliti) {
    if (radacina == NULL)
        return;

    Nod **stiva = (Nod**)malloc(sizeof(Nod*) * (2 * nrSateliti - 1));
    if (stiva == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru stiva\n");
        return;
    }

    int vf = 0;
    stiva[vf++] = radacina;
    while (vf > 0) {
        Nod *nod = stiva[--vf];

        if (nod->st != NULL)
            stiva[vf++] = nod->st;
        if (nod->dr != NULL)
            stiva[vf++] = nod->dr;

        free(nod->nume);
        free(nod);
    }
    free(stiva);
}

void decodificare(Nod *radacina, char *cod, FILE *fout) {
    char *rez[1001];
    int cnt = 0, i;
    Nod *curent = radacina;

    for (i = 0; i < strlen(cod); i++) {
        if (cod[i] == '0')
            curent = curent->st;
        else
            if (cod[i] == '1')
                curent = curent->dr;

        // Daca am ajuns la o frunza
        if (curent->st == NULL && curent->dr == NULL) {
            rez[cnt++] = curent->nume; // Pointer catre nodul satelitului
            curent = radacina;
        }
    }

    for (i = 0; i < cnt; i++) {
        fprintf(fout, "%s", rez[i]);
        if (i < cnt - 1)
        fprintf(fout, " ");
    }
    fprintf(fout, "\n");
}

char* codificare(Nod *frunza) {
    char temp[1001];
    int i, poz = 0;

    while (frunza->parinte != NULL) {
        if (frunza == frunza->parinte->st)
            temp[poz++] = '0';
        else
            temp[poz++] = '1';
        frunza = frunza->parinte;
    } 
    temp[poz] = '\0'; // Adaug si terminatorul de sir

    // Trebuie inversat sirul
    char *cod = (char*)malloc(poz + 1);
    int lungime = poz;
    for (i = 0; i < lungime; i++)
        cod[i] = temp[lungime - i - 1];
    cod[lungime] = '\0';
    return cod;
}

// Calculez adancimea plecand de la nodul curent si ajungand la radacina
int adancime(Nod *nod) {
    int adancime = 0;

    while(nod != NULL) {
         adancime++;
         nod = nod->parinte;
    }
    return adancime;
}

// Calculez "Lowest Common Ancestor" ca in laboratorul 6
Nod* LCA(Nod *a, Nod *b) {
    int adancimeA = adancime(a);
    int adancimeB = adancime(b);

    while (adancimeA > adancimeB) {
         a = a->parinte;
         adancimeA--;
    }
    while (adancimeB > adancimeA) {
         b = b->parinte;
         adancimeB--;
    }
    while (a != NULL && b != NULL && a != b) {
         a = a->parinte;
         b = b->parinte;
    }
    return a;
}

void cerinta1(FILE *fin, FILE *fout) {
    int nrSateliti;
    fscanf(fin, "%d", &nrSateliti);

    Nod **noduri = (Nod**)malloc(sizeof(Nod*) * nrSateliti);
    if (noduri == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru vectorul de noduri\n");
        return;
    }

    int i, frecventa;
    char nume[101];

    for (i = 0; i < nrSateliti; i++) {
        fscanf(fin, "%d", &frecventa);
        fscanf(fin, "%s", nume);
        noduri[i] = creareNod(frecventa, nume);
    }

    Nod *radacina = creareArbore(noduri, nrSateliti);
    afisareBFS(radacina, nrSateliti, fout);

    freeArbore(radacina, nrSateliti);
    free(noduri);
}

void cerinta2(FILE *fin, FILE *fout) {
    int nrSateliti;
    fscanf(fin, "%d", &nrSateliti);

    Nod **noduri = (Nod**)malloc(sizeof(Nod*) * nrSateliti);
    if (noduri == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru vectorul de noduri\n");
        return;
    }

    int i, frecventa;
    char nume[101];

    for (i = 0; i < nrSateliti; i++) {
        fscanf(fin, "%d", &frecventa);
        fscanf(fin, "%s", nume);
        noduri[i] = creareNod(frecventa, nume);
    }

    Nod *radacina = creareArbore(noduri, nrSateliti);
    int nr;
    char cod[1001];
    fscanf(fin, "%d", &nr);

    for (i = 0; i < nr; i++) {
        fscanf(fin, "%s", cod);
        decodificare(radacina, cod, fout);
    }

    freeArbore(radacina, nrSateliti);
    free(noduri);
}

void cerinta3(FILE *fin, FILE *fout) {
    int nrSateliti;
    fscanf(fin, "%d", &nrSateliti);

    Nod **noduri = (Nod**)malloc(sizeof(Nod*) * nrSateliti);
    if (noduri == NULL) {
        fprintf(stderr, "Nu s-a alocat memorie pentru noduri\n");
        return;
    }

    int i, j, frecventa;
    char nume[101];

    for (i = 0; i < nrSateliti; i++) {
        fscanf(fin, "%d", &frecventa);
        fscanf(fin, "%s", nume);
        noduri[i] = creareNod(frecventa, nume);
    }

    Nod *radacina = creareArbore(noduri, nrSateliti);
    int nr;
    char codificareFinala[10001] = "";
    char *cod;
    fscanf(fin, "%d", &nr);

    for (i = 0; i < nr; i++) {
        fscanf(fin, "%s", nume);
        for (j = 0; j < nrSateliti; j++) {
            if (strcmp(noduri[j]->nume, nume) == 0) {
                cod = codificare(noduri[j]);
                strcat(codificareFinala, cod);
                free(cod);
                break;
            }
        }
    }
    fprintf(fout, "%s\n", codificareFinala);

    freeArbore(radacina, nrSateliti);
    free(noduri);
}

void cerinta4(FILE *fin, FILE *fout) {
    int nrSateliti;
    fscanf(fin, "%d", &nrSateliti);

    Nod **noduri = (Nod**)malloc(sizeof(Nod*) * nrSateliti);
    if (noduri == NULL) {
       fprintf(stderr, "Nu s-a alocat memorie pentru vectorul de noduri\n");
       return;
    }

    int i, j, frecventa;
    char nume[101];

    for (i = 0; i < nrSateliti; i++) {
       fscanf(fin, "%d", &frecventa);
       fscanf(fin, "%s", nume);
       noduri[i] = creareNod(frecventa, nume);
    }

    Nod *radacina = creareArbore(noduri, nrSateliti);
    int nr;
    fscanf(fin, "%d", &nr);

    Nod **noduriErori = (Nod**)malloc(sizeof(Nod*) * nr);
    if (noduriErori == NULL) {
       fprintf(stderr, "Nu s-a alocat memorie pentru nodurile cu probleme\n");
       return;
    }

    // Caut nodurile cu probleme in vectorul original
    for (i = 0; i < nr; i++) {
        fscanf(fin, "%s", nume);

        noduriErori[i] = NULL;
        for (j = 0; j < nrSateliti; j++) {
            if (strcmp(noduri[j]->nume, nume) == 0) {
                noduriErori[i] = noduri[j];
                break;
            }
        }
    }

    Nod *comun = noduriErori[0];
    for (i = 1; i < nr; i++)
        comun = LCA(comun, noduriErori[i]);

    fprintf(fout, "%s", comun->nume);

    free(noduriErori);
    freeArbore(radacina, nrSateliti);
    free(noduri);
}

int main(int argc, char **argv) {
    FILE *fin = fopen(argv[2], "rt");
    FILE *fout = fopen(argv[3], "wt");
  
    if (strcmp(argv[1], "-c1") == 0)
       cerinta1(fin, fout);
    else
        if (strcmp(argv[1], "-c2") == 0)
            cerinta2(fin, fout);
        else
            if (strcmp(argv[1], "-c3") == 0)
                cerinta3(fin, fout);
            else
                if (strcmp(argv[1], "-c4") == 0)
                    cerinta4(fin, fout);
                else {
                    fclose(fin);
                    fclose(fout);
                    return 0;
                }

    fclose(fin);
    fclose(fout);
    return 0;
}
