#include "../btreestore.h"

void export_print(void * helper) {
    struct node * node_list = NULL;
    int num = btree_export(helper, &node_list);
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < node_list[i].num_keys; j++) {
            printf("%d ", node_list[i].keys[j]);
        }
        free(node_list[i].keys);
        printf("\n");
    }
    free(node_list);
}

int main() {
    uint32_t encryption_key[4] = {100, 200, 300, 400};
    uint32_t plaintext[2] = {10, 100};
    uint64_t nonce = 1000;

    uint32_t key1[4] = {0, 0, 0, 1};
    uint64_t plain1[2] = {10, 11};
    uint64_t nonce1 = 0;
    uint64_t cipher1[2];
    encrypt_tea_ctr(plain1, key1, nonce1, cipher1, 2);

    uint32_t key2[4] = {1000, 10000, 100000, 1000000};
    uint64_t plain2[2] = {99999, 99999};
    uint64_t nonce2 = 123456789;
    uint64_t cipher2[2];
    encrypt_tea_ctr(plain2, key2, nonce2, cipher2, 2);


    void * helper = init_store(4, 0); // Branching of 4
    // Testing a number of insertions and deletions, then accessing vertices
    btree_insert(5, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(10, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(1, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(2, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(3, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(9, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(11, plain1, 16, key1, nonce1, helper);
    btree_insert(15, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(14, plain2, 16, key2, nonce2, helper);
    btree_insert(13, plaintext, 8, encryption_key, nonce, helper);

    btree_delete(5, helper);

    // First retreive
    struct info retreive_one;
    btree_retrieve(11, &retreive_one, helper);
    if (retreive_one.size != 16) {
        printf("Failed 1\n");
    } else if (memcmp(key1, retreive_one.key, 2*sizeof(uint32_t)) != 0) {
        printf("Failed 2\n");
    } else if (retreive_one.nonce != nonce1) {
        printf("Failed 3\n");
    } else if (memcmp(retreive_one.data, cipher1, 2*sizeof(uint64_t)) != 0) {
        printf("Failed 4\n");
    }

    // ------------------------------------------------------------------------

    btree_delete(10, helper);
    btree_delete(3, helper);
    btree_delete(15, helper);
    btree_insert(20, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(21, plaintext, 8, encryption_key, nonce, helper);

    // Second retreive
    struct info retreive_two;
    btree_retrieve(14, &retreive_two, helper);
    if (retreive_two.size != 16) {
        printf("Failed 1\n");
    } else if (memcmp(retreive_two.key, key2, 2*sizeof(uint32_t)) != 0) {
        printf("Failed 2\n");
    } else if (retreive_two.nonce != nonce2) {
        printf("Failed 3\n");
    } else if (memcmp(retreive_two.data, cipher2, 2*sizeof(uint64_t)) != 0) {
        printf("Failed 4\n");
    }

    close_store(helper);
}