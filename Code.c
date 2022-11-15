//Stefano Maxenti - 865725 - 10526141

/////////////////////////////////////////////////////////////////////////////////////////////////////
//                                  PROVA FINALE DI API - 2019                                     //
// meccanismo di monitoraggio di relazioni tra entità (per esempio persone) che cambiano nel tempo //
/////////////////////////////////////////////////////////////////////////////////////////////////////

#define PRINTING_COMMAND 0 //stampa informazioni utili al debug
#define TEST_SIZE_STRUCT 0 //stampa le dimensioni delle struct - debugging
#define FAST_REPORT 1 //crea una struttura apposita per il report, contenente solo gli elementi che devono essere stampati
#define OLD_REPORT 0 //permette il report senza la struttura apposita
#define FAST_DELENT 1 //permette una cancellazione notevolmente più veloce delle entità grazie all'utilizzo del campo out di tree per ogni entità
#define OLD_DELENT 0 //la cancellazione è notevolmente lenta in quanto manca di una struttura dati apposita per velocizzare - legacy

//Permettono di attivare e disattivare le funzioni del programma.
#define ADDENT 1
#define DELENT 1
#define ADDREL 1
#define DELREL 1
#define REPORT 1

//Due buffer
#define BUFFER_SIZE 400
#define SIZE_KEY 50

/********************************************\
 LIBRERIE STANDARD
 \********************************************/
#include <stdio.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/********************************************\
 STRUCT DEFINITE
 *********************************************/

typedef enum { false, true } boolean;
typedef enum { NIGRUM, RUBER } color; //black and red

struct tree_ptr;
typedef struct tree_ptr tree_ptr;

//Struttura usata per gli alberi secondari di entità e relazioni
typedef struct tree_node_ptr {
    char *key_ptr; //puntatori per permettere la copia delle chiavi
    struct tree_node_ptr *pater, *dexter, *sinister;
    struct tree_ptr *data;
    int number; //numero di volte che key_ptr fa qualcosa...
    color color;
} tree_node_ptr;

struct tree_ptr {
    tree_node_ptr *root;
};

//Struttura usata per gli alberi principali di entità e relazioni
typedef struct tree_node {
    char *key; //puntatori per permettere la copia delle chiavi
    struct tree_node *pater, *dexter, *sinister;
    tree_ptr *out, *in;
    //se key rappresenta un'entità nell'albero delle entità, in indicherà le relazioni entranti, out le relazioni uscenti.
    //se key rappresenta una relazione nell'albero delle relazioni, in indicherà tutte le entità riceventi quella relazione,
    //out solo quelle che saranno riportate nella report
    int number; //inutilizzato nell'albero delle entità, fondamentale negli altri alberi
    color color;
} tree_node;

typedef struct {
    tree_node *root;
} tree;

/********************************************\
 VARIABILI GLOBALI
 *********************************************/
tree_node *nil;
tree_node_ptr *nil_ptr;

/********************************************\
 FUNZIONI IMPLEMENTATE
 *********************************************/

////////// PROTOTIPI //////////
void left_rotate(tree *t, tree_node *x);
void right_rotate(tree *t, tree_node *x);
void rb_insert_fixup(tree *t, tree_node *z);
void rb_insert(tree *t, tree_node *z);
#if OLD_REPORT
void walking(tree_node *t);
#endif
tree_node *tree_minimum(tree_node *x, tree *t);
tree_node *tree_maximum(tree_node *x, tree *t);
tree_node *tree_successor(tree_node *x, tree *t);
void rb_delete_fixup(tree *t, tree_node *x);
tree_node *rb_delete(tree *t, tree_node *z, boolean entity);
tree_node *tree_search(tree_node *x, tree *t, char k[]);

void left_rotate_ptr(tree_ptr *t,  tree_node_ptr *x);
void right_rotate_ptr(tree_ptr *t,  tree_node_ptr *x);
void rb_insert_fixup_ptr(tree_ptr *t,  tree_node_ptr *z);
void rb_insert_ptr(tree_ptr *t,  tree_node_ptr *z);
#if OLD_REPORT
void walking_report(tree_node_ptr *t, tree_ptr *nil, int maximum);
#endif
tree_node_ptr *tree_minimum_ptr(tree_node_ptr *x, tree_ptr *t);
tree_node_ptr *tree_maximum_ptr(tree_node_ptr *x, tree_ptr *t);
tree_node_ptr *tree_successor_ptr(tree_node_ptr *x, tree_ptr *t);
#if FAST_REPORT
void walking_fast(tree **t);
#endif
void rb_delete_fixup_ptr(tree_ptr *t, tree_node_ptr *x);
tree_node_ptr *rb_delete_ptr(tree_ptr *t, tree_node_ptr *z);
tree_node_ptr *tree_search_ptr(tree_node_ptr *x, tree_ptr *t, char k[]);

tree_node *addent(tree *entities_tree, char test[], int size_);
tree_node_ptr *addent_ptr(tree_ptr *entities_tree, char test[]);
void addrel(tree *entities_tree, tree *relationships, tree_node *entity_out, tree_node *entity_in, char relationship_str[], int size_);
boolean is_there_a_relationship(tree *entities_tree, tree_node *entity_out, tree_node *entity_in, char relationship_str[]);
#if FAST_REPORT
void reporting_maxim(tree_node *this_relationship, tree_node_ptr *entity_ptr);
#endif
void delrel(tree *entities_tree, tree *relationships, char id_origin[], char id_dest[], char relationship_str[], tree_node *entity_out, tree_node *entity_in);
void delent(tree *entities_tree, tree *relationships, tree_node *user_to_be_deleted_in_ent_tree);


//GESTIONE "TREE" - codice preso da pseudocodice delle slide
void left_rotate(tree *t, tree_node *x) {
    tree_node *y;
    y = x->dexter;
    x->dexter = y->sinister;
    if (y->sinister != nil)
        y->sinister->pater = x;
    y->pater = x->pater;
    if (x->pater == nil) {
        t->root = y;
    }
    else if (x == x->pater->sinister)
        x->pater->sinister = y;
    else x->pater->dexter = y;
    y->sinister = x;
    x->pater = y;
}

