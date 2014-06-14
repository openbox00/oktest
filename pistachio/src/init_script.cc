#include <tcb.h>
#include <schedule.h>
#include <space.h>
#include <config.h>
#include <arch/hwspace.h>
#include <kernel/generic/lib.h>
#include <init.h>
#include <l4/map.h>

typedef struct
{
    word_t magic;
    word_t version;
} ki_header_t;

typedef enum
{
    KI_OP_CREATE_HEAP           = 1,

    KI_OP_INIT_IDS              = 2,
    KI_OP_CREATE_SPACE          = 5,
    KI_OP_CREATE_THREAD         = 6,
    KI_OP_CREATE_SEGMENT        = 8,
    KI_OP_MAP_MEMORY            = 9,
} ki_op_t;

typedef union
{
    struct {
        BITFIELD3(
            word_t,
            size         : (BITS_WORD/2), /* word unit, including header */
            op           : (BITS_WORD/2-1),
            end_of_phase : 1);
    };
} ki_hdr_t;

#define SHIFT_4K (12)
#define SHIFT_1K (10)

typedef struct
{
    ki_hdr_t    hdr;
    BITFIELD2(
        word_t,
        size      : SHIFT_4K,                 /* Treat as multiples of 4K */
        phys      : (BITS_WORD - SHIFT_4K));  /* 4K Aligned */
} ki_create_heap_t;

/*
 * implicit argument: clist
 */
typedef struct
{
    ki_hdr_t    hdr;
    word_t      id;
    BITFIELD2(
        word_t,
        space_id_base  : (BITS_WORD / 2),
        space_id_num   : (BITS_WORD / 2));
    BITFIELD2(
        word_t,
        mutex_id_base  : (BITS_WORD / 2),
        mutex_id_num   : (BITS_WORD / 2));

    BITFIELD2(
        word_t,
        utcb_area_log2_size   : SHIFT_1K,
        utcb_area_base        : (BITS_WORD - SHIFT_1K));

    BITFIELD3(
        word_t,
        num_physical_segs : (BITS_WORD / 2),
        has_kresources    : 1,
        max_priority      : ((BITS_WORD / 2) - 1));

} ki_create_space_t;

/*
 * implicit argument: space
 */
typedef struct
{
    ki_hdr_t    hdr;
    BITFIELD2(
        word_t,
        cap_slot         : (BITS_WORD / 2),
        priority         : (BITS_WORD / 2));
    word_t      ip;
    word_t      sp;
    word_t      utcb_address;
    word_t      mr1;
} ki_create_thread_t;

/*
 * implicit argument: space
 */
typedef struct
{
    ki_hdr_t       hdr;
    word_t         seg_num;
    phys_segment_t segment;
} ki_create_segment_t;

/*
 * implicit argument: space, segment
 */
typedef struct
{
    ki_hdr_t     hdr;
    L4_MapItem_t desc;
} ki_map_memory_t;

/*
 * points to the an array of operations filled by Elfweaver
 */
static ki_header_t *ki_init_script;

static INLINE
void ki_set_ptr(ki_hdr_t* hdr, void* ptr)
{
    *((word_t *)hdr) = (word_t)ptr;
}

static kmem_resource_t SECTION(SEC_INIT)*
ki_create_heap(ki_create_heap_t *args)
{
    kmem_resource_t* res = (kmem_resource_t *)phys_to_virt(args->phys << SHIFT_4K);
    addr_t end_mem = (addr_t)((word_t)res + (args->size * (1024 * 4)));
    res->init(end_mem);
    return res;
}

static space_t SECTION(SEC_INIT)*
ki_create_space(kmem_resource_t * res, clist_t* clist, ki_create_space_t* args)
{
    space_t * space = allocate_space(res, spaceid(args->id), clist);   
    fpage_t utcb_area;
    space->init(utcb_area, res);

    if (args->has_kresources) {
        space->set_kmem_resource(res);
    }

    if (args->num_physical_segs > 0) {
        space->phys_segment_list = (segment_list_t*)res->alloc(kmem_group_physseg_list, sizeof(segment_list_t) + ((args->num_physical_segs - 1) * sizeof(phys_segment_t*)), true);
        space->phys_segment_list->set_num_segments(args->num_physical_segs);
    }

    return space;

}

