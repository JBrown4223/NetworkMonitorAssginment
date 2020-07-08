#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <iomanip>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;
char socket_path[] = "/tmp/a1";
char tempFile[] = "/tmp/temp";
const int BUFFER_SIZE = 256;
bool isParent = true;
pid_t* childPid;
bool isOnline;

//implementation of signal handler to exit program gracefully
static void signalHandler(int signum) {
	switch (signum) {
        case SIGINT:
            cout << "Program has been closed" << endl;
            isOnline = false;
            break;
        default:
            cout << "Program has been closed" << endl;
	}
}

int main(int argc, char *argv[]) {
	int fd;
	struct sockaddr_un addr;
	char buffer[BUFFER_SIZE];
	int fdm, max_fd, *interfaceClient, ret, rc;
	int numRead = 0;
	fd_set active_fd_set;
	fd_set read_fd_set;
	char c;
	DIR *dp;
	int n = 0, i = 0;
	struct dirent *ep;

	//Set up a signal handler to terminate the program gracefully
	struct sigaction terminate;
	terminate.sa_handler = signalHandler;
	sigemptyset(&terminate.sa_mask);
	terminate.sa_flags = 0;
	sigaction(SIGINT, &terminate, NULL);

    //opens the directory for the file to be read
	dp = opendir("/sys/class/net/");
	if (dp != NULL) {
		while (ep = readdir(dp))
			n++;
		(void)closedir(dp);
	} else perror("Cannot open the directory");

    //Creating child pids
	childPid = new pid_t[n];
	interfaceClient = new int[n];
	memset(&addr, 0, sizeof(addr)); //often used to clear memory

    //creating and binding a socket to a temp socket file
	if ((fdm = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		cout << "Cannot bind socket for master file descriptor: " << strerror(errno) << endl;
		delete[]childPid;
		delete[]interfaceClient;
		exit(-1);
	}

	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(socket_path));
	addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';
	unlink(socket_path);

    //creating and binding a socket to a temp socket file
	if (bind(fdm, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		cout << "Cannot bind socket for file descriptor(s): " << strerror(errno) << endl;
		unlink(socket_path);
		close(fdm);
		delete[]childPid;
		delete[]interfaceClient;
		exit(-1);
	}

    //opens the file descriptor up for reading only, reading into our buffer and 
    //executing interfaceMonitor with the information that's been written inside our buffer
	system("ls /sys/class/net > /tmp/temp");
	fd = open(tempFile, O_RDONLY);
	if (fd == -1) {
		cout << "Cannot open temp file: " << strerror(errno) << endl;
		close(fdm);
		delete[]childPid;
		delete[]interfaceClient;
		exit(-1);
	}
	do {
		memset(buffer, 0, sizeof(buffer));
		numRead = 0;
		do {
			read(fd, &c, 1);
			buffer[numRead] = c;
			numRead++;
		} while (c != '\n'&&numRead < BUFFER_SIZE);
		if (numRead > 1) {
			buffer[numRead - 1] = '\0';
			childPid[i] = fork();
			if (childPid[i] == 0) {
				isParent = false;
				execlp("./interfaceMonitor", "./interfaceMonitor", buffer, NULL);
				cout << "Cannot run interfaceMonitor: " << strerror(errno) << endl;
			}
			i++;
		}
	} while (numRead > 1 && isParent);

    //Accepting all connections based on file descriptors
	if (isParent) {
		if (listen(fdm, n) == -1) {
			cout << "Cannot listen: " << strerror(errno) << endl;
			unlink(socket_path);
			close(fdm);
			delete[]childPid;
			delete[]interfaceClient;
			exit(-1);
		}
        //establishing all connections and adding them to the socket set
		FD_ZERO(&read_fd_set);
		FD_ZERO(&active_fd_set);
		FD_SET(fdm, &active_fd_set);
		max_fd = fdm;       
		isOnline = true;
		i = 0;
        //While online, networkMonitor (parent) will do synchronous reads for input from interfaceMonitor(s), blocking occures
		while (isOnline) {
			read_fd_set = active_fd_set;
			ret = select(max_fd + 1, &read_fd_set, NULL, NULL, NULL);//Select from up to max_fd+1 sockets
			if (ret == -1) {
				cout << "Cannot read input: " << strerror(errno) << endl;
			}
			else {
                //Accepting new connections to the master socket, and adding them to the set
				if (FD_ISSET(fdm, &read_fd_set)) {
					interfaceClient[i] = accept(fdm, NULL, NULL);
					if (interfaceClient[i] < 0) {
						cout << "Cannot add new connection: " << strerror(errno) << endl;
					}
					else {
						FD_SET(interfaceClient[i], &active_fd_set);
						if (max_fd < interfaceClient[i]) max_fd = interfaceClient[i];
						++i;
					}
				}
				else {
                //Looking for input from interfaceMonitor(s), with a sleep(1) delay
                    for (int j = 0; j < i; ++j) {
					if (FD_ISSET(interfaceClient[j], &read_fd_set)) {
						memset(buffer, 0, sizeof(buffer));
						rc = read(interfaceClient[j], buffer, sizeof(buffer));
						if (rc < 0) {
							cout << "Cannot read input from interfaceMonitor: " << strerror(errno) << endl;
						}
						else if (strcmp(buffer, "Ready") == 0) {
							send(interfaceClient[j], "Monitor", 8, 0);
							sleep(1);
						}
						else if (strcmp(buffer, "Monitoring") == 0) {
							send(interfaceClient[j], "Monitor", 8, 0);
							sleep(1);
						}
						else if (strcmp(buffer, "Done") == 0) {
							isOnline = false;
							sleep(1);
							}
						}
					}
				}
			}
		}
	}

    //terminates interfaceMonitors by sending them the input of 'Shut Down'
	for (int i = 0; i < n; i++) {
		send(interfaceClient[i], "Shut Down", 10, 0);
		sleep(1);
		FD_CLR(interfaceClient[i], &active_fd_set);
		close(interfaceClient[i]);
	}

    //clean ups for any potential loose resources
	unlink(socket_path);
	close(fdm);
	close(fd);
	delete[]childPid;
	delete[]interfaceClient;
	return 0;
}