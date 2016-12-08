IDIR =../inc
BDIR =../bin
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=obj

_DEPS = pktHandler.h udpClient.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = pktHandler.o udpClient.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

udpClient: $(OBJ)
	gcc -o $(BDIR)/$@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~  $(BDIR)/*
