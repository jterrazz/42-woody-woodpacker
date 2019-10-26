#include <stdio.h>
#include <string.h>

extern void ft_rc4(char *data, size_t data_len, char const *key, size_t key_len);

void rc4(char *data, size_t data_len, char const *key, size_t key_len, char *result)
{
	unsigned char T[256];
	unsigned char S[256];
	unsigned char tmp;

	for (int i = 0; i < 256; i++) {
		S[i] = i;
		T[i] = key[i % key_len];
	}

	int j = 0;
	for(int i = 0; i < 256; i++)
	{
		j = (j + S[i] + T[i]) % 256;

		tmp = S[j];
		S[j]= S[i];
		S[i] = tmp;
	}

	j = 0;
	int i = 0;
	for(int x = 0 ; x < data_len ; x++)
	{
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;

		tmp = S[j];
		S[j]= S[i];
		S[i] = tmp;

		int t = (S[i] + S[j]) % 256;
		result[x] = data[x]^S[t];
	}
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	char *data = strdup("meuh42");
	char *key = "NICEKEY";

	rc4(data, strlen(data), key, strlen(key), data);
	printf("Encrypted: %s\n", data);

	rc4(data, strlen(data), key, strlen(key), data);
	printf("Decrypted: %s\n", data);

	ft_rc4(data, strlen(data), key, strlen(key));

	return 0;
}
