# Netfilter Firewall
## License
[LICENSE](./LICENSE)
## Requirement
1. Linux Kernel (Only tested on version 5.15.0-107-generic)
2. Linux Source (With KBuild)
## Compile
Use `make`.
## Notice
Port is available for only TCP and UDP.
Transport layer protocol is available for only IPv4.
## FW Code
### Registers
Provide 32-bit generic registers(number from 0 to 31), #0 always 0.  
#### Initial State
Little endian.
```
PC = 0
r0-19 = 0
r20 = SADDR(IPv4 and IPv6 0-3)
r21 = SADDR(IPv6 4-7)
r22 = SADDR(IPv6 8-11)
r23 = SADDR(IPv6 12-15)
r24 = DADDR(IPv4 and IPv6 0-3)
r25 = DADDR(IPv6 4-7)
r26 = DADDR(IPv6 8-11)
r27 = DADDR(IPv6 12-5)
r28 = SPORT
r29 = DPORT
r30 = NETWORK LAYER PROTOCOL (exclude ICMP)
r31 = TRANSPORT LAYER PROTOCOL (include ICMP)
```
##### PROTOCOL Code
The same as Linux
#### Final State
```
r0 = 0
r1 = result
```
### Instructions
Instruction length: 4byte(32bit)  
Operation Code length: 6bit  
```
R-Type | Operation Code | reserve11 | Rk | Rj | Rd |
I-Type | Operation Code |   Immediate16  | Rj | Rd |
```
#### NOP
Operation Code: 000000
```
No effect
```
#### LAND
Operation Code: 000001
```
Rd <- Rj && Rk
```
#### LOR
Operation Code: 000010
```
Rd <- Rj || Rk
```
#### LNOT
Operation Code: 000011
```
Rd <- !Rj
```
#### CMPEQU
Operation Code: 000100
```
Rd <- Rj == Rk
```
#### SWAP
Operation Code: 000101
```
Rd <- (Rd << 16) | (Rd >> 16)
```
#### ADDI
Operation Code: 000102
```
Rd <- Rj + Immediate16
```
#### HLT
Operation Code: 000110
```
Stop the programe
```