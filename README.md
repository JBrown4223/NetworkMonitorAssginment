# NetworkMonitorAssginment
The Code and Background to create a program designed to track the network processes on a Linux system

Business Case (Marketing, Project Manager, Senior Executives)
Your job will be to create a network monitor, which monitors the performance of all network interface devices. Monitoring network interfaces can help in debugging system problems as well as in enhancing system performance through a central intelligent networking device.

Currently on our virtual machine, we have two network interfaces: ens33 and lo. On other machines such as routers, there may be as many as 64 network interfaces, some of them running in the order of 10GBits/second. For routers and other networking equipment, it is crucial to know the performance of each interface and to control each interface, therefore we are going to create a network monitor.

Requirements (Marketing, Project Manager, Project Lead)
1.	Our network monitor must be able to monitor any set of network interfaces requested by the user.

2.	The statistics for each network interface can be found under Linux’s /sys directory, specifically /sys/class/net/<interface-name>. The /sys directory is another directory like /proc and /dev which can be accessed to set or obtain information about the kernel's view of the system.

The statistics to be monitored are:
The operating state – operstate
The up-count (number of times the interface has been up) – carrier_up_count
The down-count (number of times the interface has been down) – carrier_down_count
The number of received bytes – rx_bytes
The number of dropped received bytes – rx_dropped
The number of erroneous received bytes – rx_errors
The number of received packets – rx_packets
The number of transmitted bytes – tx_bytes
The number of dropped transmitted bytes – tx_dropped
The number of erroneous transmitted bytes – tx_errors
The number of transmitted packets – tx_packets

3.	If an interface goes down (see operstate) then your network monitor must set it back up.

4.	Your network monitor must shut down gracefully. That means it must intercept a ctrl-C to shutdown the program. The shutdown might include the following – close file connections, deallocate memory, etc…

5.	The network monitor will act synchronously. It will not instantly react to any changes in the statistics. Rather, it will poll the statistics every one second.

6.	Your network monitor will print out the following once every second for each network interface:
Interface:<interface-name> state:<state> up_count:<up-count> down_count:<down-count>
rx_bytes:<rx-bytes> rx_dropped:<rx-dropped> rx_errors:<rx-errors> rx_packets:<rx-packets>
tx_bytes:<tx-bytes> tx_dropped:<tx-dropped> tx_errors:<tx-errors> tx_packets:<tx-packets>
Design (Project Lead, Senior Engineers)
There are two parts to the design – the interface monitor and the network monitor itself. There may be one or more interface monitors but there is only one network monitor.

The Interface Monitor
Each interface will have a process dedicated to it. This is called the interface monitor. For instance, if there are three network interfaces on a Linux machine, three instances of the interface monitor process will be running. Each interface monitor will:
•	Communicate with the network monitor through a temporary socket file in the /tmp directory.
•	Inform the network monitor when it is ready to start monitoring.
•	Monitor and print out the statistics for a given interface as per instructed by the network monitor.
•	Report to the network monitor if its interface does down.
•	Be able to set an interface link up as per instructed by the network monitor.
•	Perform a controlled shutdown if it receives a ctrl-C of if instructed by the network monitor by closing the socket connection to the network monitor then exiting.

The Network Monitor
The network monitor controls all interface monitors running on the system. In a software defined network, it is common to centralize all intelligence in one spot in order to coordinate the operations of all network devices. In our case, intelligence is centralized in the network monitor and the work is done by the interface monitors. The network monitor will:
•	Query the user for the number of interfaces and the name of each interface.
•	It will create and bind a socket to a temporary socket file in the /tmp directory.
•	It will maintain separate socket connections to each interface monitor.
•	Once an interface is ready, the network monitor will instruct the interface monitor to monitor and print out the statistics for its interface once every second.
•	If the interface monitor indicates that its link is down, the network monitor will instruct the interface monitor to put the link back up.
•	On shutdown, the network monitor will intercept the SIGINT interrupt (ctrl-C, kill -2), send a shut down message to each interface monitor, close all connections, then exit.


