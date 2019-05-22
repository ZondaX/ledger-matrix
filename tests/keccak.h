#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int keccak_hash(uint8_t *out, size_t outlen,
                const uint8_t *in, size_t inlen,
                size_t rate, uint8_t delim);

#ifdef __cplusplus
}
#endif
