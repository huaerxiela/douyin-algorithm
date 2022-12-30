#include <stdint.h>
#include <stdio.h>
#include "simon.h"

#define WORD_SZ		64
#define T			72
#define J			4
#define M			4
#define BIT(x)		(1LL << (x))
// It is what it is
static uint64_t z = 0x3DC94C3A046D678B;
static uint8_t print = 0;

static inline uint64_t get_bit(uint64_t val, int pos)
{
	return (val & BIT(pos)) ? 1 : 0;
}

static inline uint64_t rotate_left(uint64_t val, uint8_t num)
{
	return 	(val << num) | (val >> (WORD_SZ - num));
}

static inline uint64_t rotate_right(uint64_t val, uint8_t num)
{
	return 	(val << (WORD_SZ - num)) | (val >> num);
}

static void key_expansion(uint64_t key[T])
{
	uint64_t tmp;

	if (print) {
		printf("----Key Schedule----\n");
		for (int i = 0; i < 4; i++) {
			printf("%02d - 0x%016llx\n", i + 1, key[i]);
		}
	}

	for (int i = M; i < T; i++) {
		tmp = rotate_right(key[i-1], 3);
//		printf("\t%02d - 0x%016llx\n", i + 1, tmp);
		tmp = tmp ^ key[i-3];
//		printf("\t%02d - 0x%016llx\n", i + 1, tmp);
		tmp = tmp ^ rotate_right(tmp, 1);
//		printf("\t%02d - 0x%016llx\n", i + 1, tmp);
//		printf("\t%02d - 0x%016llx\n", i + 1, ~key[i-M]);
//		printf("\t%02d - 0x%016llx\n", i + 1, get_bit(z, (i - M) % 62));
		key[i] = ~key[i-M] ^ tmp ^ get_bit(z, (i - M) % 62) ^ 3;

		if (print) {
			printf("%02d - 0x%016llx\n", i + 1, key[i]);
		}
	}
}

static void bkwrd_rnd(uint64_t pt[2], uint64_t ct[2], uint64_t k[4], cipher_t c)
{
	uint64_t tmp, f;
	uint64_t key[T] = {0};

	key[0] = k[0]; key[1] = k[1]; key[2] = k[2]; key[3] = k[3];
	key_expansion(key);

	uint64_t x_i = ct[0];
	uint64_t x_i1 = ct[1];

	if (print) {
		printf("----Decryption round----\n");
		printf("r x_i1 x_i\n");
		printf("%02d - 0x%016llx 0x%016llx\n", 0, x_i1, x_i);
	}

	for (int i = T-1; i >= 0; i--) {
		tmp = x_i;
		f = c == SSIMON ? rotate_left(x_i, 1) ^ rotate_left(x_i, 8) :
						  rotate_left(x_i, 1) & rotate_left(x_i, 8);
		x_i = x_i1 ^ f ^ rotate_left(x_i, 2) ^ key[i];
		x_i1 = tmp;

		if (print) {
			printf("%02d - 0x%016llx 0x%016llx\n", i + 1, x_i1, x_i);
		}
	}

	pt[0] = x_i;
	pt[1] = x_i1;
}

static void frwd_rnd(uint64_t pt[2], uint64_t ct[2], uint64_t k[4], cipher_t c)
{
	uint64_t tmp, f;
	uint64_t key[T] = {0};

	key[0] = k[0];
    key[1] = k[1];
    key[2] = k[2];
    key[3] = k[3];
	key_expansion(key);

	uint64_t x_i = pt[0];
	uint64_t x_i1 = pt[1];

	if (print) {
		printf("----Encryption round----\n");
		printf("x_i1 x_i\n");
		printf("%02d - 0x%016llx 0x%016llx\n", 72, x_i1, x_i);
	}

	for (int i = 0; i < T; i++) {
		tmp = x_i1;
		f = c == SSIMON ? rotate_left(x_i1, 1) ^ rotate_left(x_i1, 8) :
						  rotate_left(x_i1, 1) & rotate_left(x_i1, 8);
		x_i1 = x_i ^ f ^ rotate_left(x_i1, 2) ^ key[i];
		x_i = tmp;

		if (print) {
			printf("%02d - 0x%016llx 0x%016llx\n", i + 1, x_i1, x_i);
		}
	}

	ct[0] = x_i;
	ct[1] = x_i1;
}

void ssimon_enc(uint64_t pt[2], uint64_t ct[2], uint64_t k[4])
{
	frwd_rnd(pt, ct, k, SSIMON);
}

void ssimon_dec(uint64_t pt[2], uint64_t ct[2], uint64_t k[4])
{
	bkwrd_rnd(pt, ct, k, SSIMON);
}

void simon_enc(uint64_t pt[2], uint64_t ct[2], uint64_t k[4])
{
	frwd_rnd(pt, ct, k, SIMON);
}

void simon_dec(uint64_t pt[2], uint64_t ct[2], uint64_t k[4])
{
	bkwrd_rnd(pt, ct, k, SIMON);
}

void set_verbose(void)
{
	print = 1;
}

