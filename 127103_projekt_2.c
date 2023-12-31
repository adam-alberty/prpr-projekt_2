#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ID {
    char oznacenie[2];
    char cislovanie[4];
    char druh[2];
} ID;

typedef struct Position {
    double latitude;
    double longitude;
} Position;

typedef struct Node {
    ID id;
    Position position;
    char type[3];
    double value;
    int time;
    int date;

    struct Node *next;
} Node;

typedef struct LinkedList {
    Node *head;
    Node *tail;
} LinkedList;


// push node to the list
void append_node(LinkedList *ll, Node *node) {
    node->next = NULL;
    if (ll->tail == NULL) {
        ll->head = node;
        ll->tail = node;
    } else {
        ll->tail->next = node;
        ll->tail = node;
    }
}

// k - deallocate list
void free_list(LinkedList *ll) {
    Node *node = ll->head;
    while (node != NULL) {
        Node *node_next = node->next;
        free(node);
        node = node_next;
    }
    ll->head = NULL;
    ll->tail = NULL;
}

// n - load items to the list
void load(LinkedList *ll) {
    free_list(ll);
    FILE *fp = fopen("dataloger_V2.txt", "r");
    if (fp == NULL) {
        printf("Zaznamy neboli nacitane!\n");
    }

    char buff[20];
    int i = 0;
    Node *node;
    int num_of_loaded = 0;
    while (fgets(buff, 20, fp) != NULL) {
        switch(i) {
            case 0:
                num_of_loaded++;
                node = (Node *)malloc(sizeof(Node));
                if (node == NULL) {
                    printf("Zaznamy neboli nacitane!");
                    return;
                }
            case 1: // ID - ozn, cis, druh
                char *buff_cpy = buff;
                strncpy(node->id.oznacenie, buff_cpy, 1);
                node->id.oznacenie[1] = '\0';

                buff_cpy += 1;
                strncpy(node->id.cislovanie, buff_cpy, 3);
                node->id.cislovanie[3] = '\0';

                buff_cpy += 3;
                strncpy(node->id.druh, buff_cpy, 1);
                node->id.druh[1] = '\0';
                break;
            case 2: // Position - lat, long
                char lat[9];
                char lon[9];

                sscanf(buff, "%8s%8s", &lat, &lon);
                node->position.latitude = atof(lat);
                node->position.longitude = atof(lon);
                break;
            case 3: // Type - RD, RM, ...
                strncpy(node->type, buff, 2);
                node->type[2] = '\0';
                break;
            case 4: // Value
                node->value = atof(buff);
                break;
            case 5: // Time
                node->time = atoi(buff);
                break;
            case 6: // Date + append
                node->date = atoi(buff);
                append_node(ll, node);
        }
        i++;
        if (i == 7) i = 0;
    }
    printf("Nacitalo sa %d zaznamov\n", num_of_loaded);
    fclose(fp);
}

// Assemble ID from its parts
void get_id_from_parts(char *dest, ID id) {
    strcpy(dest, id.oznacenie);
    strcat(dest, id.cislovanie);
    strcat(dest, id.druh);
}

// v - Display the data
void show(LinkedList *ll) {
    Node *node = ll->head;

    int i = 1;
    while (node != NULL) {
        char id_full[6];
        get_id_from_parts(id_full, node->id);

        printf("%d:\n", i);
        printf("ID: %s\t%s\t%.2lf\n", id_full, node->type, node->value);
        printf("Poz: %.4lf\t%.4lf\n", node->position.latitude, node->position.longitude);
        printf("DaC: %08d\t%04d\n", node->date, node->time);

        node = node->next;
        i++;
    }
}

// p - Add entry to a specified position
void insert_entry(LinkedList *ll) {
    int c1;
    scanf("%d", &c1);

    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL) {
        printf("Nepodarilo sa alokovat");
        return;
    }

    scanf(" %1s%3s%1s", new_node->id.oznacenie, new_node->id.cislovanie, new_node->id.druh);
    new_node->id.oznacenie[1] = '\0';
    new_node->id.cislovanie[3] = '\0';
    new_node->id.druh[1] = '\0';
    scanf(" %8lf%8lf", &new_node->position.latitude, &new_node->position.longitude);
    scanf(" %2s", new_node->type);
    new_node->type[2] = '\0';
    scanf(" %lf", &new_node->value);
    scanf(" %d", &new_node->time);
    scanf(" %d", &new_node->date);

    Node *node = ll->head;
    Node *prev = NULL;
    int pos = 1;
    while (node != NULL) {
        if (pos == c1) {
            break;
        }
        prev = node;
        node = node->next;
        pos++;
    }

    // It's head
    if (prev == NULL) {
        new_node->next = ll->head;
        ll->head = new_node;

        if (node == NULL) {
            ll->tail = new_node;
        }

        return;
    }

    if (node == NULL) {
        prev->next = new_node;
        new_node->next = NULL;
        ll->tail = new_node;
        return;
    }

    prev->next = new_node;
    new_node->next = node;


}

// z - Delete entry from the list
void delete_entry(LinkedList *ll) {
    char id_to_delete[6];
    scanf("%5s", id_to_delete);

    Node *node = ll->head;
    Node *prev_node = NULL;
    char node_id[6];
    while (node != NULL) {
        get_id_from_parts(node_id, node->id);

        // Get node after current
        Node *next = node->next;
        
        if (strcmp(node_id, id_to_delete) == 0) {
            // Connect previous and next
            if (prev_node != NULL) {
                prev_node->next = next;
            } else {
                ll->head = next;
            }

            if (next == NULL) {
                ll->tail = prev_node;
            } 
        
            // Free current
            free(node);
            printf("Zaznam pre ID: %s bol vymazany.\n", id_to_delete);
        } else {
            // Set current node as previous for the next iteration
            prev_node = node;
        }

        // Go to the next node
        node = next;
    }
}

