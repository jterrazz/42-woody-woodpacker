#include "woody_woodpacker.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int ret;
	if (argc != 2) {
		ft_dprintf(STDERR_FILENO, "usage: %s executable_file\n", argv[0]);
		return 1;
	}

	int fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		perror_and_exit("open");
	}
	off_t off = lseek(fd, 0, SEEK_END);
	if (off == -1) {
		perror_and_exit("lseek end");
	}
	size_t file_len = (size_t)off;

	ft_printf("Mapping a file of len: %zu\n", file_len);

	off = lseek(fd, 0, SEEK_SET);
	if (off == -1) {
		perror_and_exit("lseek set");
	}
	// Use MAP_SHARED instead of MAP_PRIVATE to allow overwriting of the file
	u8 *data = mmap(0, file_len, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		perror_and_exit("mmap");
	}
	read_elf(data, file_len);

	// for (size_t i = 0; i < file_len; i++) {
	// 	write(1, &data[i], 1);
	// }
	ret = munmap(data, file_len);
	if (ret < 0) {
		perror_and_exit("munmap");
	}
	ret = close(fd);
	if (ret < 0) {
		perror_and_exit("close");
	}
	return 0;
}
