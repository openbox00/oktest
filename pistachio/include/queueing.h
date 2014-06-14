/**
 * Description: Queue data structures.
 */
#ifndef __QUEUEING_H__
#define __QUEUEING_H__

/* head and object may never point to the same object. */
#define ENQUEUE_LIST_TAIL(type, head, object, list)                   \
    do {                                                              \
        ringlist_t<type> * RESTRICT object_list = &object->list;      \
                                                                      \
        if (head == NULL) {                                           \
            head = object;                                            \
            object_list->next = object_list->prev = object;           \
        }                                                             \
        else {                                                        \
            ringlist_t<type> * RESTRICT head_list = &head->list;      \
            object_list->next = head;                                 \
            object_list->prev = head_list->prev;                      \
            head_list->prev->list.next = object;                      \
            head_list->prev = object;                                 \
        }                                                             \
    } while(0)

#define DEQUEUE_LIST(type, head, object, list)                        \
    do {                                                              \
        ringlist_t<type> * object_list = &object->list;               \
                                                                      \
        if (object_list->next == object) {                            \
            head = NULL;                                              \
        }                                                             \
        else {                                                        \
            ringlist_t<type> * next_list = &object_list->next->list;  \
            ringlist_t<type> * prev_list = &object_list->prev->list;  \
            if (head == object) {                                     \
                head = object_list->next;                             \
            }                                                         \
            next_list->prev = object_list->prev;                      \
            prev_list->next = object_list->next;                      \
        }                                                             \
        object_list->next = object_list->prev = NULL;                 \
    } while(0)


template <class T> class ringlist_t
{
public:
    T * next;
    T * prev;

};

#endif /* !__QUEUEING_H__ */
