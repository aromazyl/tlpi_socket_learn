#
# Makefile
# zhangyule, 2017-04-09 22:49
#

all: t_select poll_pipes epoll_input
	@echo "Makefile needs your attention"


# vim:ft=make
#
common_headers: ./common/src/*.c
	gcc -c ./common/src/*.c -I ./common/include/
	(ar rcs librtpl.a ./*.o; rm *.o)

t_select: t_select.c common_headers
	gcc -o ./bin/t_select ./t_select.c -lrtpl -L./

poll_pipes: poll_pipes.c common_headers
	gcc -o ./bin/poll_pipes ./poll_pipes.c -lrtpl -L./

epoll_input: epoll_input.c common_headers
	gcc -o ./bin/epoll_input ./epoll_input.c -lrtpl -L./

clean:
	rm -rf ./bin/* librtpl.a *.o
