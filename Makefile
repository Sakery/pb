
PROGRAM = pb
OBJS    = pb.tab.o lex.yy.o main.o node.o dump.o Device.o Statement.o kbhit.o
CC      = clang
CFLAGS  = -std=c89 -Wall
YFLAGS  = -d --debug --verbose
#YFLAGS  = -d

all: $(PROGRAM)

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $@ -O

pb.tab.c: pb.y pb.h Device.h Statement.h
	bison $(YFLAGS) pb.y

lex.yy.c: pb.l pb.h pb.tab.h
	flex pb.l

lex.yy.o: lex.yy.c
	$(CC) -std=c89 -c $*.c -o $@ -O

main.o: main.c Device.h pb.h

node.o: node.c pb.h pb.tab.h

dump.o: node.c Statement.h pb.h pb.tab.h

Device.o: Device.c Device.h Statement.h pb.h pb.tab.h

Statement.o: Statement.c Statement.h

pb: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ -ll

clean:
	rm -f $(OBJS) core *.o $(PROGRAM) pb.c y.* lex.yy.* pb.tab.* pb.output
