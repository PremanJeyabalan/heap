//
// Created by prema on 29/4/2024.
//

#include "../src/heep/hblock.c"

int test_get_set_size() {
    HeaderBlock b;
    set_size(&b, 64);
    size_t size = get_size(&b);
    int res = size != 64;
    if (res)
        printf("Failed HeaderBlock Size Test! Value expected: %lu Value seen: %lu\n", (size_t)64, size);

    return res;
}

int test_get_set_free() {
    HeaderBlock b;
    set_free(&b, NULL, NULL);
    bool res = !get_free(&b) || b.next != NULL || b.prev != NULL;
    if (res) {
        printf("Failed HeaderBlock Free Test! Value expected: %d Value seen: %d\n", true, res);
        return res;
    }

    HeaderBlock a;
    set_free(&a, NULL, NULL);
    res = !get_free(&a) || a.next != NULL || a.prev != NULL;
    if (res) {
        printf("Failed HeaderBlock Free Test! Value expected: %d Value seen: %d\n", true, res);
        return res;
    }

    HeaderBlock c;
    set_free(&c, &a, &b);
    res = !get_free(&c) || c.next != &a || c.prev != &b;
    if (res) {
        printf("Failed HeaderBlock Free Test! Value expected: %d Value seen: %d\n", true, res);
        return res;
    }

    return res;
}

int test_make_block_free() {
    HeaderBlock a;
    set_free(&a, NULL, NULL);

    HeaderBlock b;
    set_free(&b, NULL, NULL);

    HeaderBlock c;
    make_block(&c, 64, true, &a, &b);
    size_t size = get_size(&c);
    bool free = get_free(&c);
    int res = (size != 64) || !free || c.next != &a || c.prev != &b;
    if (res) {
        printf("Failed HeaderBlock Make Free Block Test! Size expected: %lu Size seen: %lu Free expected: %d Free seen: %d Next expected: %p Next seen: %p Prev expected: %p Prev seen: %p", (size_t)64, size, true, free, (void*)(&a), c.next, (void*)(&b), c.prev);
        return res;
    }

    return res;
}

int test_make_block_alloc() {
    HeaderBlock c;
    make_block(&c, 64, false, NULL, NULL);
    size_t size = get_size(&c);
    bool free = get_free(&c);
    int res = (size != 64) || free;
    if (res) {
        printf("Failed HeaderBlock Make Alloc Block Test! Size expected: %lu Size seen: %lu Free expected: %d Free seen: %d\n", (size_t)64, size, false, free);
        return res;
    }

    return res;
}

int test_expand_block_free() {
    HeaderBlock a;
    set_free(&a, NULL, NULL);

    HeaderBlock b;
    set_free(&b, NULL, NULL);

    HeaderBlock c;
    make_block(&c, 64, true, &a, &b);

    expand_block(&c, 32);
    bool free = get_free(&c);
    size_t size = get_size(&c);
    int res = size != 64+32 || !free || c.next != &a || c.prev != &b;
    if (res) {
        printf(
                "Failed HeaderBlock Expand Free Block Test! Expected size: %ld Seen size: %ld Expected Free: %d Seen free: %d Expected next: %p Seen next: %p Expected prev: %p Seen prev: %p\n",
                (size_t)64, size, true, free, (void*)&a, c.next, (void*)&b, c.prev);

        return res;
    }

    return res;
}

int test_expand_block_alloc() {
    HeaderBlock c;
    make_block(&c, 64, false, NULL, NULL);

    expand_block(&c, 32);
    bool free = get_free(&c);
    size_t size = get_size(&c);
    int res = size != 64+32 || free;
    if (res) {
        printf(
                "Failed HeaderBlock Expand Alloc Block Test! Expected size: %ld Seen size: %ld Expected Free: %d Seen free: %d\n",
                (size_t)64, size, true, free);

        return res;
    }

    return res;
}

int test_get_end_addr_block_free() {
    HeaderBlock a;
    set_free(&a, NULL, NULL);

    HeaderBlock b;
    set_free(&b, NULL, NULL);

    HeaderBlock c;
    make_block(&c, 64, true, &a, &b);

    void* end = get_end_addr_block(&c);
    int res = ((char*)end - (char*)&c != 64);
    if (res) {
        printf("Failed HeaderBlock Get End Address Free Test! Expected address: %p Seen address: %p\n", (void*)((char*)&c+64), end);
        return res;
    }

    return res;
}

int test_get_end_addr_block_alloc() {
    HeaderBlock c;
    make_block(&c, 64, false, NULL, NULL);

    void* end = get_end_addr_block(&c);
    int res = ((char*)end - (char*)&c != 64);
    if (res) {
        printf("Failed HeaderBlock Get End Address Alloc Test! Expected address: %p Seen address: %p\n", (void*)((char*)&c+64), end);
        return res;
    }

    return res;
}

int test_get_start_addr_block_from_data() {
    HeaderBlock c;
    make_block(&c, 64, false, NULL, NULL);

    void* data = (void*)((char*)&c + ALLOC_HEADER_SIZE);
    void* retC = get_start_addr_block_from_data(data);
    size_t size = get_size((HeaderBlock*) retC);
    bool free = get_free((HeaderBlock*) retC);
    int res = retC != (void*)&c || size != 64 || free;
    if (res) {
        printf(
                "Failed HeaderBlock Get Start Address Block From Data Test! Expected size: %ld Seen size: %ld Expected Free: %d Seen free: %d Expected address: %p Seen address: %p\n",
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
