#include "../btreestore.h"

int main() {
    uint32_t encryption_key[4] = {100, 200, 300, 400};
    uint32_t plaintext[2] = {10, 100};
    uint32_t cipher[2];
    encrypt_tea(plaintext, cipher, encryption_key);
    uint32_t decode[2];

    // Decrypting with the same encryption key results in the same plaintext
    decrypt_tea(cipher, decode, encryption_key);
    if (memcmp(plaintext, decode, 2*sizeof(uint32_t)) != 0) {
        printf("Failed\n");
    }

    // Decrypting with a different key does not return the plaintext
    uint32_t bad_key[4] = {100, 200, 300, 401};
    decrypt_tea(cipher, decode, bad_key);
    if (memcmp(plaintext, decode, 2*sizeof(uint32_t)) == 0) {
        printf("Failed\n");
    }
}
