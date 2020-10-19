#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>

#define MAXTHREAD 100

void error(char *msg) {
    perror(msg);
    exit(0);
}

typedef struct Address {
	unsigned int val[4];

} Address;

typedef struct AddressObj {
	Address ip;
	Address subnet;
	Address network;
	Address broadcast;
	Address minHost;
	Address maxHost;
	int hosts;

} AddressObj;

void printAddress(Address adr) {
	printf("%d.%d.%d.%d\n", adr.val[0], adr.val[1], adr.val[2], adr.val[3]);
}

void printInfo(AddressObj adr) {
	printf("IP Address: ");
	printAddress(adr.ip);
	printf("Subnet: ");
	printAddress(adr.subnet);
	printf("Network: ");
	printAddress(adr.network);
	printf("Broadcast: ");
	printAddress(adr.broadcast);
	printf("HostMin: ");
	printAddress(adr.minHost);
	printf("HostMax: ");
	printAddress(adr.maxHost);
	printf("# Hosts: %d\n\n", adr.hosts);
}

Address vectToAddress(std::vector<std::string> vect) {
	Address ret;
	ret.val[0] = std::stoi(vect[0]);
	ret.val[1] = std::stoi(vect[1]);
	ret.val[2] = std::stoi(vect[2]);
	ret.val[3] = std::stoi(vect[3]);
	return ret;
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    AddressObj adrObjOut;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    
    adrObjOut.ip.val[0] = 192;
    adrObjOut.ip.val[1] = 168;
    adrObjOut.ip.val[2] = 1;
    adrObjOut.ip.val[3] = 1;

    adrObjOut.subnet.val[0] = 255;
    adrObjOut.subnet.val[1] = 255;
    adrObjOut.subnet.val[2] = 255;
    adrObjOut.subnet.val[3] = 0;
    
    n = write(sockfd,&adrObjOut,sizeof(AddressObj));
    if (n < 0) 
         error("ERROR writing to socket");
    
    n = read(sockfd,&adrObjOut,sizeof(AddressObj));
    if (n < 0) 
         error("ERROR reading from socket");

    printInfo(adrObjOut);
    return 0;    
}

/*
void parseLine(std::string const &str, const char split, std::vector<std::string> &tokens) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(split, end)) != std::string::npos) {
		end = str.find(split, start);
		tokens.push_back(str.substr(start, end - start));
	}
}


int ipCalculator(AddressObj res[]) {
	pthread_t tid[MAXTHREAD];
	std::string line;
	int n = 0;

	while (std::cin >> line) {
		std::vector<std::string> ip;
		std::vector<std::string> subnet;

		parseLine(line, '.', ip);
		std::cin >> line;
		parseLine(line, '.', subnet);	

		res[i].ip = vectToAddress(ip);
		res[i].subnet = vectToAddress(subnet);
	
		if (pthread_create(&tid[n], NULL,  Function, &res[n])) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}

		++n;
	
	}

	for (int k = 0; k < n; k++)
		pthread_join(tid[k], NULL);

	return n;
}

int main(int argc, char *argv[]) {

	static AddressObj res[MAXTHREAD];
	int n = ipCalculator(res);

	printf("\n");
	for (int i = 0; i < n; i++) {
		printInfo(res[i]);
	}

}
*/