extern "C" CONTINUATION_FUNCTION(initial_to_user);

INLINE void set_running_and_enqueue(tcb_t * tcb)
{
    get_current_scheduler()->enqueue(tcb);
}

static tcb_t SECTION(SEC_INIT) *
    ki_create_thread(space_t *space, ki_create_thread_t *args,
                     kmem_resource_t *res)
{
    capid_t dest_tid;
    tcb_t *tcb = allocate_tcb(dest_tid, res);
    /* set the space */
    tcb->set_space(space);

    /* activate the guy */
    tcb->activate(initial_to_user, res);
    tcb->set_user_ip((addr_t)args->ip);
    tcb->set_user_sp((addr_t)args->sp);
    tcb->set_mr(1, args->mr1);
    set_running_and_enqueue(tcb);
    return tcb;
}

static void SECTION(SEC_INIT)
ki_create_segment(space_t* space, ki_create_segment_t *args)
{
    phys_segment_t *segment;
    segment = space->phys_segment_list->lookup_segment(args->seg_num);
    *segment = args->segment;
}

extern bool map_region (space_t * space, word_t vaddr, word_t paddr, word_t size, word_t attr, word_t rwx, kmem_resource_t *kresource);

#define MAP_PGSIZE(pgs) (1UL << pgs)

static void SECTION(SEC_INIT)
    ki_map_memory(space_t* space, ki_map_memory_t *args, kmem_resource_t *cur_heap)
{
    word_t size = args->desc.size.X.num_pages * MAP_PGSIZE(args->desc.size.X.page_size);
    phys_segment_t *segment = space->phys_segment_list->lookup_segment(args->desc.seg.X.segment);
    word_t phys = segment->get_base() + (args->desc.seg.X.offset << SHIFT_1K);

    if (!map_region(space, args->desc.virt.X.vbase << SHIFT_1K,
                    phys, size, args->desc.virt.X.attr,
                    args->desc.size.X.rwx, cur_heap)) {
    }
}

static ki_hdr_t* SECTION(SEC_INIT)
ki_get_first_op()
{
    word_t init_script_paddr = get_init_data()->init_script;
    ki_init_script = (ki_header_t *)phys_to_virt(init_script_paddr);
    return (ki_hdr_t *)(ki_init_script + 1);
}

void SECTION(SEC_INIT)
run_init_script(word_t phase)
{
    static ki_hdr_t *cur;
    ki_hdr_t cur_saved;
    kmem_resource_t *cur_heap;
    clist_t *cur_clist = NULL;
    space_t *cur_space = NULL;
    tcb_t *cur_tcb = NULL;

    if (phase == INIT_PHASE_FIRST_HEAP) {
        cur = ki_get_first_op();
        cur_heap = NULL;
    } else {
        cur_heap = get_current_kmem_resource();
    }

    do {
        cur_saved = *cur;

        switch (cur->op) {
        case KI_OP_CREATE_HEAP: {
            ki_create_heap_t *args = (ki_create_heap_t *)cur;
            cur_heap = ki_create_heap(args);
            if (!get_kernel_space()->get_kmem_resource()) {
                get_kernel_space()->set_kmem_resource(cur_heap);
            }
            break;
        }
        case KI_OP_CREATE_SPACE: {
            ki_create_space_t *args = (ki_create_space_t *)cur;
            cur_space = ki_create_space(cur_heap, cur_clist, args);
            break;
        }
        case KI_OP_CREATE_THREAD: {
            ki_create_thread_t *args = (ki_create_thread_t *)cur;
            cur_tcb = ki_create_thread(cur_space, args, cur_heap);
            ki_set_ptr(cur, cur_tcb);
            break;
        }
        case KI_OP_CREATE_SEGMENT: {
            ki_create_segment_t *args = (ki_create_segment_t *)cur;
            ki_create_segment(cur_space, args);
            break;
        }
        case KI_OP_MAP_MEMORY: {
            ki_map_memory_t *args = (ki_map_memory_t *)cur;
            ki_map_memory(cur_space, args, cur_heap);
            break;
        }
        default:
            break;
        }
        cur = (ki_hdr_t *)((word_t *)cur  + cur_saved.size);
    } while (!cur_saved.end_of_phase);
}