void right_rotate(tree *t, tree_node *x) {
    tree_node *y;
    y = x->sinister;
    x->sinister = y->dexter;
    if (y->dexter != nil)
        y->dexter->pater = x;
    y->pater = x->pater;
    if (x->pater == nil) {
        t->root = y;
    }
    else if (x == x->pater->dexter)
        x->pater->dexter = y;
    else x->pater->sinister = y;
    y->dexter = x;
    x->pater = y;
}

void rb_insert_fixup(tree *t, tree_node *z) {
    tree_node *x;
    if (z == t->root)
        t->root->color = NIGRUM;
    else {
        x = z->pater;
        if (x->color == RUBER) {
            if (x == x->pater->sinister) {
                tree_node *y;
                y = x->pater->dexter;
                if (y->color == RUBER) {
                    x->color = NIGRUM;
                    y->color = NIGRUM;
                    x->pater->color = RUBER;
                    rb_insert_fixup(t, x->pater);
                }
                else {
                    if (z == x->dexter) {
                        z = x;
                        left_rotate(t, z);
                        x = z->pater;
                    }
                    x->color = NIGRUM;
                    x->pater->color = RUBER;
                    right_rotate(t, x->pater);
                }
            } else {
                tree_node *y;
                y = x->pater->sinister;
                if (y->color == RUBER) {
                    x->color = NIGRUM;
                    y->color = NIGRUM;
                    x->pater->color = RUBER;
                    rb_insert_fixup(t, x->pater);
                }
                else {
                    if (z == x->sinister) {
                        z = x;
                        right_rotate(t, z);
                        x = z->pater;
                    }
                    x->color = NIGRUM;
                    x->pater->color = RUBER;
                    left_rotate(t, x->pater);
                }
            }
        }
    }
}

void rb_insert(tree *t, tree_node *z) {
    tree_node *y;
    tree_node *x;
    y = nil;
    x = t->root;
    while (x != nil) {
        y = x;
        if (strcmp(z->key, x->key) < 0)
            x = x->sinister;
        else
            x = x->dexter;
    }
    z->pater = y;
    if (y == nil)
        t->root = z;
    else {
        if (strcmp(z->key, y->key) < 0)
            y->sinister = z;
        else
            y->dexter = z;
    }
    z->sinister = nil;
    z->dexter = nil;
    z->color = RUBER;
    rb_insert_fixup(t, z);
}

#if OLD_REPORT
void walking(tree_node *t) {
    if (t != nil) {
        walking(t->sinister);
        fputs("\"", stdout);
        fputs(t->key, stdout);
        fputs("\" ", stdout);
        walking_report(t->in->root, t->in, t->number);
        printf("%d", t->number);
        fputs("; ", stdout);
        walking(t->dexter);
    }
}
#endif

tree_node *tree_minimum(tree_node *x, tree *t) {
    while (x != NULL && x->sinister != NULL && x->sinister != nil)
        x = x->sinister;
    return x;
}

tree_node *tree_maximum(tree_node *x, tree *t) {
    while (x->dexter != NULL && x->dexter != nil)
        x = x->dexter;
    return x;
}

tree_node *tree_successor(tree_node *x, tree *t) {
    if (x != NULL && x->dexter != nil)
        return tree_minimum(x->dexter, t);
    tree_node *y;
    y = x->pater;
    while (y != nil && x == y->dexter) {
        x = y;
        y = y->pater;
    }
    return y;
}

void rb_delete_fixup(tree *t, tree_node *x) {
    tree_node *w;
    if (x->color == RUBER || x->pater == nil)
        x->color = NIGRUM;
    else if (x == x->pater->sinister) {
        w = x->pater->dexter;
        if (w->color == RUBER) {
            w->color = NIGRUM;
            x->pater->color = RUBER;
            left_rotate(t, x->pater);
            w = x->pater->dexter;
        }
        if (w->sinister->color == NIGRUM && w->dexter->color == NIGRUM) {
            w->color = RUBER;
            rb_delete_fixup(t, x->pater);
        }
        else {
            if(w->dexter->color == NIGRUM) {
                w->sinister->color = NIGRUM;
                w->color = RUBER;
                right_rotate(t, w);
                w = x->pater->dexter;
            }
            w->color = x->pater->color;
            x->pater->color = NIGRUM;
            w->dexter->color = NIGRUM;
            left_rotate(t, x->pater);
        }
    }
    else {
        tree_node *w;
        w = x->pater->sinister;
        if (w->color == RUBER) {
            w->color = NIGRUM;
            x->pater->color = RUBER;
            right_rotate(t, x->pater);
            w = x->pater->sinister;
        }
        if (w->dexter->color == NIGRUM && w->sinister->color == NIGRUM) {
            w->color = RUBER;
            rb_delete_fixup(t, x->pater);
        }
        else {
            if(w->sinister->color == NIGRUM) {
                w->dexter->color = NIGRUM;
                w->color = RUBER;
                left_rotate(t, w);
                w = x->pater->sinister;
            }
            w->color = x->pater->color;
            x->pater->color = NIGRUM;
            w->sinister->color = NIGRUM;
            right_rotate(t, x->pater);
        }
    }
}

tree_node *rb_delete(tree *t, tree_node *z, boolean entity) {
    if (z != nil) {
        tree_node *y;
        tree_node *x;
        if (z->sinister == nil || z->dexter == nil)
            y = z;
        else y = tree_successor(z, t);
        if (y != NULL && y->sinister != nil)
            x = y->sinister;
        else x = y->dexter;
        //sostituisco y con il suo sottoalbero
        x->pater = y->pater;
        if (y->pater == nil)
            t->root = x;
        else if (y == y->pater->sinister) //se y punta a al fratello - fratello uguale a stesso
            y->pater->sinister = x;
        else
            y->pater->dexter = x; //
        if (y != z) { //qua copio i dati satellite - se y ha due sottoalberi
            if (entity == true)
                free(z->key);
            z->key = y->key;
            z->in = y->in;
            z->out = y->out;
            z->number = y->number;
        }
        if (y->color != RUBER)
            rb_delete_fixup(t, x);
        return y; //nodo liberato dal chiamante
    } else return NULL;
}

