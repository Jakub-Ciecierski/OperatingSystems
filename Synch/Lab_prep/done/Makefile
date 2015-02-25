#FIY: 	$@ is the target, for example common.o
#	$< is the first file in the requirements list
#	$^ is entire requirements list
#	.PHONY means that this targets are not real files
#	all is what is build if you did not stated anything else
#	you can always retype `make stage1` in the terminal

CFLAGS=-Wall # -g
LDFLAGS= #-g
all: stage1 stage2 stage3 stage4

stage1: cons_1 prod_1

stage2: cons_2 prod_2

stage3: cons_3 prod_2

stage4: cons_4 prod_2

common.o: common.c common.h
	gcc $(CFLAGS) -o $@ -c $<

prod_1: prod_1.c common.o
	gcc $(LDFLAGS) -o $@ $^

prod_2: prod_2.c common.o
	gcc $(LDFLAGS) -o $@ $^

cons_1: cons_1.c common.o
	gcc $(LDFLAGS) -o $@ $^

cons_2: cons_2.c common.o
	gcc $(LDFLAGS) -o $@ $^

cons_3: cons_3.c common.o
	gcc $(LDFLAGS) -o $@ $^

cons_4: cons_4.c common.o
	gcc $(LDFLAGS) -o $@ $^

clean:
	rm *.o cons_1 cons_2 cons_3 cons_4 prod_1 prod_2

zip:
	zip Lab3tut.zip Makefile *.c *.h
.PHONY: clean all stage1 stage2 stage3 stage4
