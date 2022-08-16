CC = gcc
FLAGS = -g 
RM = rm -f
prefix = /usr/local

INCS = Incs.c StructInc.c AlphabetInc.c DeplnInc.c CodeInc.c ArgInc.c ComplexInc.c

SRCS = tc_new tc_aug tc_deaug tc_reaug tc_param tc_sets tc_list tc_rank tc_enc tc_dec tc_decomp tc_strings tc_struct tc_qtcie udp_send udp_listen t_pack t_unpack tc_fxdlst

OBJS = $(SRCS:.c=.o)


all: $(SRCS)

tc_new: tc_new.c $(INCS)
	$(CC) tc_new.c -o tc_new

tc_aug: tc_aug.c $(INCS)
	$(CC) tc_aug.c -o tc_aug

tc_deaug: tc_deaug.c $(INCS)
	$(CC) tc_deaug.c -o tc_deaug

tc_reaug: tc_reaug.c $(INCS)
	$(CC) tc_reaug.c -o tc_reaug

tc_param: tc_param.c $(INCS)
	$(CC) tc_param.c -o tc_param

tc_sets: tc_sets.c $(INCS)
	$(CC) tc_sets.c -o tc_sets

tc_list: tc_list.c $(INCS)
	$(CC) tc_list.c -o tc_list

tc_rank: tc_rank.c $(INCS)
	$(CC) tc_rank.c -o tc_rank

tc_decomp: tc_decomp.c $(INCS)
	$(CC) tc_decomp.c -o tc_decomp

tc_strings: tc_strings.c $(INCS)
	$(CC) tc_strings.c -o tc_strings

tc_struct: tc_struct.c $(INCS)
	$(CC) tc_struct.c -o tc_struct

tc_enc: tc_enc.c $(INCS)
	$(CC) tc_enc.c -o tc_enc

tc_dec: tc_dec.c $(INCS)
	$(CC) tc_dec.c -o tc_dec

#tc_even: tc_even.c $(INCS)
#	$(CC) tc_even.c -o tc_even

udp_send: udp_send.c 
	$(CC) udp_send.c -o udp_send

udp_listen: udp_listen.c 
	$(CC) udp_listen.c -o udp_listen

t_pack: t_pack.c $(INCS)
	$(CC) t_pack.c -o t_pack

t_unpack: t_unpack.c $(INCS)
	$(CC) t_unpack.c -o t_unpack

tc_fxdlst: tc_fxdlst.c $(INCS)
	$(CC) tc_fxdlst.c -o tc_fxdlst

tc_qtcie: tc_qtcie.c 
	$(CC) tc_qtcie.c -o tc_qtcie


clean:
	$(RM) $(SRCS)
	
.PHONY: install
install: $(SRCS)
	install -m 0755 $(SRCS) $(prefix)/bin

#.PHONY: uninstall
#uninstall:
#	-rm -f $(prefix)/bin/PiCW
