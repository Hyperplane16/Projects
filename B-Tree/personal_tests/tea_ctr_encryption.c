#include "../btreestore.h"

int main() {
    uint32_t encryption_key[4] = {100, 200, 300, 400};
    uint64_t plaintext[2] = {10, 100};
    uint64_t cipher[2];
    uint64_t nonce = 1001;
    encrypt_tea_ctr(plaintext, encryption_key, nonce, cipher, 2);
    uint64_t decode[2];

    // Decrypting with the same encryption key and nonce results in the same plaintext
    decrypt_tea_ctr(cipher, encryption_key, nonce, decode, 2);
    if (memcmp(plaintext, decode, 2*sizeof(uint64_t)) != 0) {
        printf("Failed\n");
    }

    // Decrypting with a different nonce should fail
    decrypt_tea_ctr(cipher, encryption_key, 1000, decode, 2);
    if (memcmp(plaintext, decode, 2*sizeof(uint64_t)) == 0) {
        printf("Failed\n");
    }

    // Decrypting with a different key does not return the plaintext
    uint32_t bad_key[4] = {100, 200, 300, 401};
    decrypt_tea_ctr(cipher, bad_key, nonce, decode, 2);
    if (memcmp(plaintext, decode, 2*sizeof(uint64_t)) == 0) {
        printf("Failed\n");
    }

    // ------------------------------------------------------------------------
    // Variable Length Data Testing
    // ------------------------------------------------------------------------

    uint64_t v_plaintext[5] = {0};
    strcpy((char *)v_plaintext, "Hello");    
    uint64_t v_cipher[5] = {0};
    uint64_t v_decode[5] = {0};
    encrypt_tea_ctr(v_plaintext, encryption_key, nonce, v_cipher, 5);

    decrypt_tea_ctr(v_cipher, encryption_key, nonce, v_decode, 5);
    if (memcmp(v_plaintext, v_decode, 5*sizeof(uint64_t)) != 0) {
        printf("Failed\n");
    }
}
