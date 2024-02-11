#ifndef INTERPLAY_CORE_LINKED_LIST_H_
#define INTERPLAY_CORE_LINKED_LIST_H_

#define linked_list_search_name(type, var, list, target) \
    {                                                    \
        linked_list_node_t *current = list.head;         \
        while (current != NULL) {                        \
            type *data = (type *)current->data;          \
            if (strcmp(data->name, target) == 0) {       \
                var = data;                              \
                break;                                   \
            }                                            \
            current = current->next;                     \
        }                                                \
    }

typedef struct linked_list_node {
    int id;
    void *data;
    struct linked_list_node *next;
} linked_list_node_t;

typedef struct linked_list {
    linked_list_node_t *head;
    int size;
} linked_list_t;

linked_list_t linked_list_new();

linked_list_node_t *linked_list_get(const linked_list_t *list, int index);

int linked_list_append(linked_list_t *list, void *data);

void linked_list_remove(linked_list_t *list, int index);

void linked_list_clear(linked_list_t *list);

#endif  // INTERPLAY_CORE_LINKED_LIST_H_
