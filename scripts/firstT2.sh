#!/bin/bash
datarates=("1Mbps" "5Mbps" "10Mbps" "15Mbps" "20Mbps" "25Mbps")

for datarate in "${datarates[@]}"
do
    tracefile="first2_${datarate}.tr"
    echo "The data rate is: $datarate" > $tracefile
    NS_LOG=UdpEchoClientApplication=level_all:UdpEchoServerApplication=level_all ./ns3 run scratch/firstT2 -- -datarate=$datarate >> $tracefile 2>&1
done
