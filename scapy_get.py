from scapy.all import *
from scapy.layers.snmp import *


########### regular packet for SNMP GET

target_ip = "10.129.6.230"
community = "public"
oid = "1.3.6.1.4.1.99999.1.1.1"   # sysName.0

snmp_pkt = (
    IP(dst=target_ip) /
    UDP(sport=RandShort(), dport=161) /
    SNMP(
        version=1,  # SNMPv2c
        community=community,
        PDU=SNMPget(
            varbindlist=[SNMPvarbind(oid=oid)]
        )
    )
)

send(snmp_pkt, verbose=True)


resp = sr1(snmp_pkt, timeout=2, verbose=False)
if resp and SNMP in resp:
    print(resp[SNMP].show())
else:
    print("No response")

time.sleep(2)


############  Send only part of the UDP payload

raw = bytes(snmp_pkt[UDP].payload)
partial = raw[:10]   # cut payload
bad_pkt = IP(dst=target_ip) / UDP(sport=RandShort(), dport=161) / Raw(load=partial)
send(bad_pkt)


resp = sr1(snmp_pkt, timeout=2, verbose=False)
if resp and SNMP in resp:
    print(resp[SNMP].show())
else:
    print("No response")

time.sleep(2)

##################### Corrupt ASN.1 length field

raw = bytearray(bytes(snmp_pkt[UDP].payload))
raw[1] = 0xFF  # corrupt length byte
corrupt_pkt = IP(dst=target_ip) / UDP(sport=RandShort(), dport=161) / Raw(load=raw)
send(corrupt_pkt)

time.sleep(2)

###################) Send SNMP packet in two fragments 

frags = fragment(snmp_pkt, fragsize=24)
for f in frags:
    send(f)

time.sleep(2)

######################## Wrong community string

snmp_bad_comm = (
    IP(dst=target_ip) /
    UDP(sport=RandShort(), dport=161) /
    SNMP(
        version=1,
        community="wrongcomm",
        PDU=SNMPget(varbindlist=[SNMPvarbind(oid=oid)])
    )
)

time.sleep(2)


####################### Random garbage UDP packet


send(
    IP(dst=target_ip) /
    UDP(sport=RandShort(), dport=161) /
    Raw(load=os.urandom(20))
)

time.sleep(2)

#################  Stress test (slow UDP exhaustion)


for i in range(1000):
    send(snmp_pkt, verbose=False)
    time.sleep(0.05)

time.sleep(2)

 



## regular packet for SNMP GET / SNMPWALK (no delay!)
## Send only part of the UDP payload
## Corrupt ASN.1 length field
## Send SNMP packet in two fragments 
## Wrong community string
## Random garbage UDP packet
## Stress test (1000 packets, UDP exhaustion)
