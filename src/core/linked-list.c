#include "linked-list.h"

#include <stdio.h>

#include "../globals.h"
#include "../log.h"
#include "./mem.h"

global_internal int id_counter = 0;

global_internal linked_list_node_t *create_node(void *data) {
    linked_list_node_t *node =
        (linked_list_node_t *)reserve_mem(sizeof(linked_list_node_t));
    if (node) {
        node->data = data;
        node->next = NULL;
        node->id = id_counter++;
    }
    return node;
}

linked_list_t linked_list_new() {
    return (linked_list_t){.head = NULL, .size = 0};
}

linked_list_node_t *linked_list_get(const linked_list_t *list, int id) {
    if (!list->head || list->size <= 0) {
        return NULL;
    }
    linked_list_node_t *current = list->head;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int linked_list_append(linked_list_t *list, void *data) {
    linked_list_node_t *node = create_node(data);
    if (list->head == NULL) {
        list->head = node;
    } else {
        linked_list_node_t *current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
    list->size++;
    return node->id;
}

void linked_list_remove(linked_list_t *list, int id) {
    if (!list->head || list->size <= 0) {
        return;
    }
    linked_list_node_t *current = list->head;
    for (int i = 0; i < list->size; i++) {
        if (!current) return;
        if (current->id == id) {
            if (i == 0) {
                linked_list_node_t *tmp = list->head;
                list->head = list->head->next;
                free_mem(tmp);
            } else {
                linked_list_node_t *prev = linked_list_get(list, i - 1);
                if (!prev) {
                    log_warning("failed to remove item from %p index %d", list,
                                i);
                    return;
                }
                linked_list_node_t *target = prev->next;
                prev->next = target->next;
                free_mem(target);
            }
            list->size--;
            return;
        }
        current = current->next;
    }
}

void linked_list_clear(linked_list_t *list) {
    linked_list_node_t *current = list->head;
    while (current != NULL) {
        linked_list_node_t *tmp = current;
        current = current->next;
        free_mem(tmp);
    }
    list->head = NULL;
    list->size = 0;
}

