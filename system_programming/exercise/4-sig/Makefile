SRCS=main.c node.c logger.c lex.yy.c y.tab.c count.c signal0.c
CC_SRCS=scan.l parse.y
HDRS=main.h y.tab.h
EXES=main count signal0
SCRIPTS=test.sh pty.rb
TXTS=report.txt
TESTS=t/3-in.txt t/3-out.txt t/A-in.txt t/A-out.txt
INST_URL=https://se.c.titech.ac.jp/lecture/sysprog/cmd

all: $(EXES)
.PHONY: all download clean zip generate-cc test test3 testA

URL=https://se.c.titech.ac.jp/lecture/sysprog/4-sig
DLS=$(SRCS) $(HDRS) $(CC_SRCS) $(TXTS) $(SCRIPTS) $(TESTS)
download:
	-mkdir -p t
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	chmod ugo+x $(SCRIPTS)

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-sig.zip
report-sig.zip: $(REPORTS)
	zip $@ $+

CC=clang
CFLAGS=-std=gnu99 -Wall
LDFLAGS=
main: main.c node.c logger.c lex.yy.c y.tab.c $(HDRS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ main.c node.c logger.c lex.yy.c y.tab.c
count: count.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
signal0: signal0.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	-rm -f $(EXES)

OPT=-v
test: test3 testA
test3: $(EXES)
	@sh test.sh -w $(OPT) -k 3-count -i t/3-in.txt -o t/3-out.txt env NO_EMOJI=1 ruby pty.rb ./main -q
testA: $(EXES)
	@sh test.sh -w $(OPT) -k A-count -i t/A-in.txt -o t/A-out.txt env NO_EMOJI=1 ruby pty.rb ./main -q

# require: bison and flex
generate-cc:
	type bison >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.bison.sh | sh
	type flex  >/dev/null || curl -u sysprog:int128 $(INST_URL)/install.flex.sh  | sh
	flex -o lex.yy.c scan.l
	bison -t -v -d -o y.tab.c parse.y
