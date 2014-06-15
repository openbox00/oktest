#ifndef __OKL4__TYPES_H__
#define __OKL4__TYPES_H__

#include <okl4/kernel.h>
#include <stddef.h>
#include <stdint.h>

#include <l4/types.h>
#include <l4/utcb.h>

typedef enum _okl4_mem_container_type {
    _OKL4_TYPE_MEMSECTION,
    _OKL4_TYPE_ZONE,
    _OKL4_TYPE_EXTENSION
} _okl4_mem_container_type_t;

typedef word_t okl4_word_t;

typedef int8_t    okl4_s8_t;
typedef int16_t   okl4_s16_t;
typedef int32_t   okl4_s32_t;
typedef uint8_t   okl4_u8_t;
typedef uint16_t  okl4_u16_t;
typedef uint32_t  okl4_u32_t;



/* LibOKL4 Typedefs. */
typedef struct _okl4_mem_container _okl4_mem_container_t;
typedef struct _okl4_mcnode _okl4_mcnode_t;
typedef struct _okl4_pd_thread _okl4_pd_thread_t;
typedef struct okl4_allocator_attr okl4_allocator_attr_t;
typedef struct okl4_barrier okl4_barrier_t;
typedef struct okl4_barrier_attr okl4_barrier_attr_t;
typedef struct okl4_bitmap_allocator okl4_bitmap_allocator_t;
typedef struct okl4_bitmap_allocator okl4_kclistid_pool_t;
typedef struct okl4_bitmap_allocator okl4_kmutexid_pool_t;
typedef struct okl4_bitmap_allocator okl4_kspaceid_pool_t;
typedef struct okl4_bitmap_item okl4_bitmap_item_t;
typedef struct okl4_env okl4_env_t;
typedef struct okl4_envitem okl4_envitem_t;
typedef struct okl4_env_args okl4_env_args_t;
typedef struct okl4_env_device_irqs okl4_env_device_irqs_t;
typedef struct okl4_env_kernel_info okl4_env_kernel_info_t;
typedef struct okl4_env_segment okl4_env_segment_t;
typedef struct okl4_env_segments okl4_env_segments_t;
typedef struct okl4_extension okl4_extension_t;
typedef struct okl4_extension_attr okl4_extension_attr_t;
typedef struct okl4_extension_token okl4_extension_token_t;
typedef struct okl4_irqset okl4_irqset_t;
typedef struct okl4_irqset_attr okl4_irqset_attr_t;
typedef struct okl4_irqset_deregister_attr okl4_irqset_deregister_attr_t;
typedef struct okl4_irqset_register_attr okl4_irqset_register_attr_t;
typedef struct okl4_kcap_item okl4_kcap_item_t;
typedef struct okl4_kclist okl4_kclist_t;
typedef struct okl4_kclist_attr okl4_kclist_attr_t;
typedef struct okl4_kspace okl4_kspace_t;
typedef struct okl4_kspace_attr okl4_kspace_attr_t;
typedef struct okl4_kspace_map_attr okl4_kspace_map_attr_t;
typedef struct okl4_kspace_unmap_attr okl4_kspace_unmap_attr_t;
typedef struct okl4_kthread okl4_kthread_t;
typedef struct okl4_kthread_attr okl4_kthread_attr_t;
typedef struct okl4_memsec okl4_memsec_t;
typedef struct okl4_memsec_attr okl4_memsec_attr_t;
typedef struct okl4_mutex okl4_mutex_t;
typedef struct okl4_mutex_attr okl4_mutex_attr_t;
typedef struct okl4_pd okl4_pd_t;
typedef struct okl4_pd_attach_attr_t okl4_pd_attach_attr_t;
typedef struct okl4_pd_attr okl4_pd_attr_t;
typedef struct okl4_pd_dict okl4_pd_dict_t;
typedef struct okl4_pd_dict_attr okl4_pd_dict_attr_t;
typedef struct okl4_pd_thread_create_attr okl4_pd_thread_create_attr_t;
typedef struct okl4_pd_zone_attach_attr okl4_pd_zone_attach_attr_t;
typedef struct okl4_physmem_item okl4_physmem_item_t;
typedef struct okl4_physmem_pagepool okl4_physmem_pagepool_t;
typedef struct okl4_physmem_pool_attr okl4_physmem_pool_attr_t;
typedef struct okl4_physmem_segpool okl4_physmem_segpool_t;
typedef struct okl4_range_item okl4_range_item_t;
typedef struct okl4_range_item okl4_virtmem_item_t;
typedef struct okl4_range_allocator okl4_range_allocator_t;
typedef struct okl4_semaphore okl4_semaphore_t;
typedef struct okl4_semaphore_attr okl4_semaphore_attr_t;
typedef struct okl4_static_memsec okl4_static_memsec_t;
typedef struct okl4_static_memsec_attr okl4_static_memsec_attr_t;
typedef struct okl4_utcb_area okl4_utcb_area_t;
typedef struct okl4_utcb_area_attr okl4_utcb_area_attr_t;
typedef struct okl4_utcb_item okl4_utcb_item_t;
typedef struct okl4_virtmem_pool okl4_virtmem_pool_t;
typedef struct okl4_virtmem_pool_attr okl4_virtmem_pool_attr_t;
typedef struct okl4_zone okl4_zone_t;
typedef struct okl4_zone_attr okl4_zone_attr_t;


#endif /* !__OKL4__TYPES_H__ */
