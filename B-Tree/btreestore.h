#ifndef BTREESTORE_H
#define BTREESTORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <stddef.h>

struct head {
    uint16_t branching;
    uint8_t n_processors;
    uint64_t vertex_count;
    uint64_t key_count;
    struct vertex * root;
};

struct vertex {
    uint16_t num_keys;
    uint16_t num_children;
    struct vertex * parent;
    struct key_value ** key_list;
    struct vertex ** vertex_list;
};

struct key_value {
    uint32_t value;
    uint32_t size;
    uint32_t key[4];
    uint64_t nonce;
    void * data;
};

// ____________________________________________________________________________

struct info {
    uint32_t size; // Size of actual stored data in bytes
    uint32_t key[4]; // Encryption key
    uint64_t nonce; // Nonce data
    void * data; // Pointer to the encrypted stored data
};

struct node {
    uint16_t num_keys; // The number of keys in this node
    uint32_t * keys; // The keys in this node, ordered
};

void * init_store(uint16_t branching, uint8_t n_processors);

void close_store(void * helper);

int btree_insert(uint32_t key, void * plaintext, size_t count, uint32_t encryption_key[4], uint64_t nonce, void * helper);

int btree_retrieve(uint32_t key, struct info * found, void * helper);

int btree_decrypt(uint32_t key, void * output, void * helper);

int btree_delete(uint32_t key, void * helper);

uint64_t btree_export(void * helper, struct node ** list);

void encrypt_tea(uint32_t plain[2], uint32_t cipher[2], uint32_t key[4]);

void decrypt_tea(uint32_t cipher[2], uint32_t plain[2], uint32_t key[4]);

void encrypt_tea_ctr(uint64_t * plain, uint32_t key[4], uint64_t nonce, uint64_t * cipher, uint32_t num_blocks);

void decrypt_tea_ctr(uint64_t * cipher, uint32_t key[4], uint64_t nonce, uint64_t * plain, uint32_t num_blocks);

#endif
