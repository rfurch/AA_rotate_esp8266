:loop
Snmpget.exe -c:public -v:1 -r:10.129.6.230 -o:1.3.6.1.4.1.99999.1.1.1
SnmpWalk.exe -c:public -v:1 -r:10.129.6.230 -os:.1.3.6.1.4.1.99999.1.41
goto :loop
