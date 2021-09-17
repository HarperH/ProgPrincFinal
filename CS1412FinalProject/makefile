SRCDIR = ./src
OBJDIR = ./obj
INCDIR = ./inc
LIBDIR = ./lib

CC = gcc
CFLAGS =-I$(INCDIR)

AR = ar
ARFLAGS =-cq

WINS = -lws2_32

all: client.exe server.exe

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ 
	
$(LIBDIR)/%.a: $(OBJDIR)/%.o
	$(AR) $(ARFLAGS) $@ $<

client.exe: $(OBJDIR)/client_test.o $(LIBDIR)/clientcode.a
	$(CC) $(CFLAGS) $(OBJDIR)/client_test.o $(LIBDIR)/clientcode.a -o client.exe $(WINS)
	
server.exe: $(OBJDIR)/server_test.o $(LIBDIR)/servercode.a
	$(CC) $(CFLAGS) $(OBJDIR)/server_test.o $(LIBDIR)/servercode.a -o server.exe $(WINS)

clean:
	rm $(OBJDIR)/*.o
	rm $(LIBDIR)/*.a
	rm *.exe