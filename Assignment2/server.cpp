#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
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

typedef struct Octet {
	int val[6];

} Octet;

int numberOfZeroBits(int num) {
        int count = 0;
        for (int i = 0; i < 8; i++) {
                if (!(num & 1))
                        ++count;
                num >>= 1;
        }
        return count;
}

int power(int base, int exp) {
        int res = 1;
        while (exp > 0) {
                res *= base;
                --exp;
        }
        return res;
}

int numberOfHosts(Address subnet) {
        int count = 0;
        for (int i = 0; i < 4; i++)
                count += numberOfZeroBits(subnet.val[i]);
        return power(2, count) - 2;
}

void octetTail(Octet* oct) {
	oct->val[2] = oct->val[0] & oct->val[1];
	oct->val[3] = oct->val[0] | (~oct->val[1] & 255);
	oct->val[4] = oct->val[0] & oct->val[1];
	oct->val[4]++;
	oct->val[5] = oct->val[0] | (~oct->val[1] & 255);
	oct->val[5]--;
}

void octetNormal(Octet* oct) {
	oct->val[2] = oct->val[0] & oct->val[1];
	oct->val[3] = oct->val[0] | (~oct->val[1] & 255);
	oct->val[4] = oct->val[0] & oct->val[1];
	oct->val[5] = oct->val[0] | (~oct->val[1] & 255);
}


void addressCalculator(AddressObj* adrObj) {
	Octet oct[4];

	for (int i = 0; i < 4; i++) {
		oct[i].val[0] = adrObj->ip.val[i];
		oct[i].val[1] = adrObj->subnet.val[i];
		if (i != 3) {
			octetNormal(&oct[i]);
		} else {
			octetTail(&oct[i]);
		}
	}

	for (int i = 0; i < 4; i++) {
		adrObj->network.val[i] = oct[i].val[2];
		adrObj->broadcast.val[i] = oct[i].val[3];
		adrObj->minHost.val[i] = oct[i].val[4];
		adrObj->maxHost.val[i] = oct[i].val[5];
	}

	adrObj->hosts = numberOfHosts(adrObj->subnet);
}

void processConnection(int sock) {

       int n;
       AddressObj adrObjIn;

       n = read(sock, &adrObjIn, sizeof(AddressObj));
       if (n < 0) error("ERROR reading from socket");

       addressCalculator(&adrObjIn);

       n = write(sock, &adrObjIn, sizeof(AddressObj));
       if (n < 0) error("ERROR writing to socket");

}

void fireman(int signum) {
	while (waitpid(-1, NULL, WNOHANG) > 0)
		std::cout << "A child process ended" << std::endl;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd, 32);
     clilen = sizeof(cli_addr);
     
     signal(SIGCHLD, fireman);
     while (true) {

       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
       if (newsockfd < 0) 
          error("ERROR on accept");
       pid = fork();
       if (pid < 0)
	       error("ERROR on fork");
       if (pid == 0) {
         close(sockfd);
         processConnection(newsockfd);
	 exit(0);
       } else {
         close(newsockfd);
       }
     }
     close(sockfd);
     return 0;
}


