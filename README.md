# Netfilter Firewall
## FWC language
```ebnf
<STATEMENT>-><EXP_OR>
<EXP_OR>->[<EXP_OR> "|"] <EXP_AND>
<EXP_AND>->[<EXP_AND> "|"] <EXP_NOT>
<EXP_NOT>->["!"](<SADDR>|<SPORT>|<PROTOCOL>)
<SADDR>->"SADDR" "(" ip ")"
<SPORT>->"SPORT" "(" integer ")"
<PROTOCOL>->"SADDR" "(" protocol ")"

ip->integer.integer.integer.integer
integer->non-zero-digital{digital}
protocol->"tcp"|"udp"|"icmp"
non-zero-digital->"1"|"2"|"3"|"4"|"5"|"6"|"7"|"8"|"9"
zero-digital->"0"|non-zero-digital
```