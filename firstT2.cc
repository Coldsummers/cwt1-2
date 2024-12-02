/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/trace-helper.h"
#include "ns3/mobility-module.h"
#include "ns3/animation-interface.h"
#include <iostream>

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1
//    point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

void ReceivePacket(Ptr<const Packet> packet) {
    NS_LOG_UNCOND(Simulator::Now().GetSeconds() << "s Packet received size: " << packet->GetSize());
}

int
main(int argc, char* argv[])
{
    CommandLine cmd(__FILE__);
    std::string datarate = "5Mbps";
    cmd.AddValue("datarate", "Data rate per second", datarate);
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(4);

    NodeContainer n0n1 = NodeContainer (nodes.Get (0), nodes.Get (1));
    NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
    NodeContainer n2n3 = NodeContainer (nodes.Get (2), nodes.Get (3));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue(datarate));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));

    NetDeviceContainer d0d1 = pointToPoint.Install (n0n1);
    NetDeviceContainer d1d2 = pointToPoint.Install (n1n2);
    NetDeviceContainer d2d3 = pointToPoint.Install (n2n3);

    //NetDeviceContainer devices;
    //devices = pointToPoint.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    NS_LOG_INFO ("Assign IP Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);
    ipv4.SetBase ("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);
    ipv4.SetBase ("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(3));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    UdpEchoClientHelper echoClient(i2i3.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(200));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(0.5)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(20.0));

    //devices.Get(1)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&ReceivePacket));
    //devices.Get(2)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&ReceivePacket));
    //devices.Get(3)->GetObject<PointToPointNetDevice>()->TraceConnectWithoutContext("PhyRxEnd", MakeCallback(&ReceivePacket));


    MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    AnimationInterface anim("first2.xml");

    std::string traceFileName = "first2_" + datarate + ".tr";
    AsciiTraceHelper ascii;
    Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream(traceFileName);
    *stream->GetStream() << "The data rate is: " << datarate << "\n";  // Log the data rate
    pointToPoint.EnableAsciiAll(stream);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
