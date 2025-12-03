typedef struct Node {
    int info;
    struct Node *next;
} Node;

typedef struct {
    Node *primeiro;
    Node *ultimo;
} fila;

void Cria(fila *f) {
    f->primeiro = NULL;
    f->ultimo = NULL;
}

int Vazia(fila *f) {
    if (f->primeiro == NULL && f->ultimo == NULL) {
        return 1;
    } else {
        return 0;
    }
}

int Cheia(fila *f) {
    return 0;
}

void Insere(fila *f, int x, int *deucerto) {
    *deucerto = 1;

    Node *aux = (Node *)malloc(sizeof(Node));
    if (aux == NULL) {
        *deucerto = 0;
        return;
    }

    aux->info = x;
    aux->next = NULL;

    if (Vazia(f)) {
        f->primeiro = aux;
        f->ultimo = aux;
    } else {
        f->ultimo->next = aux;
        f->ultimo = aux;
    }
}

void Retira(fila *f, int *x, int *deucerto) {
    if (Vazia(f)) {
        *deucerto = 0;
    } else {
        *deucerto = 1;
        *x = f->primeiro->info;

        Node *aux = f->primeiro;
        if (f->primeiro == f->ultimo) {
            f->primeiro = NULL;
            f->ultimo = NULL;
        } else {
            f->primeiro = f->primeiro->next;
        }
        free(aux);
    }
}

int Nelementos(fila *f) {
    int count = 0;
    Node *aux = f->primeiro;

    while (aux != NULL) {
        count++;
        aux = aux->next;
    }

    return count;
}

void Transfere(fila *origem, fila *destino) {
    int x, deu;

    while (!Vazia(origem)) {
        Retira(origem, &x, &deu);
        if (deu) {
            Insere(destino, x, &deu);
        }
    }
}
