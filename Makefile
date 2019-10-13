all:
	make -C program
	make -C sandbox
	cp -v program/woody_woodpacker ./
	cp -v sandbox/test ./

clean:
	make -C program clean
	make -C sandbox clean

fclean:
	make -C program fclean
	make -C sandbox fclean
	rm -f woody_woodpacker
	rm -f test

mrproper:
	make -C program mrproper
	make -C sandbox mrproper
	rm -f woody_woodpacker
	rm -f test

re: fclean all

exec:
	./woody_woodpacker woot

.PHONY: all re clean fclean mrproper
