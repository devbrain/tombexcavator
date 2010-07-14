void huff_expand(unsigned char *pin, unsigned char *pout, unsigned long inlen, unsigned long outlen);
unsigned long huff_compress(unsigned char *pin, unsigned char *pout, unsigned long inlen, unsigned long outlen);
void huff_read_dictionary(FILE *fin, unsigned long offset);
void huff_setup_compression();

