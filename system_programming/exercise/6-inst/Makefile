SRCS=inst0.c inst0-simple.c cg.c
HDRS=
EXES=inst0 inst0-simple mygzip
TXTS=cat64.txt report.txt replit-inst.nix
ARCS=gzip-1.2.4.tar.gz
PICS=inst0-simple.svg inst0-simple-label.svg mygzip.svg mygzip-label.svg
TESTS=

all: $(EXES)
pics: $(PICS)
.PHONY: all pics download clean zip svg

URL=https://se.c.titech.ac.jp/lecture/sysprog/6-inst
DLS=$(SRCS) $(HDRS) $(TXTS) $(ARCS) $(TESTS)
download:
	for x in $(DLS); do [ -f $$x ] || curl -u sysprog:int128 $(URL)/$$x -o $$x; done
	cp replit-inst.nix replit.nix

REPORTS=$(SRCS) $(HDRS) report.txt
zip: report-inst.zip
report-inst.zip: $(REPORTS)
	zip $@ $(REPORTS)

CC=clang
CFLAGS=-std=gnu99 -Wall $(CFLAGS_COMMON)
CFLAGS_COMMON=-finstrument-functions -D __inline='__inline __attribute__((no_instrument_function))'
LDFLAGS=-ldl -rdynamic

svg: inst0-simple.svg inst0-simple-label.svg mygzip.svg mygzip-label.svg

inst0: inst0.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

inst0-simple: inst0-simple.c cg.c
	$(CC) $(CFLAGS) -o $@ inst0-simple.c cg.c $(LDFLAGS)
inst0-simple.svg: inst0-simple
	./inst0-simple
	dot -Tsvg cg.dot > $@
inst0-simple-label.svg: inst0-simple
	env SYSPROG_LABEL=1 ./inst0-simple
	dot -Tsvg cg.dot > $@

# gzip-1.2.4/configure: gzip-1.2.4.tar.gz
GZIP_MIN=gzip zip deflate trees bits unzip inflate util crypt lzw unlzw unpack unlzh getopt
GZIP_SRCS=$(GZIP_MIN:%=gzip-1.2.4/%.c)
GZIP_OBJS=$(GZIP_MIN:%=gzip-1.2.4/%.o)
gzip-1.2.4/.extracted: gzip-1.2.4.tar.gz
	tar zxvf gzip-1.2.4.tar.gz
	# make static non-static
	ruby -i.bak -pe '$$_.sub!(/^local\b/,"/*local*/") if /^local .*\(/' $(GZIP_SRCS)
	ruby -i.bak -pe '$$_.sub!(/build_tree/,"build_tree_unpack")' gzip-1.2.4/unpack.c
	touch $@
gzip-1.2.4/Makefile: gzip-1.2.4/.extracted
	(cd gzip-1.2.4 && env CC=$(CC) CFLAGS='$(CFLAGS_COMMON)' ./configure)
	# mac: -DHAVE_UNISTD_H=1 -DVOID_CLOSEDIR=1 -DRETSIGTYPE=int
	# repl.it: -DSTDC_HEADERS=1 -DHAVE_UNISTD_H=1 -DDIRENT=1
mygzip-formal: gzip-1.2.4/Makefile cg.c
	(cd gzip-1.2.4 && make $(GZIP_MIN:%=%.o))
	$(CC) $(CFLAGS_COMMON) $(LDFLAGS) -o $@ $(GZIP_OBJS) cg.c
gzip-1.2.4/gzip.o: gzip-1.2.4/.extracted
	(cd gzip-1.2.4 && $(CC) -c $(CFLAGS_COMMON) -w -DHAVE_UNISTD_H $(LDFLAGS) $(GZIP_MIN:%=%.c))
mygzip: gzip-1.2.4/gzip.o cg.c
	$(CC) $(CFLAGS_COMMON) -w -DHAVE_UNISTD_H -o $@ $(GZIP_OBJS) cg.c $(LDFLAGS)
mygzip.svg: mygzip
	./mygzip -c cat64.txt > /dev/null
	dot -Tsvg cg.dot > $@
mygzip-label.svg: mygzip
	env SYSPROG_LABEL=1 ./mygzip -c cat64.txt > /dev/null
	dot -Tsvg cg.dot > $@

clean:
	-rm -rf $(EXES) $(PICS) *.dSYM *.o cg.dot gzip-1.2.4
