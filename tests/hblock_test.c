//
// Created by prema on 29/4/2024.
//

#include "../src/heep/hblock.c"

int test_get_set_size() {
    HBlock b;
    SET_BLOCK_SIZE(&b, 64);
    size_t size = GET_BLOCK_SIZE(&b);
    int res = size != 64;
    if (res)
        printf("Failed HBlock Size Test! Value expected: %lu Value seen: %lu\n", (size_t)64, size);

    return res;
}

int test_get_set_free() {
    HBlock b;
    block_init(&b, 0, true, NULL, NULL);

    bool res = !GET_BLOCK_FREE(&b) || b.next != NULL || b.prev != NULL;
    if (res) {
        printf("Failed HBlock Free Test! Value expected: %d Value seen: %d\n", true, res);
        return res;
    }

    HBlock a;
    block_init(&a, 0, true, NULL, NULL);

    res = !GET_BLOCK_FREE(&a) || a.next != NULL || a.prev != NULL;
    if (res) {
        printf("Failed HBlock Free Test! Value expected: %d Value seen: %d\n", true, res);
        return res;
    }

    HBlock c;
    block_init(&c, 0, true, &a, &b);

    res = !GET_BLOCK_FREE(&c) || c.next != &a || c.prev != &b;
    if (res) {
        printf("Failed HBlock Free Test! Value expected: %d Value seen: %d\n", true, res);
        return res;
    }

    return res;
}

int test_make_block_free() {
    HBlock a;
    SET_BLOCK_FREE(&a, true);

    HBlock b;
    SET_BLOCK_FREE(&b, true);

    HBlock c;
    block_init(&c, 64, true, &a, &b);
    size_t size = GET_BLOCK_SIZE(&c);
    bool free = GET_BLOCK_FREE(&c);
    int res = (size != 64) || !free || c.next != &a || c.prev != &b;
    if (res) {
        printf("Failed HBlock Make Free Block Test! Size expected: %lu Size seen: %lu Free expected: %d Free seen: %d Next expected: %p Next seen: %p Prev expected: %p Prev seen: %p", (size_t)64, size, true, free, (void*)(&a), c.next, (void*)(&b), c.prev);
        return res;
    }

    return res;
}

int test_make_block_alloc() {
    HBlock c;
    block_init(&c, 64, false, NULL, NULL);
    size_t size = GET_BLOCK_SIZE(&c);
    bool free = GET_BLOCK_FREE(&c);
    int res = (size != 64) || free;
    if (res) {
        printf("Failed HBlock Make Alloc Block Test! Size expected: %lu Size seen: %lu Free expected: %d Free seen: %d\n", (size_t)64, size, false, free);
        return res;
    }

    return res;
}

int test_expand_block_free() {
    HBlock a;
    block_init(&a, 0, true, NULL, NULL);

    HBlock b;
    block_init(&b, 0, true, NULL, NULL);

    HBlock c;
    block_init(&c, 64, true, &a, &b);

    block_expand(&c, 32);
    bool free = GET_BLOCK_FREE(&c);
    size_t size = GET_BLOCK_SIZE(&c);
    int res = size != 64+32 || !free || c.next != &a || c.prev != &b;
    if (res) {
        printf(
                "Failed HBlock Expand Free Block Test! Expected size: %ld Seen size: %ld Expected Free: %d Seen free: %d Expected next: %p Seen next: %p Expected prev: %p Seen prev: %p\n",
                (size_t)64, size, true, free, (void*)&a, c.next, (void*)&b, c.prev);

        return res;
    }

    return res;
}

int test_expand_block_alloc() {
    HBlock c;
    block_init(&c, 64, false, NULL, NULL);

    block_expand(&c, 32);
    bool free = GET_BLOCK_FREE(&c);
    size_t size = GET_BLOCK_SIZE(&c);
    int res = size != 64+32 || free;
    if (res) {
        printf(
                "Failed HBlock Expand Alloc Block Test! Expected size: %ld Seen size: %ld Expected Free: %d Seen free: %d\n",
                (size_t)64, size, true, free);

        return res;
    }

    return res;
}

int test_get_end_addr_block_free() {
    HBlock a;
    block_init(&a, 0, true, NULL, NULL);

    HBlock b;
    block_init(&b, 0, true, NULL, NULL);

    HBlock c;
    block_init(&c, 64, true, &a, &b);

    void* end = GET_BLOCK_END_ADDRESS(&c);
    int res = ((char*)end - (char*)&c != 64);
    if (res) {
        printf("Failed HBlock Get End Address Free Test! Expected address: %p Seen address: %p\n", (void*)((char*)&c+64), end);
        return res;
    }

    return res;
}

int test_get_end_addr_block_alloc() {
    HBlock c;
    block_init(&c, 64, false, NULL, NULL);

    void* end = GET_BLOCK_END_ADDRESS(&c);
    int res = ((char*)end - (char*)&c != 64);
    if (res) {
        printf("Failed HBlock Get End Address Alloc Test! Expected address: %p Seen address: %p\n", (void*)((char*)&c+64), end);
        return res;
    }

    return res;
}

int test_get_start_addr_block_from_data() {
    HBlock c;
    block_init(&c, 64, false, NULL, NULL);

    void* data = (void*)((char*)&c + ALLOC_HEADER_SIZE);
    void* retC = GET_BLOCK_START_ADDRESS_FROM_DATA(data);
    size_t size = GET_BLOCK_SIZE((HBlock *) retC);
    bool free = GET_BLOCK_FREE((HBlock *) retC);
    int res = retC != (void*)&c || size != 64 || free;
    if (res) {
        printf(
                "Failed HBlock Get Start Address Block From Data Test! Expected size: %ld Seen size: %ld Expected Free: %d Seen free: %d Expected address: %p Seen address: %p\n",
                (size_t)64, size, true, free, (void*)&c, retC);
        return res;
    }

    return res;
}

int main(void) {
    if (test_get_set_size())
        return 1;

    if (test_get_set_free())
        return 1;

    if (test_make_block_free())
        return 1;

    if (test_make_block_alloc())
        return 1;

    if (test_expand_block_free())
        return 1;

    if (test_expand_block_alloc())
        return 1;

    if (test_get_end_addr_block_free())
        return 1;

    if (test_get_end_addr_block_alloc())
        return 1;

    if (test_get_start_addr_block_from_data())
        return 1;
}
