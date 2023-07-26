#include "btreestore.h"

// Computes 2 to the power of a positive integer "exponent"
uint64_t power_2(uint8_t exponent) {
    if (exponent == 0) { 
        return 1;
    } // Bit shifting does not work expectedly with 0
    return ((uint64_t)2 << (exponent-1));
}

// A B-tree vertex will store at most <branching> children, and 1 fewer keys
// All vertices will have list lengths plus 1 to temporarily store objects when 
// doing splitting operations
struct vertex * initialise_vertex(uint16_t num_keys, uint16_t num_children, struct vertex * parent, uint16_t branching) {
    struct vertex * node = (struct vertex *)malloc(sizeof(struct vertex));
    node->num_keys = num_keys;
    node->num_children = num_children;
    node->parent = parent;
    node->key_list = (struct key_value **)malloc(sizeof(struct key_value)*(branching-1+1));
    node->vertex_list = (struct vertex **)malloc(sizeof(struct vertex)*(branching+1));
    return node;
}

struct key_value * initialise_key(uint32_t value, uint32_t size, uint32_t encryption_key[4], uint64_t nonce) {
    struct key_value * store = (struct key_value *)malloc(sizeof(struct key_value));
    store->value = value;
    store->size = size;
    memcpy(store->key, encryption_key, 4*sizeof(uint32_t));
    store->nonce = nonce;
    store->data = NULL;
    return store;
}

// ____________________________________________________________________________

// Searches and returns the index of a key inside a list of keys
// [Returns -1 if not found]
int key_index(uint32_t key, struct key_value ** key_list, uint16_t num_keys) {
    for (int i = 0; i < num_keys; i++) {
        if (key_list[i]->value == key) {
            return i;
        }
    }
    return -1;
}

// Searches and returns the index of a vertex inside a list of vertices
int vertex_index(struct vertex * target, struct vertex ** vertex_list, uint16_t num_children) {
    for (int i = 0; i < num_children; i++) {
        if (vertex_list[i] == target) {
            return i;
        } 
    }
    return -1;
}

// Insert a key into a list of keys, maintaining the sorted order
// [Accepts <num_keys>, the current number of keys before insertion]
void insert_key(struct key_value * key, struct key_value ** key_list, uint16_t * num_keys) {    
    // Shift everything RIGHT, startin2g from the end
    int index = * num_keys;
    for ( ; index > 0; index--) { 
        if (key_list[index-1]->value < key->value) {
            break; // Add the key in order
        }
        key_list[index] = key_list[index-1];
    }
    key_list[index] = key;
    * num_keys += 1;
}

// Remove a key from a list of keys, maintaining the sorted order
// [Accepts <num_keys>, the current number of keys before deletion]
void remove_key(struct key_value * key, struct key_value ** key_list, uint16_t * num_keys) {    
    // Shift everything LEFT, starting from the index of removal
    int index = key_index(key->value, key_list, * num_keys);
    for (int i = index; i < * num_keys; i++) { 
        key_list[i] = key_list[i+1];
    }
    * num_keys -= 1;
}

// Using a vertex's <key_list> insert a vertex correspondingly into its <vertex_list>
// position.
void insert_vertex(struct vertex * in_vertex, struct vertex ** vertex_list, uint16_t * num_children) {
    uint32_t in_lower = in_vertex->key_list[0]->value;
    // Shift everything RIGHT, starting from the end
    int index = * num_children;
    for ( ; index > 0; index--) { 
        // First key of the current vertex in consideration
        uint32_t lower = vertex_list[index-1]->key_list[0]->value; 
        if (lower < in_lower) {
            break; // Add the vertex in order of the keys
        }
        vertex_list[index] = vertex_list[index-1];
    }
    vertex_list[index] = in_vertex; 
    * num_children += 1;
}

// Using a vertex's <key_list> remove a vertex and shift all vertices.
void remove_vertex(struct vertex * out_vertex, struct vertex ** vertex_list, uint16_t * num_children) {
    // Shift everything LEFT, starting from the index of removal
    int index = vertex_index(out_vertex, vertex_list, * num_children);
    for (int i = index; i < * num_children; i++) { 
        vertex_list[i] = vertex_list[i+1];
    }
    * num_children -= 1;
}

