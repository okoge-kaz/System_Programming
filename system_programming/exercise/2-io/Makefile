SRCS=main.c logger.c stdio-size.c
HDRS=main.h
EXES=main
TXTS=report.txt Dockerfile
SCRIPTS=test.sh test-cat.sh
TESTS=t/hello.txt t/64.txt t/jugemu.txt t/null.txt t/cat.txt t/parrot.txt
DLS=$(SRCS) $(HDRS) $(TXTS) $(SCRIPTS) $(TESTS)
REPORTS=$(SRCS) $(HDRS) $(TXTS)
URL=https://se.c.titech.ac.jp/lecture/sysprog/2-io
CMD_URL=https://se.c.titech.ac.jp/lecture/sysprog/cmd

all: $(EXES)
.PHONY: all download clean test test1 test2 testA benchmark install_strace

download:
	-mkdir -p t
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	chmod ugo+x $(SCRIPTS)

zip: report-io.zip
report-io.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall -Werror
main: main.c logger.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c logger.c

stdio-size: stdio-size.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ stdio-size.c

clean:
	-rm -f $(EXES)
	-rm -f random.bin

install_strace:
	type strace >/dev/null || curl -u sysprog:int128 $(CMD_URL)/install.strace.sh | sh

test: test1 test2 testA

OPT=
test1: $(EXES)
	@sh ./test-cat.sh -k Hello         $(OPT)               ./main t/hello.txt
	@sh ./test-cat.sh -k 64b           $(OPT)               ./main t/64.txt
	@sh ./test-cat.sh -k more64b       $(OPT)               ./main t/jugemu.txt
	@sh ./test-cat.sh -k null          $(OPT)               ./main t/null.txt
	@sh ./test-cat.sh -k cat                                ./main t/cat.txt
	@sh ./test-cat.sh -k parrot                             ./main t/parrot.txt
	@sh ./test-cat.sh -k Hello-40      $(OPT) -p '-s 40'    ./main t/hello.txt
	@sh ./test-cat.sh -k 64b-40        $(OPT) -p '-s 40'    ./main t/64.txt
	@sh ./test-cat.sh -k more64b-40    $(OPT) -p '-s 40'    ./main t/jugemu.txt
	@sh ./test-cat.sh -k null-40       $(OPT) -p '-s 40'    ./main t/null.txt
	@sh ./test-cat.sh -k cat-40               -p '-s 40'    ./main t/cat.txt
	@sh ./test-cat.sh -k parrot-40            -p '-s 40'    ./main t/parrot.txt
test2: $(EXES)
	@sh ./test-cat.sh -k Hello-l       $(OPT) -p '-l'       ./main t/hello.txt
	@sh ./test-cat.sh -k 64b-l         $(OPT) -p '-l'       ./main t/64.txt
	@sh ./test-cat.sh -k more64b-l     $(OPT) -p '-l'       ./main t/jugemu.txt
	@sh ./test-cat.sh -k null-l        $(OPT) -p '-l'       ./main t/null.txt
	@sh ./test-cat.sh -k cat-l                -p '-l'       ./main t/cat.txt
	@sh ./test-cat.sh -k parrot-l             -p '-l'       ./main t/parrot.txt
	@sh ./test-cat.sh -k Hello-40l     $(OPT) -p '-l -s 40' ./main t/hello.txt
	@sh ./test-cat.sh -k 64b-40l       $(OPT) -p '-l -s 40' ./main t/64.txt
	@sh ./test-cat.sh -k more64b-40l   $(OPT) -p '-l -s 40' ./main t/jugemu.txt
	@sh ./test-cat.sh -k null-40l      $(OPT) -p '-l -s 40' ./main t/null.txt
	@sh ./test-cat.sh -k cat-40l              -p '-l -s 40' ./main t/cat.txt
	@sh ./test-cat.sh -k parrot-40l           -p '-l -s 40' ./main t/parrot.txt
testA: $(EXES)
	@sh ./test-cat.sh -k a1-stdin   $(OPT)         -i t/jugemu.txt ./main
	@sh ./test-cat.sh -k a2-2args   $(OPT)                         ./main t/64.txt t/jugemu.txt
	@sh ./test-cat.sh -k a3-stdin   $(OPT)         -i t/jugemu.txt ./main -
	@sh ./test-cat.sh -k a3-mixed   $(OPT)         -i t/jugemu.txt ./main t/64.txt -
	@sh ./test-cat.sh -k a1-stdin-l $(OPT) -p '-l' -i t/jugemu.txt ./main
	@sh ./test-cat.sh -k a2-2args-l $(OPT) -p '-l'                 ./main t/64.txt t/jugemu.txt
	@sh ./test-cat.sh -k a3-stdin-l $(OPT) -p '-l' -i t/jugemu.txt ./main -
	@sh ./test-cat.sh -k a3-mixed-l $(OPT) -p '-l' -i t/jugemu.txt ./main t/64.txt -

export TIMEFORMAT=real: %R, user: %U, sys: %S
SIZE=52428800 # 50MB
benchmark: $(EXES)
	@cat /dev/urandom | base64 | head -c $(SIZE) > random.bin
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s 4194304'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s  524288'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s   65536'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s    8192'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s    1024'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s     128'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s      16'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '   -s       2'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s 4194304'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s  524288'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s   65536'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s    8192'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s    1024'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s     128'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s      16'
	@bash -c 'printf "[$$0] "; eval "time ./main -q $$0 random.bin >/dev/null"' '-l -s       2'