tree_node *tree_search(tree_node *x, tree *t, char k[]) {
    if (x == nil || strcmp(k, x->key) == 0)
        return x;
    if (strcmp(k, x->key) < 0)
        return tree_search(x->sinister, t, k);
    else
        return tree_search(x->dexter, t, k);
}

//////////////////////////////////////////////////////////////////////////
//GESTIONE "TREE_PTR" - codice preso da pseudocodice delle slide
void left_rotate_ptr(tree_ptr *t,  tree_node_ptr *x) {
    tree_node_ptr *y;
    y = x->dexter;
    x->dexter = y->sinister;
    if (y->sinister != nil_ptr)
        y->sinister->pater = x;
    y->pater = x->pater;
    if (x->pater == nil_ptr) {
        t->root = y;
    }
    else if (x == x->pater->sinister)
        x->pater->sinister = y;
    else x->pater->dexter = y;
    y->sinister = x;
    x->pater = y;
}

void right_rotate_ptr(tree_ptr *t,  tree_node_ptr *x) {
    tree_node_ptr *y;
    y = x->sinister;
    x->sinister = y->dexter;
    if (y->dexter != nil_ptr)
        y->dexter->pater = x;
    y->pater = x->pater;
    if (x->pater == nil_ptr) {
        t->root = y;
    }
    else if (x == x->pater->dexter)
        x->pater->dexter = y;
    else x->pater->sinister = y;
    y->dexter = x;
    x->pater = y;
}

void rb_insert_fixup_ptr(tree_ptr *t,  tree_node_ptr *z) {
    tree_node_ptr *x;
    if (z == t->root)
        t->root->color = NIGRUM;
    else {
        x = z->pater;
        if (x->color == RUBER) {
            if (x == x->pater->sinister) {
                tree_node_ptr *y;
                y = x->pater->dexter;
                if (y->color == RUBER) {
                    x->color = NIGRUM;
                    y->color = NIGRUM;
                    x->pater->color = RUBER;
                    rb_insert_fixup_ptr(t, x->pater);
                }
                else {
                    if (z == x->dexter) {
                        z = x;
                        left_rotate_ptr(t, z);
                        x = z->pater;
                    }
                    x->color = NIGRUM;
                    x->pater->color = RUBER;
                    right_rotate_ptr(t, x->pater);
                }
            } else {
                tree_node_ptr *y;
                y = x->pater->sinister;
                if (y->color == RUBER) {
                    x->color = NIGRUM;
                    y->color = NIGRUM;
                    x->pater->color = RUBER;
                    rb_insert_fixup_ptr(t, x->pater);
                }
                else {
                    if (z == x->sinister) {
                        z = x;
                        right_rotate_ptr(t, z);
                        x = z->pater;
                    }
                    x->color = NIGRUM;
                    x->pater->color = RUBER;
                    left_rotate_ptr(t, x->pater);
                }
            }
        }
    }
}

void rb_insert_ptr(tree_ptr *t,  tree_node_ptr *z) {
    tree_node_ptr *y;
    tree_node_ptr *x;
    y = nil_ptr;
    x = t->root;
    while (x != nil_ptr) {
        y = x;
        if (strcmp(z->key_ptr, x->key_ptr) < 0)
            x = x->sinister;
        else
            x = x->dexter;
    }
    z->pater = y;
    if (y == nil_ptr)
        t->root = z;
    else {
        if (strcmp(z->key_ptr, y->key_ptr) < 0)
            y->sinister = z;
        else
            y->dexter = z;
    }
    z->sinister = nil_ptr;
    z->dexter = nil_ptr;
    z->color = RUBER;
    rb_insert_fixup_ptr(t, z);
}

#if OLD_REPORT
void walking_report(tree_node_ptr *t, tree_ptr *nil, int maximum) {
    if (t != nil_ptr) {
        walking_report(t->sinister, nil, maximum);
        if (t->number == maximum) {
            fputs("\"", stdout);
            fputs(t->key_ptr, stdout);
            fputs("\" ", stdout);
        }
        walking_report(t->dexter, nil, maximum);
    }
}
#endif

tree_node_ptr *tree_minimum_ptr(tree_node_ptr *x, tree_ptr *t) {
    while (x != NULL && x->sinister != NULL && x->sinister != nil_ptr)
        x = x->sinister;
    return x;
}

tree_node_ptr *tree_maximum_ptr(tree_node_ptr *x, tree_ptr *t) {
    while (x->dexter != NULL && x->dexter != nil_ptr)
        x = x->dexter;
    return x;
}

tree_node_ptr *tree_successor_ptr(tree_node_ptr *x, tree_ptr *t) {
    if (x != NULL && x->dexter != nil_ptr)
        return tree_minimum_ptr(x->dexter, t);
    tree_node_ptr *y;
    y = x->pater;
    while (y != nil_ptr && x == y->dexter) {
        x = y;
        y = y->pater;
    }
    return y;
}

#if FAST_REPORT
void walking_fast(tree **t) {
    tree_node *min = tree_minimum((*t)->root, *t);
    while (min != nil) {
        tree_node *temp = tree_successor(min, *t);
        fputs("\"", stdout);
        fputs(min->key, stdout);
        fputs("\" ", stdout);
        tree_node_ptr *min2 = tree_minimum_ptr(min->out->root, min->out);
        while (min2 != nil_ptr) {
            tree_node_ptr *temp2 = tree_successor_ptr(min2, min->out);
            fputs("\"", stdout);
            fputs(min2->key_ptr, stdout);
            fputs("\" ", stdout);
            min2 = temp2;
        }
        printf("%d", min->number);
        fputs("; ", stdout);
        min = temp;
    }
}
#endif

