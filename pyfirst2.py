import re
import matplotlib.pyplot as plt

def parse_trace_file(filename):
    total_bytes_recv = 0
    total_bytes_sent = 0
    total_packets_recv = 0
    total_packets_sent = 0
    packet_id = 0
    recv = {}
    sent = {}

    with open(filename, 'r') as file:
        for line in file:
            fields = line.split()
            if len(fields) < 29:
                continue  # Skip lines that don't have enough fields
            
            n = fields[2].split("/")
            
            if fields[0] == "r" and len(n) > 2 and n[2] == "1":
                total_bytes_recv += int(re.search(r'\d+', fields[28]).group())
                total_packets_recv += 1
                recv[int(fields[18])] = float(fields[1])

            elif fields[0] == "-" and len(n) > 2 and n[2] == "0":
                total_bytes_sent += int(re.search(r'\d+', fields[28]).group())
                total_packets_sent += 1
                sent[int(fields[18])] = float(fields[1])
                packet_id += 1

    timedelay = 0
    for i in range(packet_id + 1):
        if i in recv and i in sent:
            timedelay += recv[i] - sent[i]

    delay = timedelay / packet_id if packet_id > 0 else 0

    return total_bytes_sent, total_bytes_recv, total_packets_sent, total_packets_recv, delay

def calculate_throughput(total_bytes_recv, duration):
    return (total_bytes_recv * 8) / (duration)

def main():
    data_rates = ["1Mbps", "5Mbps", "10Mbps", "15Mbps", "20Mbps", "25Mbps"]
    duration = 18  # Duration of the active simulation time (2s to 20s)

    throughputs = []
    delays = []

    for data_rate in data_rates:
        filename = f"first2_{data_rate}.tr"  # Adjust the filename for each data rate
        total_bytes_sent, total_bytes_recv, total_packets_sent, total_packets_recv, delay = parse_trace_file(filename)
        
        throughput = calculate_throughput(total_bytes_recv, duration)
        
        throughputs.append(throughput)
        delays.append(delay)
        
        print(f"Data Rate: {data_rate}")
        print(f"Throughput: {throughput} bps")
        print(f"End-to-End Delay: {delay} s")
        print("-------------")

    # Plotting the graphs
    fig, axes = plt.subplots(1, 2, figsize=(15, 7))

    # Throughput vs. Data Rate
    axes[0].plot([int(dr[:-4]) for dr in data_rates], throughputs, 'ro-')
    axes[0].set_title('Throughput vs. Data Rate')
    axes[0].set_xlabel('Data Rate (Mbps)')
    axes[0].set_ylabel('Throughput (bps)')
    axes[0].set_xticks([int(dr[:-4]) for dr in data_rates])
    axes[0].set_yticks(range(0, 600, 100)) 
    axes[0].set_ylim(0, 600)

    # End-to-End Delay vs. Data Rate
    axes[1].plot([int(dr[:-4]) for dr in data_rates], delays, 'ro-')
    axes[1].set_title('End-to-End Delay vs. Data Rate')
    axes[1].set_xlabel('Data Rate (Mbps)')
    axes[1].set_ylabel('End-to-End Delay (s)')
    axes[1].set_xticks([int(dr[:-4]) for dr in data_rates])
    axes[1].set_yticks([i * 0.005 for i in range(11)])
    axes[1].set_ylim(0, 0.050)

    plt.tight_layout()
    plt.savefig('first2results.png')

if __name__ == "__main__":
    main()