// Search the key, filling in <location> with the vertex it is in, or the leaf
// vertex if not found. [Returns the index of the key if found, -1 if not found]
int btree_search(uint32_t key, struct head * head, struct vertex ** location) {
    struct vertex * current = head->root;
    while (1) {
        * location = current;
        int ret = key_index(key, current->key_list, current->num_keys);
        if (ret != -1) {
            return ret;
        } else if (current->num_children == 0) {
            break;
        }

        if (key < current->key_list[0]->value) {
            current = current->vertex_list[0];
        } else if (key > current->key_list[current->num_keys-1]->value) {
            current = current->vertex_list[current->num_children-1];
        } else {
            for (int i = 0; i < current->num_keys-1; i++) {
                if (current->key_list[i]->value < key && key < current->key_list[i+1]->value) {
                    current = current->vertex_list[i+1];
                }
            }
        }
    }
    return -1;
}

// ____________________________________________________________________________
// ____________________________________________________________________________

// Initialise the B-tree root. Returns "void * helper"
void * init_store(uint16_t branching, uint8_t n_processors) {
    if (branching < 3) { // Branching of 2 is very degenerate and generally not used
        printf("B-tree branching factor must be greater than or equal to 3\n");
        exit(1);
        return NULL;
    }
    struct vertex * root = initialise_vertex(0, 0, NULL, branching);

    // The head of the data structure stores the tree parameters
    struct head * head = (struct head *)malloc(sizeof(struct head));
    head->branching = branching;
    head->n_processors = n_processors;
    head->vertex_count = 1;
    head->key_count = 0;
    head->root = root;
    return head;
}

// Conduct a iterative BFS traversal of all nodes in the tree to free their data
void close_store(void * helper) {
    struct head * head = (struct head *)helper;
    int length = 1;
    struct vertex ** node_list = (struct vertex **)malloc(sizeof(struct vertex)*head->vertex_count);
    node_list[0] = head->root;
    while (1) {
        if (length == 0) {
            free(head);
            free(node_list);
            return;
        }
        struct vertex * node = node_list[0];
        // Remove the first element and shift all elements left by one index
        for (int i = 0; i < length-1; i++) { 
            node_list[i] = node_list[i+1];
        }
        length -= 1;

        for (int i = 0; i < node->num_children; i++) {
            // Append the next node to the list of nodes
            node_list[length] = node->vertex_list[i];
            length += 1;
        }

        // Conduct free operations on the node itself and its keys
        for (int i = 0; i < node->num_keys; i++) {
            free(node->key_list[i]->data);
            free(node->key_list[i]);
        }
        free(node->key_list);
        free(node->vertex_list);
        free(node);
    }
}

int btree_insert(uint32_t key, void * plaintext, size_t count, uint32_t encryption_key[4], uint64_t nonce, void * helper) {
    if (plaintext == NULL || count == 0 || encryption_key == NULL || helper == NULL) {
        // printf("Invalid parameters\n");
        return -1;
    }
    struct head * head = (struct head *)helper;
    struct vertex * current = NULL;
    if (btree_search(key, head, &current) != -1) {
        // printf("Key already exists in B-tree\n");
        return 1;
    }
    head->key_count += 1;

    // Round plaintext byte length to nearest ceiling multiple of 64 bits (8 bytes)
    // Key stores the rounded plaintext as <data> and the initial length as <size> bytes
    size_t round = ((count + 8 - 1) / 8) * 8;
    uint64_t * filltext = (uint64_t *)malloc(sizeof(uint64_t)*(round/8));
    memset(filltext, '\0', sizeof(uint64_t)*(round/8)); 
    memcpy(filltext, plaintext, count);
    // The rounded plaintext is has '0' bytes padded to the end

    // Initialise and add the key_value struct to the vertex
    struct key_value * store = initialise_key(key, count, encryption_key, nonce);
    store->data = (uint64_t *)malloc(sizeof(uint64_t)*(round/8));
    encrypt_tea_ctr(filltext, encryption_key, nonce, store->data, round/8);
    insert_key(store, current->key_list, &current->num_keys); 
    free(filltext);

    // ------------------------------------------------------------------------

    while (current->num_keys > head->branching - 1) {
        head->vertex_count += 1;

        int median = (current->num_keys-1)/2;
        store = current->key_list[median];

        // Initialise the left sibling vertex
        struct vertex * left = initialise_vertex(median, current->num_children, current->parent, head->branching);
        for (int i = 0; i < median; i++) {
            left->key_list[i] = current->key_list[i];
        }

        // Alter the current vertex to be the right sibling vertex
        for (int i = median+1; i < current->num_keys; i++) {
            current->key_list[i - (median+1)] = current->key_list[i];
        }

        // Relink the children correctly to the siblings (non-leaf vertices)
        // Current number of children is always +1 of keys. 
        if (current->num_children != 0) {
            // Splitting key at index <median> splits median+1 children for the left sibling
            for (int i = 0; i < median + 1; i++) {
                left->vertex_list[i] = current->vertex_list[i]; // parent->child links
                left->vertex_list[i]->parent = left; // child->parent links
            }
            // And splits (num_keys - median) children for the right
            for (int i = 0; i < current->num_keys - median; i++) {
                current->vertex_list[i] = current->vertex_list[median+1+i]; // parent->child links
                // Note that child->parent links remain the same for current 
            }
            left->num_children = median + 1;
            current->num_children = current->num_keys - median;
        }

        current->num_keys = current->num_keys - median - 1;

        // ____________________________________________________________________

        if (current->parent == NULL) {
            head->vertex_count += 1;

            // Initialise new root vertex and alter sibling links
            struct vertex * new_root = initialise_vertex(0, 0, NULL, head->branching);
            left->parent = new_root;
            current->parent = new_root;
            head->root = new_root;
            insert_key(store, new_root->key_list, &new_root->num_keys);
            insert_vertex(left, new_root->vertex_list, &new_root->num_children);
            insert_vertex(current, new_root->vertex_list, &new_root->num_children); 
            return 0;
        }
        insert_key(store, current->parent->key_list, &current->parent->num_keys); 
        insert_vertex(left, current->parent->vertex_list, &current->parent->num_children);
        current = current->parent;
    }

    return 0;
}

