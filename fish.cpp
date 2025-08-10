// mod_Fish.cpp
// Fusion: crypt.cpp (ZNC) + original FiSH blowfish ECB implementation
// - CBC (Mircryption compatible) is the default
// - ECB (FiSH) supported for backward compatibility
// - NV storage format: "CBC:<key>" or "ECB:<key>"
// - Additional NV keys / flags (disabled, topic encryption) are simple NV entries
//
// Sources merged:
// - crypt.cpp (znc official) -- used as base (modern API, DH1080 handling)
// - original Fish module (Blowfish ECB functions and base64 table)

#include <znc/Chan.h>
#include <znc/IRCNetwork.h>
#include <znc/SHA256.h>
#include <znc/User.h>
#include <znc/Modules.h>
#include <znc/Utils.h>
#include <znc/Message.h>
#include <znc/ZNCString.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <openssl/bn.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/blowfish.h>
#include <openssl/evp.h>
#pragma GCC diagnostic pop

#include <arpa/inet.h>
#include <map>
#include <memory>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <cstring>
#include <cstdlib>
#include <vector>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

using std::map;
using std::make_pair;

/*** FiSH-specific base64 & Blowfish ECB helpers (from original Fish) ***/
unsigned char B64[] = "./0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static inline bool IsFishB64Char(unsigned char c) {
    for (int i = 0; i < 64; ++i) if (B64[i] == c) return true;
    return false;
}

static int base64dec_fish(char c) {
    for (int i = 0; i < 64; ++i)
        if (B64[i] == (unsigned char)c) return i;
    return 0;
}

static void fish_derive_key_md5(const char* pass, unsigned char out16[16]) {
    unsigned int mdlen = 0;
    EVP_Digest(pass, strlen(pass), out16, &mdlen, EVP_md5(), nullptr);
    if (mdlen < 16) memset(out16 + mdlen, 0, 16 - mdlen);
}

/* Blowfish ECB encrypt -> custom base64 used by FiSH */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
static char* encrypts_fish(const char* key, const char* str) {
    if (key == nullptr || str == nullptr) return nullptr;

    const size_t in_len = strlen(str);
    const size_t pad = (8 - (in_len % 8)) % 8;
    const size_t total = in_len + pad;

    // Prepare padded input
    unsigned char* inbuf = (unsigned char*)malloc(total);
    if (!inbuf) return nullptr;
    memcpy(inbuf, str, in_len);
    if (pad) memset(inbuf + in_len, 0x00, pad);

    // Output: 12 chars per 8-byte block, plus NUL
    const size_t blocks = total / 8;
    const size_t out_len = blocks * 12 + 1;
    char* out = (char*)malloc(out_len);
    if (!out) { free(inbuf); return nullptr; }

    BF_KEY bfkey;
    BF_set_key(&bfkey, (int)strlen(key), reinterpret_cast<const unsigned char*>(key));

    char* d = out;
    for (size_t b = 0; b < blocks; ++b) {
        unsigned char cipher[8];
        BF_ecb_encrypt(inbuf + (b * 8), cipher, &bfkey, BF_ENCRYPT);

        // FiSH packs as big-endian words, then emits 6b chunks least-significant-first (R then L)
        uint32_t l = ((uint32_t)cipher[0] << 24) | ((uint32_t)cipher[1] << 16) |
                     ((uint32_t)cipher[2] << 8)  |  (uint32_t)cipher[3];
        uint32_t r = ((uint32_t)cipher[4] << 24) | ((uint32_t)cipher[5] << 16) |
                     ((uint32_t)cipher[6] << 8)  |  (uint32_t)cipher[7];

        for (int i = 0; i < 6; ++i) { *d++ = B64[r & 0x3F]; r >>= 6; }
        for (int i = 0; i < 6; ++i) { *d++ = B64[l & 0x3F]; l >>= 6; }
    }
    *d = '\0';

    // Cleanup
    memset(inbuf, 0, total);
    free(inbuf);
    return out;
}

/* Blowfish ECB decrypt from custom base64 (FiSH) */
static char* decrypts_fish(const char* key, const char* str) {
    if (key == nullptr || str == nullptr) return nullptr;

    const size_t in_len = strlen(str);
    if (in_len % 12 != 0) return nullptr; // invalid length

    const size_t blocks = in_len / 12;
    const size_t out_bin_len = blocks * 8;
    unsigned char* bin = (unsigned char*)malloc(out_bin_len);
    if (!bin) return nullptr;

    BF_KEY bfkey;
    BF_set_key(&bfkey, (int)strlen(key), reinterpret_cast<const unsigned char*>(key));

    const char* p = str;
    unsigned char* w = bin;
    for (size_t b = 0; b < blocks; ++b) {
        uint32_t r = 0, l = 0;
        for (int i = 0; i < 6; ++i) { r |= (uint32_t)base64dec_fish(*p++) << (i * 6); }
        for (int i = 0; i < 6; ++i) { l |= (uint32_t)base64dec_fish(*p++) << (i * 6); }

        // Convert to big-endian byte order (network) into block
        r = htonl(r);
        l = htonl(l);
        unsigned char block[8];
        memcpy(block, &l, 4);
        memcpy(block + 4, &r, 4);

        BF_ecb_encrypt(block, block, &bfkey, BF_DECRYPT);
        memcpy(w, block, 8);
        w += 8;
    }

    // Strip trailing NUL padding
    size_t plain_len = out_bin_len;
    while (plain_len > 0 && bin[plain_len - 1] == '\0') plain_len--;

    char* out = (char*)malloc(plain_len + 1);
    if (!out) { memset(bin, 0, out_bin_len); free(bin); return nullptr; }
    memcpy(out, bin, plain_len);
    out[plain_len] = '\0';

    memset(bin, 0, out_bin_len);
    free(bin);
    return out;
}
#pragma GCC diagnostic pop

// ...
