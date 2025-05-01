/* Costache Ovidiu-Stefan - 313CC */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct page {
	int id;
	char url[51];
	char *description;
} Page;

typedef struct stack_node {
	Page *page;
	struct stack_node *next;
} StackNode;

typedef struct stack {
	StackNode *top;
} Stack;

typedef struct tab {
	int id;
	Page *currentPage;
	Stack *forwardStack;
	Stack *backwardStack;
} Tab;

typedef struct tab_node {
	Tab *info;
	struct tab_node *next;
	struct tab_node *prev;
} TabNode;

typedef struct tab_list {
	int size;
	TabNode *head;
} TabList;

typedef struct browser {
	int last_id;
	Tab *current;
	TabList list;
} Browser;

Page* create_page(int id, char *url, char *description) {
	Page *p = (Page*)malloc(sizeof(Page));
	if (p == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru pagina\n");
		return NULL;
	}

	p->id = id;
	strcpy(p->url, url);
	p->description = (char*)malloc(strlen(description) + 1);
	if (p->description == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru descriere\n");
		free(p);
		return NULL;
	}

	strcpy(p->description, description);
	return p;
}

Stack* create_stack(void) {
	Stack *s = (Stack*)malloc(sizeof(Stack));
	if (s == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru stiva\n");
		return NULL;
	}
	s->top = NULL;
	return s;
}

void push(Stack *s, Page *page) {
	StackNode *new = (StackNode*)malloc(sizeof(StackNode));
	if (new == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru nodul stivei\n");
		return;
	}

	new->page = page;
	new->next = s->top;
	s->top = new;
}

Page* pop(Stack *s) {
	if (s == NULL || s->top == NULL)
		return NULL;

	StackNode *temp = s->top;
	Page *page = temp->page;
	s->top = s->top->next;
	free(temp);
	return page;
}

Browser* create_browser(void) {
	Browser *b = (Browser*)malloc(sizeof(Browser));
	if (b == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru browser\n");
		return NULL;
	}

	// Initializare lista de tab-uri
	b->list.head = (TabNode*)malloc(sizeof(TabNode));
	if (b->list.head == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru nodul santinela\n");
		free(b);
		return NULL;
	}
	// Nodul santinela
	b->list.head->next = b->list.head;
	b->list.head->prev = b->list.head;
	b->list.size = 0;
	b->last_id = 0;

	// Creare tab implicit
	Tab *default_tab = (Tab*)malloc(sizeof(Tab));
	if (default_tab == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru tab\n");
		free(b->list.head);
		free(b);
		return NULL;
	}
	default_tab->id = 0;
	char url[51] = "https://acs.pub.ro/";
	char description[256] = "Computer Science\n";
	default_tab->currentPage = create_page(0, url, description);
	default_tab->forwardStack = create_stack();
	default_tab->backwardStack = create_stack();

	TabNode *new = (TabNode*)malloc(sizeof(TabNode));
	if (new == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru nodul tab-ului\n");
		free(default_tab->currentPage->description);
		free(default_tab->currentPage);
		free(default_tab->forwardStack);
		free(default_tab->backwardStack);
		free(default_tab);
		free(b->list.head);
		free(b);
		return NULL;
	}
	new->info = default_tab;
	new->next = b->list.head;
	new->prev = b->list.head->prev;
	b->list.head->prev->next = new;
	b->list.head->prev = new;
	b->list.size++;

	b->current = default_tab;
	return b;
}

void free_page(Page *p) {
	if (p != NULL) {
		free(p->description);
		free(p);
	}
}

void free_stack(Stack *s) {
	while (s->top != NULL) {
		StackNode *temp = s->top;
		s->top = s->top->next;
		free_page(temp->page);
		free(temp);
	}
	free(s);
}

void free_tab(Tab *tab) {
	if (tab != NULL) {
		free_page(tab->currentPage);
		free_stack(tab->forwardStack);
		free_stack(tab->backwardStack);
		free(tab);
	}
}

void free_browser(Browser *b) {
	if (b != NULL) {
		TabNode *current = b->list.head->next;
		while (current != b->list.head) {
			TabNode *temp = current;
			current = current->next;
			free_tab(temp->info);
			free(temp);
		}
		free(b->list.head);
		free(b);
	}
}