int btree_retrieve(uint32_t key, struct info * found, void * helper) {
    struct head * head = (struct head *)helper;;
    struct vertex * current = NULL;
    int index = btree_search(key, head, &current);
    if (index == -1) {
        // printf("Key does not exist in B-tree\n");
        return 1;
    }

    found->size = current->key_list[index]->size;
    memcpy(found->key, current->key_list[index]->key, 4*sizeof(uint32_t));
    found->nonce = current->key_list[index]->nonce;
    found->data = current->key_list[index]->data;
    return 0;
}

int btree_decrypt(uint32_t key, void * output, void * helper) {
    struct head * head = (struct head *)helper;;
    struct vertex * current = NULL;
    int index = btree_search(key, head, &current);
    if (index == -1) {
        // printf("Key does not exist in B-tree\n");
        return 1;
    }

    // Round plaintext length to nearest ceiling multiple of 64 bits (8 bytes)
    size_t round = ((current->key_list[index]->size + 8 - 1) / 8) * 8;

    uint64_t * outtext = (uint64_t *)malloc(sizeof(uint64_t)*(round/8));
    decrypt_tea_ctr(current->key_list[index]->data, current->key_list[index]->key, 
        current->key_list[index]->nonce, outtext, round/8);
    
    memcpy(output, outtext, current->key_list[index]->size);
    free(outtext);
    
    return 0;
}

// ----------------------------------------------------------------------------
// Modular implementation of steps 4 and 5 to shift keys and children of the left
// or right sibling
void deletion_helper(struct vertex * current, struct vertex * sibling, int leaf_index, int sibling_key_index, int sibling_vertex_index) {
    // insert K(left/right) into the leaf node 
    insert_key(current->parent->key_list[leaf_index], current->key_list, &current->num_keys); 
    // The sibling's key is removed and replaces the index of K(left/right)
    current->parent->key_list[leaf_index] = sibling->key_list[sibling_key_index];
    remove_key(sibling->key_list[sibling_key_index], sibling->key_list, &sibling->num_keys);
    
    if (current->num_children != 0) { // INTERNAL NODE
        // Reparent the inserted vertex before insertion
        sibling->vertex_list[sibling_vertex_index]->parent = current;
        // Move the specified child (+ subtree) in sibling to current
        insert_vertex(sibling->vertex_list[sibling_vertex_index], current->vertex_list, &current->num_children);
        remove_vertex(sibling->vertex_list[sibling_vertex_index], sibling->vertex_list, &sibling->num_children);
    }
}

// Modular implementation of step 6 to merge a sibling with the current node
void merge_helper(struct vertex * current, struct vertex * sibling, int leaf_index) {
    for (int i = 0; i < sibling->num_keys; i++) {
        insert_key(sibling->key_list[i], current->key_list, &current->num_keys);
    }
    for (int i = 0; i < sibling->num_children; i++) {
        // Reparent the inserted vertex before insertion
        sibling->vertex_list[i]->parent = current;
        insert_vertex(sibling->vertex_list[i], current->vertex_list, &current->num_children);
    }
    // Remove the sibling node from the parent
    remove_vertex(sibling, current->parent->vertex_list, &current->parent->num_children);
    free(sibling->key_list);
    free(sibling->vertex_list);
    free(sibling);
    // Remove K(left) from parent and into leaf
    insert_key(current->parent->key_list[leaf_index], current->key_list, &current->num_keys);
    remove_key(current->parent->key_list[leaf_index], current->parent->key_list, &current->parent->num_keys);
}

