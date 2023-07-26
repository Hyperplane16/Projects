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

    void * helper = init_store(4, 0);

    // Large number of insertions and deletions
    btree_insert(5, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(6, plaintext, 5, encryption_key, nonce, helper);
    btree_insert(2, plaintext, 2, encryption_key, nonce, helper);
    btree_insert(4, plaintext, 2, encryption_key, 9, helper);
    btree_insert(7, plaintext, 2, encryption_key, 9, helper);
    btree_insert(8, plaintext, 2, encryption_key, 9, helper);

    export_print(helper); // Output -------------------------------------------

    btree_insert(10, plaintext, 2, encryption_key, 9, helper);
    btree_insert(11, plaintext, 2, encryption_key, 9, helper);
    btree_insert(3, plaintext, 2, encryption_key, 9, helper);
    btree_insert(9, plaintext, 2, encryption_key, 9, helper);

    btree_delete(8, helper);
    btree_delete(3, helper);
    btree_delete(7, helper);
    btree_delete(6, helper);

    btree_insert(15, plaintext, 2, encryption_key, 9, helper);
    btree_insert(16, plaintext, 2, encryption_key, 9, helper);
    btree_insert(20, plaintext, 2, encryption_key, 9, helper);
    btree_insert(40, plaintext, 2, encryption_key, 9, helper);
    btree_insert(80, plaintext, 2, encryption_key, 9, helper);

    printf("\n");
    export_print(helper); // Output -------------------------------------------

    btree_delete(11, helper);
    btree_delete(10, helper);
    btree_insert(3, plaintext, 2, encryption_key, 9, helper);
    btree_delete(5, helper);
    btree_delete(15, helper);

    btree_insert(17, plaintext, 2, encryption_key, 9, helper);    
    btree_insert(18, plaintext, 2, encryption_key, 9, helper);  
    btree_insert(25, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(26, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(50, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(100, plaintext, 2, encryption_key, 9, helper);
    btree_insert(51, plaintext, 2, encryption_key, 9, helper);
    btree_insert(200, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(101, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(53, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(52, plaintext, 2, encryption_key, 9, helper);
    btree_insert(99, plaintext, 2, encryption_key, 9, helper);
    btree_insert(54, plaintext, 2, encryption_key, 9, helper);
    btree_insert(150, plaintext, 2, encryption_key, 9, helper);
    btree_insert(300, plaintext, 2, encryption_key, 9, helper);
    btree_insert(301, plaintext, 2, encryption_key, 9, helper);
    btree_insert(302, plaintext, 2, encryption_key, 9, helper);
    btree_insert(303, plaintext, 2, encryption_key, 9, helper);
    btree_insert(304, plaintext, 2, encryption_key, 9, helper);
    btree_insert(500, plaintext, 2, encryption_key, 9, helper); 
    btree_insert(205, plaintext, 2, encryption_key, 9, helper);
    btree_insert(206, plaintext, 2, encryption_key, 9, helper);
    btree_insert(305, plaintext, 2, encryption_key, 9, helper);
    btree_insert(501, plaintext, 2, encryption_key, 9, helper);
    btree_insert(1000, plaintext, 2, encryption_key, 9, helper);

    printf("\n");
    export_print(helper); // Output -------------------------------------------

    btree_delete(301, helper);
    btree_delete(20, helper);
    btree_delete(80, helper);
    btree_delete(150, helper);
    btree_delete(303, helper);
    btree_delete(501, helper);
    btree_delete(18, helper);
    btree_delete(17, helper);
    btree_delete(54, helper);
    btree_delete(16, helper);
    btree_delete(53, helper);
    btree_delete(52, helper);
    btree_delete(9, helper);
    btree_delete(3, helper);

    printf("\n");
    export_print(helper); // Output -------------------------------------------

    close_store(helper);
}