void NEW_TAB(Browser *browser) {
	browser->last_id++;
	int new_id = browser->last_id;

	Tab *new_tab = (Tab*)malloc(sizeof(Tab));
	if (new_tab == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru tab\n");
		return;
	}

	new_tab->id = new_id;
	char url[51] = "https://acs.pub.ro/";
	char description[256] = "Computer Science\n";
	new_tab->currentPage = create_page(0, url, description);
	new_tab->forwardStack = create_stack();
	new_tab->backwardStack = create_stack();

	TabNode *new_node = (TabNode*)malloc(sizeof(TabNode));
	if (new_node == NULL) {
		fprintf(stderr, "Nu s-a alocat memorie pentru nodul tab-ului\n");
		free_tab(new_tab);
		return;
	}
	// Adaug tab-ul in lista
	new_node->info = new_tab;
	new_node->next = browser->list.head;
	new_node->prev = browser->list.head->prev;
	browser->list.head->prev->next = new_node;
	browser->list.head->prev = new_node;

	browser->list.size++;
	browser->current = new_tab;
}

void PRINT(Browser *b, FILE *fout) {
	if (!b || !b->current || !b->list.head || b->list.size == 0) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	TabNode *current_node = b->list.head->next;
	// Caut tab-ul curent in lista
	while (current_node != b->list.head && current_node->info != b->current)
		current_node = current_node->next;

	// Afisez tab-urile in ordinea in care au fost deschise
	int cnt = 0;
	while (cnt < b->list.size) {
        fprintf(fout, "%d ", current_node->info->id);
        current_node = current_node->next;

        // Daca urmatorul nod este santinela, sar peste el
        if (current_node == b->list.head)
            current_node = b->list.head->next;
        cnt++;
    }

	fprintf(fout, "\n%s", b->current->currentPage->description);
}

void CLOSE(Browser *b, FILE *fout) {
	if (!b || !b->current || !b->list.head || b->current->id == 0) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	// Caut tab-ul ce trebuie sters
	TabNode *close = b->list.head->next;
	while (close != b->list.head && close->info != b->current)
		close = close->next;

	if (close == b->list.head)
		return;

	// Mut tab-ul curent in stanga celui sters
	if (close->prev != b->list.head)
		b->current = close->prev->info;
	else
		b->current = close->next->info;

	close->prev->next = close->next;
	close->next->prev = close->prev;
	b->list.size--;

	free_tab(close->info);
	free(close);
}

void OPEN(Browser *b, int id, FILE *fout) {
	if (b == NULL || b->list.head == NULL) {
		fprintf(fout, "403 Forbidden\n");
	    return;
	}

	// Caut tab-ul cu id-ul dat
	TabNode *current = b->list.head->next;
	while (current != b->list.head) {
		if (current->info->id == id) {
			b->current = current->info;
			return;
		}
		current = current->next;
	}

	fprintf(fout, "403 Forbidden\n");
}

void NEXT(Browser *b) {
	if (b == NULL || b->current == NULL || b->list.size == 0)
		return;

	// Caut tab-ul curent in lista
	TabNode *current = b->list.head->next;
	while (current != b->list.head && current->info != b->current)
		current = current->next;

	TabNode *next = current->next;
	// Cazul in care urmatorul tab este santinela
	if (next == b->list.head)
		next = next->next;

	b->current = next->info;
}

void PREV(Browser *b) {
	if (b == NULL || b->current == NULL || b->list.size == 0)
		return;

	// Caut tab-ul curent in lista
	TabNode *current = b->list.head->next;
	while (current != b->list.head && current->info != b->current)
		current = current->next;

	TabNode *prev = current->prev;
	// Cazul in care tab-ul anterior este santinela
	if (prev == b->list.head)
		prev = prev->prev;

	b->current = prev->info;
}

void PAGE(Browser *b, Page **pages, int pages_cnt, int id, FILE *fout) {
	if (b == NULL || b->current == NULL)
		return;

	int i;
	Page *found = NULL;
	for (i = 0; i < pages_cnt; i++) {
		if (pages[i]->id == id) {
			found = pages[i];
			break;
		}
	}

	// Daca nu am gasit pagina
	if (found == NULL) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	// Pagina curenta este pusa in stiva backward
	push(b->current->backwardStack, b->current->currentPage);

	// Eliberez memoria pentru fiecare pagina scoasa din stiva forward
	Page *temp;
    while ((temp = pop(b->current->forwardStack)) != NULL) {
        free_page(temp);
	}

	// Creez o copie, fara sa eliberez pagina originala care ar fi putut fi
	// Deja eliberata in stiva anterior (pentru a evita eroarea "double free")
	Page *copy = create_page(found->id, found->url, found->description);
	// Copia devine pagina curenta
	b->current->currentPage = copy;
}

