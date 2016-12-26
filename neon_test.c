/* ARM 128bit integer addtion test 1,000,000 times
   ARM(32bit) & NEON(128bit SIMD)
   arm-linux-gnueabihf-gcc -mfpu=neon  -std=gnu99 ./neon_test.c -o neon_test
   qemu-arm ./neon_test
*/
#include <arm_neon.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

bool exit_if_result_failer(const char *buf)
{
	for (int i = 0; i < 256; i++) {
		if (buf[i] != 2) {
			fprintf(stderr, "ERROR: idx:%d, %d\n", i, buf[i]);
			exit(1);
		}
	}
}

double add_neon(const char *buf1, const char *buf2, char *buf3)
{
	uint8x8_t a;
	uint8x8_t b;
	uint8x8_t c;
	struct timeval tv0, tv1;

	gettimeofday(&tv0, NULL);
	for (int n = 0; n < 1000000; n++) {
		for (int i = 0; i < 256 / 8; i++) {
			a = vld1_u8(&buf1[i * 8]);
			b = vld1_u8(&buf2[i * 8]);
			c = vadd_u8(a, b);
			vst1_u8(&buf3[i * 8], c);
		}
	}
	gettimeofday(&tv1, NULL);
	exit_if_result_failer(buf3);
	return (tv1.tv_sec + tv1.tv_usec * 1e-6) - (tv0.tv_sec + tv0.tv_usec * 1e-6);
}

double add_arm(const char *buf1, const char *buf2, char *buf3)
{
	struct timeval tv0, tv1;
	gettimeofday(&tv0, NULL);
	for (int n = 0; n < 1000000; n++) {
		for (int i = 0; i < 256; i++) {
			buf3[i] = buf1[i] + buf2[i];
		}
	}
	gettimeofday(&tv1, NULL);
	exit_if_result_failer(buf3);
	return (tv1.tv_sec + tv1.tv_usec * 1e-6) - (tv0.tv_sec + tv0.tv_usec * 1e-6);
}

int main(int argc, char const *argv[])
{
	char buf1[256] = {};
	char buf2[256] = {};
	char buf3[256] = {};
	double diff0, diff1;

	for (int i; i < 256; i++) {
		if (buf1[i] != 0 || buf2[i] != 0) {
			fprintf(stderr, "ERROR: %d", i);
			exit(0);
		}
		buf1[i] = buf2[i] = 1;
	}

	printf("instruction test ARM & NEON\n");
	diff0 = add_arm(buf1, buf2, buf3);
	printf("ARM:  %f[sec]  (x%.2f)\n", diff0, 1.0);

	diff1 = add_neon(buf1, buf2, buf3);
	printf("NEON: %f[sec]  (x%.2f)\n", diff1, diff0 / diff1);

	return 0;
}
