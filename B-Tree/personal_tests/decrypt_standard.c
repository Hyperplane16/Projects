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

    uint32_t key2[4] = {1000, 10000, 100000, 1000000};
    uint64_t plain2[2] = {99999, 99999};
    uint64_t nonce2 = 123456789;

    void * helper = init_store(5, 0); // Branching of 5
    // Testing a number of insertions and deletions, then accessing vertices
    btree_insert(100, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(500, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(5, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(200, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(201, plain1, 16, key1, nonce1, helper);
    btree_insert(2, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(501, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(499, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(199, plaintext, 8, encryption_key, nonce, helper);

    btree_delete(5, helper);

    // First decrypt
    uint64_t decrypt_one[2] = {0};
    btree_decrypt(201, &decrypt_one, helper);
    if (memcmp(decrypt_one, plain1, 2*sizeof(uint64_t)) != 0) {
        printf("Failed 1\n");
    }

    // ------------------------------------------------------------------------

    btree_insert(102, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(105, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(205, plain2, 16, key2, nonce2, helper);
    btree_insert(1, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(5, plaintext, 8, encryption_key, nonce, helper);

    btree_delete(200, helper);
    btree_delete(201, helper);
    btree_delete(2, helper);

    // Second decrypt
    uint64_t decrypt_two[2] = {0};
    btree_decrypt(205, &decrypt_two, helper);
    if (memcmp(decrypt_two, plain2, 2*sizeof(uint64_t)) != 0) {
        printf("Failed 2\n");
    }

    close_store(helper);
}