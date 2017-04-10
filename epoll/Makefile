#
# Makefile
# zhangyule, 2017-04-09 22:49
#

all: t_select
	@echo "Makefile needs your attention"


# vim:ft=make
#
common_headers: ./common/src/*.c
	gcc -c ./common/src/*.c -I ./common/include/
	(ar rcs librtpl.a ./*.o; rm *.o)

t_select: t_select.c common_headers
	gcc -o t_select ./t_select.c -lrtpl -L./

clean:
	rm -rf t_select librtpl.a *.o