void rb_delete_fixup_ptr(tree_ptr *t, tree_node_ptr *x) {
    tree_node_ptr *w;
    if (x->color == RUBER || x->pater == nil_ptr)
        x->color = NIGRUM;
    else if (x == x->pater->sinister) {
        w = x->pater->dexter;
        if (w->color == RUBER) {
            w->color = NIGRUM;
            x->pater->color = RUBER;
            left_rotate_ptr(t, x->pater);
            w = x->pater->dexter;
        }
        if (w->sinister->color == NIGRUM && w->dexter->color == NIGRUM) {
            w->color = RUBER;
            rb_delete_fixup_ptr(t, x->pater);
        }
        else {
            if(w->dexter->color == NIGRUM) {
                w->sinister->color = NIGRUM;
                w->color = RUBER;
                right_rotate_ptr(t, w);
                w = x->pater->dexter;
            }
            w->color = x->pater->color;
            x->pater->color = NIGRUM;
            w->dexter->color = NIGRUM;
            left_rotate_ptr(t, x->pater);
        }
    }
    else {
        tree_node_ptr *w;
        w = x->pater->sinister;
        if (w->color == RUBER) {
            w->color = NIGRUM;
            x->pater->color = RUBER;
            right_rotate_ptr(t, x->pater);
            w = x->pater->sinister;
        }
        if (w->dexter->color == NIGRUM && w->sinister->color == NIGRUM) {
            w->color = RUBER;
            rb_delete_fixup_ptr(t, x->pater);
        }
        else {
            if(w->sinister->color == NIGRUM) {
                w->dexter->color = NIGRUM;
                w->color = RUBER;
                left_rotate_ptr(t, w);
                w = x->pater->sinister;
            }
            w->color = x->pater->color;
            x->pater->color = NIGRUM;
            w->sinister->color = NIGRUM;
            right_rotate_ptr(t, x->pater);
        }
    }
}

tree_node_ptr *rb_delete_ptr(tree_ptr *t, tree_node_ptr *z) {
    if (z != nil_ptr) {
        tree_node_ptr *y;
        tree_node_ptr *x;
        if (z->sinister == nil_ptr || z->dexter == nil_ptr)
            y = z;
        else y = tree_successor_ptr(z, t);
        if (y != NULL && y->sinister != nil_ptr)
            x = y->sinister;
        else x = y->dexter;
        x->pater = y->pater;
        if (y->pater == nil_ptr)
            t->root = x;
        else if (y == y->pater->sinister)
            y->pater->sinister = x;
        else
            y->pater->dexter = x;
        if (y != z) {
            z->key_ptr = y->key_ptr;
            z->data = y->data;
            z->number = y->number;
        }
        if (y->color != RUBER)
            rb_delete_fixup_ptr(t, x);
        return y;
    } else return NULL;
}

tree_node_ptr *tree_search_ptr(tree_node_ptr *x, tree_ptr *t, char k[]) {
    if (x != NULL && (x == nil_ptr || strcmp(k, x->key_ptr) == 0)) {
        return x;
    }
    if (strcmp(k, x->key_ptr) < 0)
        return tree_search_ptr(x->sinister, t, k);
    else
        return tree_search_ptr(x->dexter, t, k);
}


//Questa funzione aggiunge a un albero di tipo tree una chiave key[] che ha lunghezza size_
tree_node *addent(tree *entities_tree, char key[], int size_) {
    tree_node *is_there_already = tree_search(entities_tree->root, entities_tree, key); //evitare duplicati
    if (is_there_already == nil) {
        tree_node *to_add = calloc(1, sizeof(tree_node));
        to_add->key = malloc(sizeof(char)*size_);
        memset(to_add->key, '\0', size_);
        memcpy(to_add->key, key, size_);
        to_add->in = NULL;
        to_add->out = NULL;
        to_add->pater = nil;
        to_add->sinister = nil;
        to_add->dexter = nil;
        to_add->color = RUBER;
        rb_insert(entities_tree, to_add);
        return to_add;
    }
    return is_there_already; //restituisce il nodo se già presente
}

//Questa funzione aggiunge a un albero di tipo tree_ptr una chiave key[] che ha lunghezza size_
tree_node_ptr *addent_ptr(tree_ptr *entities_tree, char key[]) {
    tree_node_ptr *is_there_already = tree_search_ptr(entities_tree->root, entities_tree, key); //evitare duplicati
    if (is_there_already == nil_ptr) {
        tree_node_ptr *to_add = calloc(1, sizeof(tree_node_ptr));
        to_add->data = NULL;
        to_add->key_ptr = key;
        to_add->pater = nil_ptr;
        to_add->sinister = nil_ptr;
        to_add->dexter = nil_ptr;
        to_add->color = RUBER;
        rb_insert_ptr(entities_tree, to_add);
        return to_add;
    }
    return is_there_already; //restituisce il nodo se già presente
}