void BACKWARD(Browser *b, FILE *fout) {
	if (b == NULL || b->current == NULL || b->current->backwardStack == NULL)
		return;

	if (b->current->backwardStack->top == NULL) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	Page *backPage = pop(b->current->backwardStack);
	push(b->current->forwardStack, b->current->currentPage);
	b->current->currentPage = backPage;
}

void FORWARD(Browser *b, FILE *fout) {
	if (b == NULL || b->current == NULL || b->current->forwardStack == NULL)
		return;

	if (b->current->forwardStack->top == NULL) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	Page *forwardPage = pop(b->current->forwardStack);
	push(b->current->backwardStack, b->current->currentPage);
	b->current->currentPage = forwardPage;
}

void PRINT_HISTORY(Browser *b, int id, FILE *fout) {
	if (b == NULL || b->list.head == NULL) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	TabNode *current = b->list.head->next;
	while (current != b->list.head) {
		if (current->info->id == id)
			break;
		current = current->next;
	}

	if (current == b->list.head) {
		fprintf(fout, "403 Forbidden\n");
		return;
	}

	Tab *tab = current->info;
	StackNode *node = tab->forwardStack->top;
	Page *forward_pages[100];
	int i, cnt = 0;
	// Afisez paginile din stiva forward
	while (node != NULL) {
		forward_pages[cnt++] = node->page;
		node = node->next;
	}
	for (i = cnt - 1; i >= 0; i--)
		fprintf(fout, "%s\n", forward_pages[i]->url);

	// Afisez pagina curenta
	fprintf(fout, "%s\n", tab->currentPage->url);

	// Afisez paginile din stiva backward
	node = tab->backwardStack->top;
	while (node != NULL) {
		fprintf(fout, "%s\n", node->page->url);
		node = node->next;
	}
}

void input(Browser *browser, FILE *fin, FILE *fout) {
	if (browser == NULL || fin == NULL || fout == NULL)
		return;

	int i, id, page_nr;
	char url[51], description[256];
	fscanf(fin, "%d", &page_nr);

	Page *pages[51];
	for (i = 0; i < page_nr; i++) {
		fscanf(fin, "%d", &id);
		fscanf(fin, "%s", url);
		// Elimin newline-ul ramas in buffer pentru a citi corect descrierea
		fgetc(fin);
		fgets(description, 256, fin);
		pages[i] = create_page(id, url, description);
	}

	int command_nr;
	fscanf(fin, "%d", &command_nr);

	char command[20];
	for (i = 0; i < command_nr; i++) {
		fscanf(fin, "%s", command);

		if (strcmp(command, "NEW_TAB") == 0) {
			NEW_TAB(browser);
		} else if (strcmp(command, "PRINT") == 0) {
			PRINT(browser, fout);
		} else if (strcmp(command, "CLOSE") == 0) {
			CLOSE(browser, fout);
		} else if (strcmp(command, "OPEN") == 0) {
			int tab_id;
			if (fscanf(fin, "%d", &tab_id) != 1)
				fprintf(fout, "403 Forbidden\n");
			else
				OPEN(browser, tab_id, fout);
		} else if (strcmp(command, "NEXT") == 0) {
			NEXT(browser);
		} else if (strcmp(command, "PREV") == 0) {
			PREV(browser);
		} else if (strcmp(command, "PAGE") == 0) {
			int page_id;
			if (fscanf(fin, "%d", &page_id) != 1)
				fprintf(fout, "403 Forbidden\n");
			else
				PAGE(browser, pages, page_nr, page_id, fout);
		} else if (strcmp(command, "PRINT_HISTORY") == 0) {
			int tab_id;
			if (fscanf(fin, "%d", &tab_id) != 1)
				fprintf(fout, "403 Forbidden\n");
			else
				PRINT_HISTORY(browser, tab_id, fout);
		} else if (strcmp(command, "BACKWARD") == 0) {
			BACKWARD(browser, fout);
		} else if (strcmp(command, "FORWARD") == 0) {
			FORWARD(browser, fout);
		}
	}

	for (i = 0; i < page_nr; i++)
		free_page(pages[i]);
}

int main() {
	FILE *fin = fopen("tema1.in", "r");
	FILE *fout = fopen("tema1.out", "w");

	Browser *browser = create_browser();
	if (browser == NULL) {
		fclose(fin);
		fclose(fout);
		return 0;
	}

	input(browser, fin, fout);

	fclose(fin);
	fclose(fout);
	free_browser(browser);
	return 0;
}
