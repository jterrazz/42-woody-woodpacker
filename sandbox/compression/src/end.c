
#ifndef SILENT
	printf("Output file len: %zu\n", new_len);
#endif

	STREAM *out = sopen(argv[2], new_len);
	if (out == NULL) {
		dprintf(STDERR_FILENO, "Cannot create new file\n");
		exit(1);
	}
	ret = swrite(out, new_data, 0, new_len);
	if (ret < 0) {
		dprintf(STDERR_FILENO, "Write error on new file\n");
		exit(1);
	}
	ret = sclose(out);
	if (ret < 0) {
		dprintf(STDERR_FILENO, "Cannot close new file\n");
		exit(1);
	}

	free(new_data);

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
