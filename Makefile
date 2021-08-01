#author -ofir ovadia
# measure=server , sender = client  - TCP task

all: sender measure

measure: measure.c
	gcc -o measure measure.c

sender: sender.c
	gcc -o sender sender.c

clean:
	rm -f *.o measure sender

runs:
	./measure

runc:
	./sender

runs-strace:
	strace -f ./measure

runc-strace:
	strace -f ./sender