//Questa funzione aggiunge una relazione fra due entità nell'albero delle entità e aggiorna correttamente gli alberi delle relazioni
void addrel(tree *entities_tree, tree *relationships, tree_node *entity_out, tree_node *entity_in, char relationship_str[], int size_) {
    //-------------------------------->
    //devo verificare che non esista già la relazione fra le due entità
    //                                <---------------------------------
    boolean _is_there_a_rel_ = is_there_a_relationship(entities_tree, entity_out, entity_in, relationship_str);
    tree_node *this_rel_in_relationships = tree_search(relationships->root, relationships, relationship_str);
    tree_node_ptr *this_entity_in_this_rel = NULL;
    if (this_rel_in_relationships == nil) { //non esiste la relazione nell'albero delle relazioni
        this_rel_in_relationships = addent(relationships, relationship_str, size_); //aggiungere la relazione
        this_rel_in_relationships->number = 1;
        this_rel_in_relationships->in = malloc(sizeof(tree_ptr));
        this_rel_in_relationships->in->root = nil_ptr;
        //aggiungere l'entità all'albero delle entità riceventi tale relazione
        this_entity_in_this_rel = addent_ptr(this_rel_in_relationships->in, entity_in->key);
        //mi sposto nell'entità appena aggiunta
        this_entity_in_this_rel->number = 1;
        
#if FAST_REPORT
        if (this_entity_in_this_rel != NULL)
            reporting_maxim(this_rel_in_relationships, this_entity_in_this_rel);
#endif
        
    } else { //la relazione in generale esiste già
        if (_is_there_a_rel_ == false) { //la relazione fra le stesse due entità non era stata aggiunta prima
            this_entity_in_this_rel = addent_ptr(this_rel_in_relationships->in, entity_in->key); //aggiungere l'entità all'albero delle entità riceventi tale relazione
            this_entity_in_this_rel->number += 1;
            
#if FAST_REPORT
            if (this_entity_in_this_rel != NULL)
                reporting_maxim(this_rel_in_relationships, this_entity_in_this_rel);
#endif
            
#if OLD_REPORT
            //aggiornare il valore nella radice della relazione se il valore dell'entità è maggiore
            if (this_entity_in_this_rel->number > this_rel_in_relationships->number)
                this_rel_in_relationships->number = this_entity_in_this_rel->number;
#endif
        } //else: le due entità erano già collegate con la stessa relazione, quindi non devo fare nulla
    }
    
    //negli alberi delle entità
    if (_is_there_a_rel_ == false) {
        if (entity_in->in == NULL) { //sottoalbero in
            entity_in->in = malloc(sizeof(tree_ptr));
            entity_in->in->root = nil_ptr;
        }
        tree_ptr *s1 = entity_in->in;
        tree_node_ptr *s2 = tree_search_ptr(s1->root, s1, this_rel_in_relationships->key);
        if (s2 != nil_ptr) { //se la relazione già esiste
            addent_ptr(s2->data, entity_out->key); //aggiunge l'entità
        } else {
            tree_node_ptr *added = addent_ptr(s1, this_rel_in_relationships->key); //aggiunge la relazione
            if (added->data == NULL) {
                added->data = malloc(sizeof(tree_ptr));
                added->data->root = nil_ptr;
            }
            addent_ptr(added->data, entity_out->key);
        }
#if FAST_DELENT
        if (entity_out->out == NULL) { //sottoalbero out
            entity_out->out = malloc(sizeof(tree_ptr));
            entity_out->out->root = nil_ptr;
        }
        s1 = entity_out->out;
        tree_node_ptr *added = addent_ptr(s1, this_rel_in_relationships->key); //aggiunge la relazione
        added->number += 1; //incrementa il valore del contatore
#endif
    } //_is_there_a_rel_ == false
    
}


//Controlla se esiste la relazione in input fra le due entità, ricercandole nell'albero delle entità.
//PRECONDIZIONE: entity_out e entity_in sono presenti in entities_tree
boolean is_there_a_relationship(tree *entities_tree, tree_node *entity_out, tree_node *entity_in, char relationship_str[]) {
    if (entity_in->in == NULL) //non ci sono relazioni entranti
        return false;
    //cerco la relazione dentro l'entità ricevente
    tree_node_ptr *relationship_inside_entity = tree_search_ptr(entity_in->in->root, entity_in->in, relationship_str);
    if (relationship_inside_entity != nil_ptr) {
        //cerco l'entità di origine dentro la relazione appena trovata
        tree_node_ptr *entity_inside_relationship_found = tree_search_ptr(relationship_inside_entity->data->root, relationship_inside_entity->data, entity_out->key);
        if (entity_inside_relationship_found != nil_ptr)
            return true;
        else
            return false;
    }
    return false;
}


#if FAST_REPORT
//Permette di inserire nel sottoalbero out in relationships le relazioni e le entità che riceveono il maggior numero
//di tali relazioni. Questo sottoalbero viene aggiornato ogni volta che viene aggiunta una relazione.
//L'eliminazione di una relazione è gestita separatemante nel codice di delrel.
void reporting_maxim(tree_node *this_relationship, tree_node_ptr *entity_ptr) {
    if (this_relationship->number < entity_ptr->number) {
        tree_node_ptr *min = tree_minimum_ptr(this_relationship->out->root, this_relationship->out);
        while (min != nil_ptr) { //cancello tutti i valori attualmente presenti
            tree_node_ptr *temp = tree_successor_ptr(min, this_relationship->out);
            free(rb_delete_ptr(this_relationship->out, min));
            min = temp;
        }
        //aggiungo nodo
        tree_node_ptr *added = addent_ptr(this_relationship->out, entity_ptr->key_ptr);
        //aggiorno valore
        added->number = entity_ptr->number;
        this_relationship->number = entity_ptr->number;
        
    }
    else if (this_relationship->number == entity_ptr->number) {
        if (this_relationship->out == NULL) { //verifico che il sottoalbero sia stato allocato, altrimenti lo alloco
            this_relationship->out = malloc(sizeof(tree_ptr));
            this_relationship->out->root = nil_ptr;
        }
        //aggiungo l'entità al sottoalbero
        tree_node_ptr *added = addent_ptr(this_relationship->out, entity_ptr->key_ptr);
        added->number = entity_ptr->number; //aggiorno valore
        
    }
}
#endif