// ----------------------------------------------------------------------------
int btree_delete(uint32_t key, void * helper) {
    if (helper == NULL) {
        // printf("Invalid parameters\n");
        return -1;
    }
    struct head * head = (struct head *)helper;
    struct vertex * current = NULL;
    int index = btree_search(key, head, &current);
    if (index == -1) {
        // printf("Key does not exist in B-tree\n");
        return 1;
    }
    head->key_count -= 1;

    // Conduct removal of key. If internal node, swap the key with the largest
    // key of the subtree rooted at the left of the two nodes separated by the key
    // and move to the leaf node
    if (current->num_children != 0) {
        struct vertex * initial = current;
        // Left of the two children the key separates
        current = current->vertex_list[index];
        while (current->num_children != 0) {
            current = current->vertex_list[current->num_children-1];
        } // Find the very right leaf node (largest key to the very right)
        struct key_value * temp = current->key_list[current->num_keys-1];
        current->key_list[current->num_keys-1] = initial->key_list[index];
        initial->key_list[index] = temp;
        index = current->num_keys-1;
    }

    // Delete and free the key from the leaf node [CURRENT = LEAF]
    struct key_value * key_removed = current->key_list[index];
    remove_key(key_removed, current->key_list, &current->num_keys);
    free(key_removed->data);
    free(key_removed);
    
    // ------------------------------------------------------------------------

    if (current->parent == NULL && current->num_children == 0) {
        return 0; // If only root exists, all keys from it can be removed
    }
    // Number of keys in leaf must satisfy [(ceil(b/2))-1 <= k], or [1 <= k] for root
    int key_minimum = ((head->branching + 2 - 1)/2)-1;
    while (current->num_keys < key_minimum) {
        index = vertex_index(current, current->parent->vertex_list, current->parent->num_children);
        
        // NODE MERGE PROCESS [1] ---------------------------------------------
        // Conducts step 4 or 5 depending on whether current is a leaf or internal
        if (index != 0) { // Left Sibling
            struct vertex * left_sibling = current->parent->vertex_list[index-1];
            if (left_sibling->num_keys > key_minimum) {
                deletion_helper(current, left_sibling, index-1, left_sibling->num_keys-1, left_sibling->num_children-1);
                return 0;
            }
        } 
        if (index != current->parent->num_children-1) { // Right Sibling
            struct vertex * right_sibling = current->parent->vertex_list[index+1];
            if (right_sibling->num_keys > key_minimum) {
                deletion_helper(current, right_sibling, index, 0, 0);
                return 0;
            }   
        }

        // MERGE PROCESS [2] --------------------------------------------------
        // Conducts step 6
        if (index != 0) { // Left Sibling
            struct vertex * left_sibling = current->parent->vertex_list[index-1];
            merge_helper(current, left_sibling, index-1);
        } else { // Right Sibling
            struct vertex * right_sibling = current->parent->vertex_list[index+1];
            merge_helper(current, right_sibling, index);
        }

        // ____________________________________________________________________
        
        head->vertex_count -= 1;

        // Next iteration current is root
        if (current->parent->parent == NULL) {
            // Less than minimum keys allowed, delete it and relink root
            if (current->parent->num_keys < 1) {
                free(current->parent->key_list);
                free(current->parent->vertex_list);
                free(current->parent);

                current->parent = NULL; // Current node becomes new root
                head->root = current;
                head->vertex_count -= 1;
                return 0;
            }
            break;
        }
        current = current->parent;
    }

    return 0;
}

// Conduct a iterative DFS pre-order traversal to export all vertices
uint64_t btree_export(void * helper, struct node ** list) {
    struct head * head = (struct head *)helper;
    if (head->vertex_count == 0) {
        * list = NULL;
        return 0;
    }

    int length = 1;
    struct vertex ** node_list = (struct vertex **)malloc(sizeof(struct vertex)*head->vertex_count);
    node_list[0] = head->root;

    // Setup the export array
    struct node * export = (struct node *)malloc(sizeof(struct node)*head->vertex_count);
    int count = 0;

    while (1) {
        if (length == 0) {
            free(node_list);
            * list = export;
            return head->vertex_count;
        }
        struct vertex * node = node_list[length-1];
        length -= 1; // Mark the last item as removed

        // Reverse traversal of children for iterative DFS
        for (int i = node->num_children-1; i >= 0; i--) {
            // Append the next node to the list of nodes
            node_list[length] = node->vertex_list[i];
            length += 1;
        }

        // Export the key data for each node
        struct node n_export;
        n_export.num_keys = node->num_keys;
        uint32_t * key_array = (uint32_t *)malloc(sizeof(uint32_t)*node->num_keys);
        for (int i = 0; i < node->num_keys; i++) {
            key_array[i] = node->key_list[i]->value;
        }
        n_export.keys = key_array;

        export[count] = n_export;
        count += 1;
    }
}

