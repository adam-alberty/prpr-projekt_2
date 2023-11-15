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
    struct Node *prev;
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

// deallocate list
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
    printf("Nacitalo sa %d zaznamov\n", num_of_loaded); // TODO
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
        printf("ID: %s\t%s\t%lf\n", id_full, node->type, node->value);
        printf("Poz: %lf\t%lf\n", node->position.latitude, node->position.longitude);
        printf("DaC: %d\t%d\n", node->date, node->time);


        node = node->next;
        i++;
    }
}

// Add entry to a specified position
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
    int pos = 1;
    while (node != NULL) {
        if (pos == (c1 - 1)) {
            new_node->next = node->next;
            node->next = new_node;
            return;
        }
        pos++;
    }
    append_node(ll, new_node);
}

void delete_entry(LinkedList *ll) {
    char id[6];
    scanf("%5s", id);

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
        }
    }


    return 0;
}