//Cancella la relazione passata in input fra le due entità (passate a loro volta in input).
void delrel(tree *entities_tree, tree *relationships, char id_origin[], char id_dest[], char relationship_str[], tree_node *entity_out, tree_node *entity_in) {
    //cerco la relazione all'interno dell'albero delle relazioni
    tree_node *this_relationship_in_relationship = tree_search(relationships->root, relationships, relationship_str);
    if (this_relationship_in_relationship != nil) { //se esiste
        //cerco nella relazione trovata l'entità ricevente
        tree_node_ptr *receiving_entity = tree_search_ptr(this_relationship_in_relationship->in->root, this_relationship_in_relationship->in, entity_in->key);
        if (receiving_entity != nil_ptr) {
            //Dopo averla trovata, aggiorno il numero ed elimino l'entità se il valore fosse 0. Aggiorno il sottoalbero out delle relazioni
            int old_receiving_number = receiving_entity->number; //memorizzo il valore iniziale, servirà successivamente
            if (receiving_entity->number == 1) {
                receiving_entity->number = 0;
#if FAST_REPORT
                reporting_maxim(this_relationship_in_relationship, receiving_entity); //aggiorno sottoalbero out
#endif
                free(rb_delete_ptr(this_relationship_in_relationship->in, receiving_entity));
                
            }
            else {
                if (receiving_entity->number != 0) {
                    receiving_entity->number--;
#if FAST_REPORT
                    reporting_maxim(this_relationship_in_relationship, receiving_entity); //aggiorno sottoalbero out
#endif
                }
            }
            //Ricreo il sottoalbero out delle relazioni nel caso particolare in cui il numero di entità riceventi la relazione prima e dopo sia uguale
            if (this_relationship_in_relationship->number == old_receiving_number) {
                tree_node_ptr *ent_in_min_rel = tree_minimum_ptr(this_relationship_in_relationship->in->root, this_relationship_in_relationship->in); //trovo entità minima
                this_relationship_in_relationship->number = 0;
                while (ent_in_min_rel != nil_ptr) {
                    if (this_relationship_in_relationship->number < ent_in_min_rel->number)
                        this_relationship_in_relationship->number = ent_in_min_rel->number;
                    ent_in_min_rel = tree_successor_ptr(ent_in_min_rel, this_relationship_in_relationship->in);
                }
                //Scorro e cancello
                tree_node_ptr *min = tree_minimum_ptr(this_relationship_in_relationship->out->root, this_relationship_in_relationship->out);
                while (min != nil_ptr) {
                    tree_node_ptr *temp = tree_successor_ptr(min, this_relationship_in_relationship->out);
                    free(rb_delete_ptr(this_relationship_in_relationship->out, min));
                    min = temp;
                }
                //Reinserisco
                min = tree_minimum_ptr(this_relationship_in_relationship->in->root, this_relationship_in_relationship->in);
                while (min != nil_ptr) {
                    tree_node_ptr *temp = tree_successor_ptr(min, this_relationship_in_relationship->in);
                    if (min->number >= this_relationship_in_relationship->number) {
                        tree_node_ptr *added = addent_ptr(this_relationship_in_relationship->out, min->key_ptr);
                        added->number = min->number;
                    }
                    min = temp;
                }
            }
        }
    }
    //Cancello all'interno del sottoalbero in dell'entità ricevente gli elementi superflui
    if (entity_in->in != NULL) {
        tree_node_ptr *this_relationship = tree_search_ptr(entity_in->in->root, entity_in->in, relationship_str);
        //ho trovato la relazione nell'entità ricevente
        if (this_relationship != nil_ptr) {
            //mi sposto nell'elenco delle entità
            tree_ptr *tree_entities_relationship_coming_from = this_relationship->data;
            if (tree_entities_relationship_coming_from->root != NULL ) {
                //cerco l'origine che aveva prodotto la relazione
                tree_node_ptr *this_entity = tree_search_ptr(tree_entities_relationship_coming_from->root, tree_entities_relationship_coming_from, id_origin);
                if (this_entity != nil_ptr) {
                    free(rb_delete_ptr(tree_entities_relationship_coming_from, this_entity));
                }
                if (this_relationship->data->root == nil_ptr && this_relationship->dexter == nil_ptr && this_relationship->sinister == nil_ptr && this_relationship->pater == nil_ptr) {
                    //rimuovo l'intero albero in quanto vuoto
                    free(rb_delete_ptr(entity_in->in, this_relationship));
                    entity_in->in = NULL;
                }
            }
        }
    }
    
#if FAST_DELENT
    //aggiorno le informazioni all'interno del sottoalbero out dell'entità che genera la relazione
    if (entity_out->out != NULL) {
        tree_ptr *tree_entities_receiving = entity_out->out;
        tree_node_ptr *this_rel = tree_search_ptr(tree_entities_receiving->root, tree_entities_receiving, relationship_str);
        if (this_rel != nil_ptr) {
            this_rel->number -= 1;
            if (this_rel->number == 0)
                free(rb_delete_ptr(tree_entities_receiving, this_rel));
        }
        if (tree_entities_receiving->root == nil_ptr) {
            //rimuovo l'intero albero in quanto vuoto
            free(tree_entities_receiving);
            entity_out->out = NULL;
        }
    }
#endif
    
    //cancello la relazione dall'albero delle relazioni se non ci sono entità riceventi tale relazione
    if (this_relationship_in_relationship != nil && this_relationship_in_relationship->number == 0)
        free(rb_delete(relationships, this_relationship_in_relationship, false));
}


