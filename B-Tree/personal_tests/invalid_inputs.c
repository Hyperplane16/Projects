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
    void * helper = init_store(6, 0); // Branching of 6
    uint32_t encryption_key[4] = {100, 200, 300, 400};
    uint32_t plaintext[2] = {10, 100};
    uint64_t nonce = 1000;

    // Testing a number of insertions and deletions, then conducting invalid inputs
    if (btree_insert(100, plaintext, 8, encryption_key, nonce, NULL) != -1) {
        printf("Failed\n"); // NULL helper
    }
    btree_insert(10, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(100, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(50, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(13, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(1, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(2, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(7, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(9, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(3, plaintext, 8, encryption_key, nonce, helper);
    btree_insert(5, plaintext, 8, encryption_key, nonce, helper);

    export_print(helper);

    // ------------------------------------------------------------------------

    if (btree_insert(100, plaintext, 8, encryption_key, nonce, helper) != 1) {
        printf("Failed\n"); // Key already exists
    }
    if (btree_insert(1000, plaintext, 0, encryption_key, nonce, helper) != -1) {
        printf("Failed\n"); // Count of 0
    }

    btree_delete(5, helper);
    btree_delete(9, helper);
    btree_delete(7, helper);
    btree_delete(13, helper);

    // Retest that no changes have been made
    printf("\n");
    export_print(helper);

    if (btree_delete(13, helper) != 1) {
        printf("Failed\n");
    }
    uint64_t decrypt_test[2] = {0};
    if (btree_decrypt(5, &decrypt_test, helper) != 1) {
        printf("Failed\n");
    }
    struct info retreive_test;
    if (btree_retrieve(7, &retreive_test, helper) != 1) {
        printf("Failed\n");
    }

    // Retest that no changes have been made
    printf("\n");
    export_print(helper);
    
    close_store(helper);
}