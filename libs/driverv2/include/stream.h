#ifndef _DRIVER_STREAM_H_
#define _DRIVER_STREAM_H_

struct stream_pkt;

#include <driver/stream_if.h>

struct stream_pkt {
    struct stream_pkt *next;
    uint8_t *data;
    size_t length;
    size_t xferred;
    int status;
    int flags;
    void * ref;
    void * priv;
};

static inline struct stream_pkt*
stream_get_head(struct stream_interface *si)
{
    return si->start;
}

static inline struct stream_pkt*
stream_switch_head(struct stream_interface *si)
{
    struct stream_pkt *oldhead = si->start;
    if (oldhead == NULL)
        return NULL;
    /* Fixup active list */
    si->start = oldhead->next;
    if (si->start == NULL) {
        si->end = NULL;
    }
    /* Fixup completed list */
    if (si->completed_end == NULL) {
        si->completed_start = oldhead;
    } else {
        si->completed_end->next = oldhead;
    }
    si->completed_end = oldhead;
    oldhead->next = NULL;
    return si->start;
}

static inline void
stream_add_stream_pkt(struct stream_interface *si, struct stream_pkt *stream_pkt)
{
    stream_pkt->next = NULL;
    if (si->end == NULL) {
        /* Empty */
        si->end = stream_pkt;
        si->start = stream_pkt;
        stream_sync(si);
    } else {
        si->end->next = stream_pkt;
        si->end = stream_pkt;
    }
}

static inline void
stream_complete_stream_pkt(struct stream_interface *si, struct stream_pkt *stream_pkt)
{
    if (si->completed_end == NULL) {
        si->completed_end = stream_pkt;
        si->completed_start = stream_pkt;
    } else {
        si->completed_end->next = stream_pkt;
        si->completed_end = stream_pkt;
    }
    stream_pkt->next = NULL;
}


#endif