//Questa funzione cancella una entità dall'albero delle entità, eliminando anche tutte le relazioni di cui è origine o destinazione.
void delent(tree *entities_tree, tree *relationships, tree_node *user_to_be_deleted_in_ent_tree) {
    //Cancellando dall'albero delle relazioni
    tree_ptr *relationship_from_utbd_tree = user_to_be_deleted_in_ent_tree->in;
    if (relationship_from_utbd_tree != NULL) {
        tree_node_ptr *minimum_of_relationship_from_utbd_tree = tree_minimum_ptr(relationship_from_utbd_tree->root, relationship_from_utbd_tree); //minima relazione entrante all'entità
        while (minimum_of_relationship_from_utbd_tree != nil_ptr) {
            tree_node_ptr *temp = tree_successor_ptr(minimum_of_relationship_from_utbd_tree, relationship_from_utbd_tree);
            tree_ptr *entity_providing_this_rel = minimum_of_relationship_from_utbd_tree->data;
            tree_node_ptr *minimum_entity_providing_this_rel = tree_minimum_ptr(entity_providing_this_rel->root, entity_providing_this_rel); //minima entità che aveva prodotto quella relazione
            while (minimum_entity_providing_this_rel != nil_ptr) {
                tree_node_ptr *temp = tree_successor_ptr(minimum_entity_providing_this_rel, entity_providing_this_rel);
                tree_node *entity_out = tree_search(entities_tree->root, entities_tree, minimum_entity_providing_this_rel->key_ptr);
                tree_node *entity_in = tree_search(entities_tree->root, entities_tree, user_to_be_deleted_in_ent_tree->key);
#if PRINTING_COMMAND
                printf("Rel tree: %s %s %s\n", entity_out->key, entity_in->key, minimum_of_relationship_from_utbd_tree->key_ptr);
#endif
                delrel(entities_tree, relationships, entity_out->key, entity_in->key, minimum_of_relationship_from_utbd_tree->key_ptr, entity_out, entity_in);
                minimum_entity_providing_this_rel = temp;
            }
            minimum_of_relationship_from_utbd_tree = temp;
        }
    }
    
#if OLD_DELENT
    //Cancellando le relazioni delle entità che ricevono una relazione dall'entità da eliminare. MOLTO LENTO (ricerca esaustiva di tutti i sottoalberi), MA FUNZIONANTE. Legacy.
    tree_node *min = tree_minimum(entities_tree->root, entities_tree); //prima entità
    while (min != nil) {
        if (min->in != NULL) {
            tree_node_ptr *min_rel = tree_minimum_ptr(min->in->root, min->in); //prima relazione
            while (min_rel != nil_ptr) {
                tree_node_ptr *temp2 = tree_successor_ptr(min_rel, min->in);
                if (min_rel->data != NULL) {
                    tree_node_ptr *min2 = tree_minimum_ptr(min_rel->data->root, min_rel->data); //prima entità dentro relazione
                    while (min2 != nil_ptr) {
                        tree_node_ptr *temp3 = tree_successor_ptr(min2, min_rel->data);
                        char *k = temp3->key_ptr;
                        if (strcmp(min2->key_ptr, user_to_be_deleted_in_ent_tree->key) == 0) { //se effettivamente è presente una relazione
                            tree_node *entity_out = user_to_be_deleted_in_ent_tree;
                            tree_node *entity_in = tree_search(entities_tree->root, entities_tree, min->key);
                            delrel(entities_tree, relationships, entity_out->key, entity_in->key, min_rel->key_ptr, entity_out, entity_in);
                            if (temp3 != nil_ptr && min_rel->data != NULL)
                                temp3 = tree_search_ptr(min_rel->data->root, min_rel->data, k);
                        }
                        min2 = temp3;
                        
                    }
                    min_rel = temp2;
                }
            }
        }
        tree_node *temp = tree_successor(min, entities_tree);
        min = temp;
    }
#endif
    
#if FAST_DELENT
    //sfrutta la struttura dati apposita per ridurre il numero delle delrel, evitando di scorrere tutto l'albero e utilizzando l'albero delle relazioni
    //k, k2 e k3 sono stringhe di appoggio per evitare che le delete o le free facciano scomparire il riferimento alla chiave.
    tree_ptr *out = user_to_be_deleted_in_ent_tree->out;
    tree_node_ptr *min_in_out = NULL;
    if (out != NULL)
        min_in_out = tree_minimum_ptr(out->root, out); //minima relazione prodotta dall'entità
    while (out != NULL && min_in_out != nil_ptr) {
        tree_node_ptr *succ = tree_successor_ptr(min_in_out, out);
        char k3[SIZE_KEY];
        if (succ != nil_ptr)
            strcpy(k3, succ->key_ptr);
        tree_node *this_rel_in_rel_tree = tree_search(relationships->root, relationships, min_in_out->key_ptr); //ritrovo min_in_out nell'albero delle relazioni
        tree_node_ptr *min_in_trirt = tree_minimum_ptr(this_rel_in_rel_tree->in->root, this_rel_in_rel_tree->in); //minima entità che riceve quella relazione nell'albero delle relazioni
        char k2[SIZE_KEY];
        if (out != NULL && min_in_out != nil_ptr && min_in_out != NULL && min_in_out->key_ptr != NULL)
            strcpy(k2, min_in_out->key_ptr);
        while (min_in_trirt != nil_ptr) {
            tree_node_ptr *succ2 = tree_successor_ptr(min_in_trirt, this_rel_in_rel_tree->in);
            char k[SIZE_KEY];
            if (succ2 != nil_ptr)
                strcpy(k, succ2->key_ptr);
            tree_node *this_rec_ent = tree_search(entities_tree->root, entities_tree, min_in_trirt->key_ptr); //trovo l'entità ricevente nell'albero delle relazioni
            boolean _is_there_a_rel = is_there_a_relationship(entities_tree, user_to_be_deleted_in_ent_tree, this_rec_ent, k2);
            if (_is_there_a_rel == true) {
                delrel(entities_tree, relationships, user_to_be_deleted_in_ent_tree->key, this_rec_ent->key, k2, user_to_be_deleted_in_ent_tree, this_rec_ent);
                if (succ2 != nil_ptr)
                    succ2 = tree_search_ptr(this_rel_in_rel_tree->in->root, this_rel_in_rel_tree->in, k);
            }
            min_in_trirt = succ2;
        }
        if (succ != nil_ptr)
            succ = tree_search_ptr(out->root, out, k3);
        min_in_out = succ;
    }
#endif
    free(rb_delete(entities_tree, user_to_be_deleted_in_ent_tree, true));
}



/************************************************************************
 ************************************************************************
 ************************************************************************
 ********************************M-A-I-N*********************************
 ************************************************************************
 ************************************************************************
 ***********************************************************************/
