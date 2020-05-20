#include <stdio.h>
#include "os.h"
#include <inttypes.h>

/*
 * This function returns a pointer to the PTE of the vpn
 * if the vpn is unmapped, returns NO_MAPPING,
 * otherwise, returns a pointer to the PTE.
 */
uint64_t* get_pte_pointer(uint64_t pt, uint64_t vpn){
    int i;
    uint64_t entry;
    uint64_t *curr_pointer = (uint64_t*)phys_to_virt(pt << 12); /* curr_pointer contains the virtual address of pt */
    for(i = 4 ; i >= 0; i--){
        entry = ((vpn & (0x1ff << (9*i))) >> (9*i)) & 0x1ff; /* entry contains the entry number for curr_pointer */
        if(curr_pointer[entry] & 1 == 0){
            return NULL;
        }
        else{
            if(i != 0){
                curr_pointer = (uint64_t*)phys_to_virt(curr_pointer[entry] & (~0xfff)); /* advance curr_pointer to the next level (that exists if we have arrived here) */
            }
        }
    }
    return &(curr_pointer[entry]);
}

/*
 * This function changes the valid bit to be 0.
 */
void destroy_vpn_mapping(uint64_t pt, uint64_t vpn){
    uint64_t *pte = get_pte_pointer(pt, vpn);
    if (pte != NULL){ /* asserts that the vpn is mapped */
        if (((*pte) & 1) == 1){ /* (offset % 2 == 0) -> the number is even and the least significant bit is 0 ->the page is already invalid */
            *pte = (*pte) & (~1) ;     /* changes the valid bit to 0 */
        }
    }
    return;
}

/*
 * This function sets for given vpn the value of ppn in the pte,
 * and setting the valid bit to be 1.
 * ppn can not be NO_MAPPING
 */
void update_vpn_mapping(uint64_t pt, uint64_t vpn, uint64_t ppn){
    int i;
    uint64_t entry;
    uint64_t *curr_pointer = (uint64_t*)phys_to_virt(pt << 12); /* curr_pointer contains the virtual address of pt */
    for(i = 4 ; i >= 0; i--){
        entry = ((vpn & (0x1ff << (9*i))) >> (9*i)) & 0x1ff; /* entry contains the entry number for curr_pointer */
        if (i != 0){
            if ((curr_pointer[entry] & 1) == 0){ /* there is no valid page */
                curr_pointer[entry] = (alloc_page_frame() << 12) | 1; /* create a new one and make it valid */
            }
            curr_pointer = (uint64_t*)phys_to_virt(curr_pointer[entry]&(~0xfff));
        } /* i = 0 */
        else {
              curr_pointer[entry] = (ppn << 12) | 1; /* set the ppn and make it valid */
        }
    }
    return;
}


/*
 * This function is used to create/destroy virtual memory mappings in a page table.
 * Method arguments:
 * pt: The physical page number of the page table root (this is the physical page that the page table base register in the CPU state will point to).
 *     You can assume that pt has been previously returned by alloc_page_frame().
 * vpn: The virtual page number the caller wishes to map/unmap.
 * ppn: Can be one of two cases:
 *      If ppn is equal to a special NO_MAPPING value (defined in os.h), then vpn’s mapping (if it exists) should be destroyed.
 *      Otherwise, ppn specifies the physical page number that vpn should be mapped to.
 */
void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn) {
    if (ppn == NO_MAPPING){
        destroy_vpn_mapping(pt, vpn);
    }
    else {
        update_vpn_mapping(pt, vpn, ppn);
    }
}

/*
 * This function is used to query the mapping of a virtual page number in a page table.
 * This function returns the physical page number that vpn is mapped to, or NO_MAPPING if no mapping exists.
 * The meaning of the pt argument is the same as with page table update().
 */
uint64_t page_table_query(uint64_t pt, uint64_t vpn){
    uint64_t *pte = get_pte_pointer(pt, vpn);
    if(pte == NULL){
        return NO_MAPPING;
    }
    if(((*pte) & 1) == 0){
        return NO_MAPPING;
    }
    return *pte >> 12;
}