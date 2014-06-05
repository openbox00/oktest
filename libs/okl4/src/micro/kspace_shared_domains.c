#if defined(ARM_SHARED_DOMAINS)

#include <l4/map.h>
#include <l4/arch/ver/space_resources.h>
#include <l4/space.h>

#include <okl4/kspace.h>
#include <okl4/errno.h>
#include <okl4/types.h>


int
_okl4_kspace_domain_share(okl4_kspaceid_t client,
        okl4_kspaceid_t share, int manager)
{
    okl4_word_t error;
    okl4_word_t op = L4_SPACE_RESOURCES_WINDOW_GRANT;
    L4_ClistId_t null_clist = {0};

    if (manager) {
        op = L4_SPACE_RESOURCES_WINDOW_MANAGE;
    }

    L4_LoadMR(0, share.raw);
    error = L4_SpaceControl(client, L4_SpaceCtrl_resources,
            null_clist, L4_Nilpage, op, NULL);
    if (error != 1) {
        return _okl4_convert_kernel_errno(L4_ErrorCode());
    }

    return OKL4_OK;
}

void
_okl4_kspace_domain_unshare(okl4_kspaceid_t client, okl4_kspaceid_t share)
{
    okl4_word_t error;
    L4_ClistId_t null_clist = {0};

    L4_LoadMR(0, share.raw);
    error = L4_SpaceControl(client, L4_SpaceCtrl_resources,
            null_clist, L4_Nilpage,
            L4_SPACE_RESOURCES_WINDOW_REVOKE, NULL);
}

int
_okl4_kspace_window_map(okl4_kspaceid_t client, okl4_kspaceid_t share,
        okl4_range_item_t window)
{
    okl4_word_t error;
    L4_Fpage_t fpage;


    fpage = L4_Fpage(window.base, window.size);
    (void)L4_Set_Rights(&fpage, L4_FullyAccessible);
    L4_LoadMR(0, share.raw);
    L4_LoadMR(1, fpage.raw);
    error = L4_MapControl(client, L4_MapCtrl_MapWindow);
    if (error != 1) {
        return _okl4_convert_kernel_errno(L4_ErrorCode());
    }

    return OKL4_OK;
}

int
_okl4_kspace_window_unmap(okl4_kspaceid_t client, okl4_kspaceid_t share,
        okl4_range_item_t window)
{
    okl4_word_t error;
    L4_Fpage_t fpage = L4_Fpage(window.base, window.size);

    (void)L4_Set_Rights(&fpage, L4_NoAccess);
    L4_LoadMR(0, share.raw);
    L4_LoadMR(1, fpage.raw);
    error = L4_MapControl(client, L4_MapCtrl_MapWindow);
    if (error != 1) {
        return _okl4_convert_kernel_errno(L4_ErrorCode());
    }

    return OKL4_OK;
}

/*
 * Share a domain between the two spaces, and add a window.
 */
int
_okl4_kspace_share_domain_add_window(okl4_kspaceid_t client,
        okl4_kspaceid_t owner, int manager, okl4_virtmem_item_t window)
{
    int error;

    /* Share the zone space. */
    error = _okl4_kspace_domain_share(client, owner, manager);
    if (error) {
        return error;
    }

    /* Create a window between the two spaces. */
    error = _okl4_kspace_window_map(client, owner, window);
    if (error) {
        return error;
    }

    return OKL4_OK;
}

#endif

