
#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;
char socket_path[] = "/tmp/a1";
int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char buf[256];
	int fd, n, rx;
	bool isRunning = true;
	char c;
	int numRead = 0;
	string filePath(argv[1]);
	filePath = "/sys/class/net/" + filePath;
	string fileP;
	const char* t;
	memset(&addr, 0, sizeof(addr));
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		cout << "client: " << strerror(errno) << endl;
		exit(-1);
	}
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		cout << "client: " << strerror(errno) << endl;
		close(fd);
		exit(-1);
	}
	//Step 1: Ready - this let's the network monitor know the interface is ready for instructions
	memset(buf, 0, sizeof(buf));
	strncpy(buf, "Ready", 6);
	n = write(fd, buf, sizeof(buf));
	while (isRunning && (n = read(fd, buf, 99)) > 0) {
		//Step 2: Recieve message to Monitor
		if (strcmp(buf, "Monitor") == 0) {
	

			//Step 3: Recieve and Open Interface
			cout << "Interface:";
			cout<< argv[1] << " ";
			
			/*
				The following blocks of code perform these operations:
				1 - open the specified file for reading
				2 - read the file via a buffer
				3 -print the contents

			*/
			//Operstate
			fileP = filePath + "/operstate";
			t = fileP.c_str();
			rx = open(t, O_RDONLY);
			if (rx == -1) {
				cout << "error: " << strerror(errno) << endl;
			}
			else {
				cout << "state:";
				numRead = 0;
				do {
					read(rx, &c, 1);
					buf[numRead] = c;
					numRead++;
				} while (c != '\n'&&numRead < 256);
				buf[numRead-1] = '\0';
				cout << buf << " ";
				if (strcmp(buf, "down") == 0) {
					memset(buf, 0, sizeof(buf));
					strncpy(buf, "Link Down", 10);
					n = write(fd, buf, sizeof(buf));
				}

				//}

				//The up - count(number of times the interface has been up) ¨C carrier_up_count
				fileP = filePath + "/carrier_up_count";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error: " << strerror(errno) << endl;
				}
				else {
					cout << "up_count:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << " ";
				}

				//The down - count(number of times the interface has been down) ¨C carrier_down_count
				fileP = filePath + "/carrier_down_count";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "down_count:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << endl;
				}

				//The number of received bytes ¨C rx_bytes
				fileP = filePath + "/statistics/rx_bytes";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "rx_bytes:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << " ";
				}

				// rx_dropped
				fileP = filePath + "/statistics/rx_dropped";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "rx_dropped:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << " ";
				}

				//rx_errors
				fileP = filePath + "/statistics/rx_errors";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "rx_errors:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << " ";
				}

				// rx_packets
				fileP = filePath + "/statistics/rx_packets";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "rx_packets:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << endl;
				}

				// tx_bytes
				fileP = filePath + "/statistics/tx_bytes";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "tx_bytes:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << " ";
				}

				// tx_dropped
				fileP = filePath + "/statistics/tx_dropped";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "tx_dropped:";
					do {
						read(rx, &c, 1);
						if (c != '\n')
							cout << c;
					} while (c != '\n');
					cout << " ";
				}

				// tx_errors
				fileP = filePath + "/statistics/tx_errors";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "tx_errors:";
					do {
						read(rx, &c, 1);
						if(c != '\n')
						cout << c;
					} while (c != '\n');
					cout << " ";
				}

				// tx_packets
				fileP = filePath + "/statistics/tx_packets";
				t = fileP.c_str();
				rx = open(t, O_RDONLY);
				if (rx == -1) {
					cout << "error:" << strerror(errno) << endl;
				}
				else {
					cout << "tx_packets:";
					do {
						read(rx, &c, 1);
						cout << c;
					} while (c != '\n');
					cout << endl;
				}
				//Step 4: Monitoring
				close(rx);
				memset(buf, 0, sizeof(buf));
				strncpy(buf, "Monitoring", 11);
				n = write(fd, buf, sizeof(buf));
				if (n < 0) {
					cout << "client: " << strerror(errno) << endl;
				}
			

			}
		}
		//Step 5: Shutdown
		else if (strcmp(buf, "Shut Down") == 0) {
			cout << "This client is quitting" << endl;
			isRunning = false;
			memset(buf, 0, sizeof(buf));
			strncpy(buf, "Done", 5);
			n = write(fd, buf, sizeof(buf));
		}
		else {
			cout << "client: " << strerror(errno) << endl;
			exit(-1);
		}
		memset(buf, 0, sizeof(buf));
	}
	close(fd);

	return 0;
}
