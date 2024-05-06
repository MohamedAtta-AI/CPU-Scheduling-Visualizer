#include <stdio.h>
#include <stdlib.h>


struct Node {
    int start; int end;
    struct Node* next;
};


struct Node* createNode(int start, int end) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    newNode->start = start;
    newNode->end = end;
    newNode->next = NULL;
    return newNode;
}


// void insertAtBeginning(struct Node** headRef, int data) {
//     struct Node* newNode = createNode(data);
//     newNode->next = *headRef;
//     *headRef = newNode;
// }


void insertAtEnd(struct Node** headRef, int start, int end) {
    struct Node* newNode = createNode(start, end);
    if (*headRef == NULL) {
        *headRef = newNode;
        return;
    }
    struct Node* current = *headRef;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}


void printList(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        printf("Start: %d End:%d\n", current->start, current->end);
        current = current->next;
    }
    printf("\n");
}


void freeList(struct Node* head) {
    struct Node* current = head;
    while (current != NULL) {
        struct Node* temp = current;
        current = current->next;
        free(temp);
    }
}