#ifndef SIMON_H
#define SIMON_H

#include <stdint.h>

typedef enum cipher_t {
	SIMON =		0,
	SSIMON =	1
} cipher_t;

void simon_enc(uint64_t pt[2], uint64_t ct[2], uint64_t k[4]);

void simon_dec(uint64_t pt[2], uint64_t ct[2], uint64_t k[4]);

void ssimon_enc(uint64_t pt[2], uint64_t ct[2], uint64_t k[4]);

void ssimon_dec(uint64_t pt[2], uint64_t ct[2], uint64_t k[4]);

void set_verbose(void);

#endif