// Compare datetimes for sorting
int compare_datetimes(int date_1, int time_1, int date_2, int time_2) {
    return date_1 + time_1 > date_2 + time_2 ? 1 : 0;
}

Node *copy_node(Node *node) {
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        return NULL;
    }

    strcpy(new_node->id.oznacenie, node->id.oznacenie);
    strcpy(new_node->id.cislovanie, node->id.cislovanie);
    strcpy(new_node->id.druh, node->id.druh);
    strcpy(new_node->type, node->type);
    new_node->position.latitude = node->position.latitude;
    new_node->position.longitude = node->position.longitude;
    new_node->value = node->value;
    new_node->time = node->time;
    new_node->date = node->date;

    return new_node;
}

// u - Sort the list by datetime - insertion sort
void sort_list(LinkedList *ll) {
    LinkedList sorted_ll = {
        .head = NULL,
        .tail = NULL
    };
    Node *node = ll->head;

    int i = 1;
    while (node != NULL) {
        Node *current_node = ll->head;
        Node *prev_node = NULL;
        Node *max_node = ll->head;
        Node *max_node_prev = NULL;

        while (current_node != NULL) {
            // if date of current node is greater than the max node - swap nodes
            if (compare_datetimes(current_node->date, current_node->time, max_node->date, max_node->time)) {
                max_node = current_node;
                max_node_prev = prev_node;
            }

            prev_node = current_node;
            current_node = current_node->next;
        }

        append_node(&sorted_ll, copy_node(max_node));

        // Delete node from the list
        if (max_node_prev == NULL) {
             ll->head = max_node->next;
        } else {
            max_node_prev->next = max_node->next;
        }
        
        if (max_node->next == NULL) {
            ll->tail = max_node_prev;
        }

        free(max_node);

        node = ll->head;
    }

    free_list(ll);
    ll->head = sorted_ll.head;
    ll->tail = sorted_ll.tail;
}

// r - Swap 2 nodes of a list
void swap_entries(LinkedList *ll) {
    int c1, c2;
    scanf("%d %d", &c1, &c2);

    if (c1 < 1 || c2 < 1) {
        printf("Invalid input\n");
        return;
    }

    if (c1 == c2) {
        return;
    }

    // Find nodes to swap
    int i = 1;
    Node *node = ll->head;
    Node *n1_prev = NULL, *n2_prev = NULL;
    int is_head = 0;
    int is_tail = 0;
    int is_adjacent = 0;

    if (1 == c1 || 1 == c2) {
        is_head = 1;
    }

    if (c1 - c2 == 1 || c1 - c2 == -1) {
        is_adjacent = 1;
    }

    while (node != NULL && (i < c1 || i < c2)) {
        if (i == c1 - 1 || i == c2 - 1) {
            if (n1_prev != NULL) {
                n2_prev = node;
            } else {
                n1_prev = node;
            }
        }
        i++;
        node = node->next;
    }

    if (n1_prev == NULL) {
        return;
    }

    if (!is_head && n2_prev == NULL) {
        return;
    }

    if (!is_head && n2_prev->next == NULL) {
        return;
    }


    if (is_head && n1_prev->next == NULL) {
        return;
    }

    if ((is_head && n1_prev->next->next == NULL) || (!is_head && n2_prev->next->next == NULL)) {
        is_tail = 1;
    }

    if (is_head && is_adjacent) {
        if (is_tail) {
            ll->tail = ll->head;
        }

        Node *head_prev = ll->head;
        ll->head = n1_prev->next;
        head_prev->next = ll->head->next;
        ll->head->next = head_prev;

    } else if (is_head) {
        if (is_tail) {
            ll->tail = ll->head;
        }
        Node *old_head = ll->head;
        Node *old_node_next_next = n1_prev->next->next;
        ll->head = n1_prev->next;
        n1_prev->next->next = old_head->next;
        n1_prev->next = old_head;
        old_head->next = old_node_next_next;

    } else if (is_adjacent) {
        if (is_tail) {
            ll->tail = n1_prev->next;
        }

        Node *n2_next = n2_prev->next->next;
        n1_prev->next = n2_prev->next;
        n2_prev->next->next = n2_prev;
        n2_prev->next = n2_next;

    } else {
        if (is_tail) {
            ll->tail = n1_prev->next;
        }

        Node *n1_old = n1_prev->next;
        Node *n1_old_next = n1_prev->next->next;
        Node *n2_old_next = n2_prev->next->next;
        n1_prev->next = n2_prev->next;
        n1_prev->next->next = n1_old->next;
        n2_prev->next = n1_old;
        n1_old->next = n2_old_next;
    }
}

int main(void) {
    char command;
    LinkedList ll = {
        .head = NULL,
        .tail = NULL
    };

    while(command = getchar()) {
        switch(command) {
            case 'n':
                load(&ll);
                break;
            case 'v':
                show(&ll);
                break;
            case 'p':
                insert_entry(&ll);
                break;
            case 'z':
                delete_entry(&ll);
                break;
            case 'u':
                sort_list(&ll);
                break;
            case 'r':
                swap_entries(&ll);
                break;
            case 'k':
                free_list(&ll);
                return 0;
        }
    }

    return 0;
}