int main(int argc, const char * argv[]) {
#if TEST_SIZE_STRUCT
    printf("sizeof tree: %d ; tree_node: %d ; tree_ptr: %d ; tree_node_ptr: %d\n", sizeof(tree), sizeof(tree_node), sizeof(tree_ptr), sizeof(tree_node_ptr));
#endif
    //Inizializzo le due variabili globali nil
    nil = calloc(1, sizeof(tree_node));
    nil_ptr = calloc(1, sizeof(tree_node_ptr));
    
    tree *entities_tree = malloc(sizeof(tree)); //albero delle entità
    entities_tree->root = nil;
    
    tree *relationships = malloc(sizeof(tree)); //albero delle relazioni
    relationships->root = nil;
    
    //stringhe di supporto per la lettura dell'input (allocate e inizializzate)
    char input_string[BUFFER_SIZE];
    memset(input_string, '\0', BUFFER_SIZE);
    char command[7];
    memset(command, 0, 7);
    char string_parsed[BUFFER_SIZE];
    memset(string_parsed, '\0', BUFFER_SIZE);
    char relationship_str[SIZE_KEY];
    memset(relationship_str, '\0', SIZE_KEY);
    
    
    //-------------------------------->
    //                  inizio ciclo principale do - while
    //                                <---------------------------------
    do {
        //reset
        memset(command, '\0', 7);
        memset(input_string, '\0', BUFFER_SIZE);
        memset(string_parsed, '\0', BUFFER_SIZE);
        fgets(input_string, BUFFER_SIZE, stdin);
#if PRINTING_COMMAND
        printf("\n***** %s ", input_string); //permette di stampare l'input - debugging
#endif
        strncpy(command, input_string, 6);
        
        
#if ADDENT
        if (strcmp(command, "addent") == 0) {
            int i = 0;
            for (i = 8; input_string[i] != '\n' && input_string[i] != '\0'; i++)
                if (input_string[i] != '\"')
                    string_parsed[i-8] = input_string[i];
            addent(entities_tree, string_parsed, i-7);
        }
#endif
        
        
#if DELENT
        if (strcmp(command, "delent") == 0) {
            for (int i = 8; input_string[i] != '\n' && input_string[i] != '\0'; i++)
                if (input_string[i] != '\"')
                    string_parsed[i-8] = input_string[i];
            
            tree_node *user_to_be_deleted_in_ent_tree = tree_search(entities_tree->root, entities_tree, string_parsed);
            if (user_to_be_deleted_in_ent_tree != nil) {
                delent(entities_tree, relationships, user_to_be_deleted_in_ent_tree);
            }
        }
#endif
        
        
#if ADDREL
        if (strcmp(command, "addrel") == 0) {
            //variabili di supporto per le stringhe
            char id_origin[SIZE_KEY];
            char id_dest[SIZE_KEY];
            memset(id_origin, '\0', SIZE_KEY);
            memset(id_dest, '\0', SIZE_KEY);
            memset(relationship_str, '\0', SIZE_KEY);
            int latest = 0;
            
            //id_origin
            for (int i = 8; input_string[i] != '\n' && input_string[i] != '\0'; i++) {
                if (input_string[i] != '\"') {
                    id_origin[i-8] = input_string[i];
                    latest++;
                } else {
                    break;
                }
            }
            int old_latest = latest;
            
            //id_dest
            for (int i = 11+latest; input_string[i] != '\n' && input_string[i] != '\0'; i++) {
                if (input_string[i] != '\"') {
                    id_dest[i-11-old_latest] = input_string[i];
                    latest++;
                } else {
                    break;
                }
            }
            
            tree_node *entity_out = tree_search(entities_tree->root, entities_tree, id_origin);
            tree_node *entity_in = tree_search(entities_tree->root, entities_tree, id_dest);
            int i = 0;
            if (entity_out != NULL && entity_out != nil && entity_in != NULL && entity_in != nil) {
                for (i = 14+latest; input_string[i] != '\n' && input_string[i] != '\0'; i++) {
                    if (input_string[i] != '\"') {
                        relationship_str[i-latest-14] = input_string[i];
                    }
                }
                int size_ = i-latest-13;
                addrel(entities_tree, relationships, entity_out, entity_in, relationship_str, size_);
            }
        }
#endif
        
        
#if DELREL
        if (strcmp(command, "delrel") == 0) {
            char id_origin[SIZE_KEY];
            char id_dest[SIZE_KEY];
            memset(id_origin, '\0', SIZE_KEY);
            memset(id_dest, '\0', SIZE_KEY);
            memset(relationship_str, '\0', SIZE_KEY);
            int latest = 0;
            
            //id_origin
            for (int i = 8; input_string[i] != '\n' && input_string[i] != '\0'; i++) {
                if (input_string[i] != '\"') {
                    id_origin[i-8] = input_string[i];
                    latest++;
                } else {
                    break;
                }
            }
            int old_latest = latest;
            
            //id_dest
            for (int i = 11+latest; input_string[i] != '\n' && input_string[i] != '\0'; i++) {
                if (input_string[i] != '\"') {
                    id_dest[i-11-old_latest] = input_string[i];
                    latest++;
                } else {
                    break;
                }
            }
            tree_node *entity_out = tree_search(entities_tree->root, entities_tree, id_origin);
            tree_node *entity_in = tree_search(entities_tree->root, entities_tree, id_dest);
            
            if (entity_out != nil && entity_in != nil) {
                for (int i = 14+latest; input_string[i] != '\n' && input_string[i] != '\0'; i++) {
                    if (input_string[i] != '\"') {
                        relationship_str[i-latest-14] = input_string[i];
                    }
                }
                boolean _is_there_a_rel_ = is_there_a_relationship(entities_tree, entity_out, entity_in, relationship_str);
                
                //qua mi occupo dell'albero delle relazioni
#if OLD_DELENT
                if (_is_there_a_rel_ == true && entity_in->in != NULL && entity_out != NULL)
#endif
#if FAST_DELENT
                    if (_is_there_a_rel_ == true && entity_in->in != NULL && entity_out->out != NULL)
#endif
                        delrel(entities_tree, relationships, id_origin, id_dest, relationship_str, entity_out, entity_in);
            }
        }
#endif
        
        
#if REPORT
        if (strcmp(command, "report") == 0) {
#if OLD_REPORT
            walking(relationships->root);
            if (relationships->root == nil)
                fputs("none", stdout);
            fputs("\n", stdout);
#endif
#if FAST_REPORT
            walking_fast(&relationships);
            if (relationships->root == nil)
                fputs("none", stdout);
            fputs("\n", stdout);
#endif
        }
#endif
        
    } while (strcmp(command, "end") != 0 && strcmp(command, "end\n") != 0 && strcmp(command, "end\r\n"));
    
    return 0;
}

//Stefano Maxenti - 865725 - 10526141
