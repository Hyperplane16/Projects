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
    uint32_t cipher[2];
    uint64_t nonce = 1000;

    void * helper = init_store(3, 0);

    // Same as deletion example
    btree_insert(3, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(7, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(13, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(2, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(5, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(11, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(17, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(19, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(20, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(21, plaintext, 5, encryption_key, nonce, helper);

    btree_delete(2, helper);
    export_print(helper);

    // Additional deletions
    printf("\n");
    btree_delete(13, helper);
    export_print(helper);

    printf("\n");
    btree_delete(19, helper);
    export_print(helper);

    printf("\n");
    btree_delete(5, helper);
    export_print(helper);

    close_store(helper);
}
