#include <stdio.h>
#include <string.h>

extern void ft_rc4(char *data, size_t data_len, char *key, size_t key_len);
extern void ft_rc4_set_s(char *data);
//extern void ft_rc4_set_k(char *data, char *key, size_t ley_len);

void rc4(char *data, size_t data_len, char *key, size_t key_len, char *result)
{
	char K[256];
	char S[256];
	char tmp;

	for (int i = 0; i < 256; i++) {
//		S[i] = i;
		K[i] = key[i % key_len];
	}
	ft_rc4_set_s(S);
//	ft_rc4_set_k(K, key, key_len);

	int j = 0;
	for(int i = 0; i < 256; i++)
	{
		j = (j + S[i] + K[i]) % 256;

		tmp = S[j];
		S[j]= S[i];
		S[i] = tmp;
	}

	j = 0;
	int i = 0;
	for(size_t x = 0 ; x < data_len ; x++)
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

//	ft_rc4(data, strlen(data), key, strlen(key));

	return 0;
}