// ____________________________________________________________________________

// Standard TEA encryption
void encrypt_tea(uint32_t plain[2], uint32_t cipher[2], uint32_t key[4]) {
    uint32_t sum = 0; 
    uint32_t delta = 0x9E3779B9; // MAGIC hash constant
    uint64_t modulus = power_2(32);
    cipher[0] = plain[0];
    cipher[1] = plain[1];
    for (int i = 0; i < 1024; i++) {
        sum = (sum + delta) % modulus;
        uint32_t tmp1 = ((cipher[1] << 4) + key[0]) % modulus;
        uint32_t tmp2 = (cipher[1] + sum) % modulus;
        uint32_t tmp3 = ((cipher[1] >> 5) + key[1]) % modulus;
        cipher[0] = (cipher[0] + (tmp1 ^ tmp2 ^ tmp3)) % modulus;
        uint32_t tmp4 = ((cipher[0] << 4) + key[2]) % modulus;
        uint32_t tmp5 = (cipher[0] + sum) % modulus;
        uint32_t tmp6 = ((cipher[0] >> 5) + key[3]) % modulus;
        cipher[1] = (cipher[1] + (tmp4 ^ tmp5 ^ tmp6)) % modulus;
    }
    return;
}

// Standard TEA decryption
void decrypt_tea(uint32_t cipher[2], uint32_t plain[2], uint32_t key[4]) {
    uint32_t sum = 0xDDE6E400; // MAGIC hash constant
    uint32_t delta = 0x9E3779B9; // MAGIC hash constant
    uint64_t modulus = power_2(32);
    for (int i = 0; i < 1024; i++) {
        uint32_t tmp4 = ((cipher[0] << 4) + key[2]) % modulus;
        uint32_t tmp5 = (cipher[0] + sum) % modulus;
        uint32_t tmp6 = ((cipher[0] >> 5) + key[3]) % modulus;
        cipher[1] = (cipher[1] - (tmp4 ^ tmp5 ^ tmp6)) % modulus;
        uint32_t tmp1 = ((cipher[1] << 4) + key[0]) % modulus;
        uint32_t tmp2 = (cipher[1] + sum) % modulus;
        uint32_t tmp3 = ((cipher[1] >> 5) + key[1]) % modulus;
        cipher[0] = (cipher[0] - (tmp1 ^ tmp2 ^ tmp3)) % modulus;
        sum = (sum - delta) % modulus;
    }
    plain[0] = cipher[0];
    plain[1] = cipher[1];
    return;
}

// Standard TEA-CTR encryption
void encrypt_tea_ctr(uint64_t * plain, uint32_t key[4], uint64_t nonce, uint64_t * cipher, uint32_t num_blocks) {
    for (int i = 0; i < num_blocks; i++) {
        uint32_t tmp1[2];
        uint64_t * tmp3 = (uint64_t *)tmp1;
        tmp3[0] = i ^ nonce; // Store uint64_t in tmp1[2] space
        uint32_t tmp2[2];
        encrypt_tea(tmp1, tmp2, key);
        uint64_t * tmp4 = (uint64_t *)tmp2;
        cipher[i] = plain[i] ^ tmp4[0];
    }
    return;
}

// Standard TEA-CTR decryption
void decrypt_tea_ctr(uint64_t * cipher, uint32_t key[4], uint64_t nonce, uint64_t * plain, uint32_t num_blocks) {
    for (int i = 0; i < num_blocks; i++) {
        uint32_t tmp1[2];
        uint64_t * tmp3 = (uint64_t *)tmp1;
        tmp3[0] = i ^ nonce; // Store uint64_t in tmp1[2] space
        uint32_t tmp2[2];
        encrypt_tea(tmp1, tmp2, key);
        uint64_t * tmp4 = (uint64_t *)tmp2;
        plain[i] = cipher[i] ^ tmp4[0];
    }
    return;
}

// ____________________________________________________________________________