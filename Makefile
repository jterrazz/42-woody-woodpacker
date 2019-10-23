all:
	make -C program
	make -C sandbox
	cp -v program/woody_woodpacker ./

clean:
	make -C program clean
	make -C sandbox clean

fclean:
	make -C program fclean
	make -C sandbox fclean
	rm -f woody_woodpacker
	rm -f tmpwoody

mrproper:
	make -C program mrproper
	make -C sandbox mrproper
	rm -f woody_woodpacker
	rm -f tmpwoody

re: fclean all

exec:
	./woody_woodpacker woot

.PHONY: all re clean fclean mrproper
