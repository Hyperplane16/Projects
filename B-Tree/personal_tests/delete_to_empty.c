#include "../btreestore.h"

int export_print(void * helper) {
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
    return num;
}

int main() {
    uint32_t encryption_key[4] = {100, 200, 300, 400};
    uint32_t plaintext[2] = {10, 100};
    uint32_t cipher[2];
    uint64_t nonce = 1000;

    void * helper = init_store(4, 0);

    // Add some nodes, then delete all of them
    btree_insert(3, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(10, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(7, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(5, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(1, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(2, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(8, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(11, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(13, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(12, plaintext, 5, encryption_key, nonce, helper);

    export_print(helper);

    // Deletion (every second from the end, then from the start)
    btree_delete(12, helper);
    btree_delete(11, helper);
    btree_delete(2, helper);
    btree_delete(5, helper);
    btree_delete(10, helper);
    btree_delete(3, helper);
    btree_delete(7, helper);
    btree_delete(1, helper);
    btree_delete(8, helper);

    printf("\n");
    export_print(helper); // Only 13 left

    btree_delete(13, helper);
    printf("\n");
    if (export_print(helper) != 1) {
        printf("Failed\n");
    } // Should be two newlines

    close_store(helper);
}
