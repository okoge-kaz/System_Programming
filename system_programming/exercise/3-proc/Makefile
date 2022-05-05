SRCS=main.c node.c logger.c lex.yy.c y.tab.c fd.c
CC_SRCS=scan.l parse.y
HDRS=main.h y.tab.h
EXES=main fd
SCRIPTS=test.sh test-sh.sh
TXTS=report.txt
TESTS=t/1-1whoami.txt t/1-2hostname.txt t/1-3echo.txt t/1-4sleep.txt t/1-5cd.txt t/1-s1ppid.txt t/2-1echo.txt t/2-2cd.txt t/2-s1fd.txt t/2-s2ppid.txt t/3-1simplel.txt t/3-2simpler.txt t/3-3rev.txt t/3-4big.txt t/3-5yes.txt t/3-s1fd.txt t/3-s2ppid.txt t/4-1in.txt t/4-2out.txt t/4-3inout.txt t/4-4append.txt t/4-s1in-fd.txt t/4-s2out-fd.txt t/4-s3append-fd.txt t/4-s4ppid.txt t/A-1small.txt t/A-2big.txt t/A-3chain.txt t/A-4yes.txt t/A-5gzip.txt t/A-6max.txt t/A-s1fd.txt t/A-s2in-fd.txt t/A-s3out-fd.txt t/A-s4append-fd.txt t/A-s5ppid.txt t/B-1and.txt t/B-2or.txt t/B-3mixed.txt t/B-4mixed2.txt t/B-s1and-fd.txt t/B-s2or-fd.txt t/B-s3ppid.txt t/C-1echo.txt t/C-2and.txt t/C-3cd.txt t/C-s1fd.txt t/hello.txt t/services.txt
DLS=$(SRCS) $(HDRS) $(CC_SRCS) $(TXTS) $(SCRIPTS) $(TESTS)
URL=https://se.c.titech.ac.jp/lecture/sysprog/3-proc
CMD_URL=https://se.c.titech.ac.jp/lecture/sysprog/cmd

all: $(EXES)
.PHONY: all download clean zip generate-cc test test1 test2 test3 test4 testA testB testC

download:
	-mkdir -p t
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	chmod ugo+x $(SCRIPTS)

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-proc.zip
report-proc.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall -Werror
LDFLAGS=
main: main.c node.c logger.c lex.yy.c y.tab.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c node.c logger.c lex.yy.c y.tab.c
fd: fd.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	-$(RM) -f $(EXES)

OPT=
test: test1 test2 test3 test4 testA testB testC
test1: $(EXES)
	@for x in t/1-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done
test2: $(EXES)
	@for x in t/2-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done
test3: $(EXES)
	@for x in t/3-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done
test4: $(EXES)
	@for x in t/4-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done
testA: $(EXES)
	@for x in t/A-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done
testB: $(EXES)
	@for x in t/B-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done
testC: $(EXES)
	@for x in t/C-*.txt; do sh ./test-sh.sh $(OPT) -p '-q -p' -i $$x ./main; done

# require: bison and flex
generate-cc:
	type bison >/dev/null || curl -u sysprog:int128 $(CMD_URL)/install.bison.sh | sh
	type flex  >/dev/null || curl -u sysprog:int128 $(CMD_URL)/install.flex.sh  | sh
	flex -o lex.yy.c scan.l
	bison -t -v -d -o y.tab.c parse.y
