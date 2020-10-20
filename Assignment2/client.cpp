#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <strings.h>
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

typedef struct ServerObj {
  int portno;
  char* servername;
  AddressObj adrObj;
} ServerObj;

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

void parseLine(std::string const &str, const char split, std::vector<std::string> &tokens) {
  size_t start;
  size_t end = 0;

  while ((start = str.find_first_not_of(split, end)) != std::string::npos) {
    end = str.find(split, start);
    tokens.push_back(str.substr(start, end - start));
  }
}

void* connectToServer(void *servObj_void_ptr) {

  ServerObj *servObj = (ServerObj *) servObj_void_ptr;

  int sockfd, n;
  struct sockaddr_in serv_addr;
    
  int portno = servObj->portno;
  struct hostent* server = gethostbyname(servObj->servername);
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    char msg_open[] = "ERROR opening socket";
    error(msg_open);
  }

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
      
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    char msg_connect[] = "ERROR connecting";
    error(msg_connect);
  }  
   
  n = write(sockfd, &(servObj->adrObj), sizeof(AddressObj));
      
  if (n < 0) {
    char msg_write[] = "ERROR writing to socket";
    error(msg_write);
  }

  n = read(sockfd, &(servObj->adrObj), sizeof(AddressObj));
      
  if (n < 0) {
    char msg_read[] = "ERROR reading from socket";
    error(msg_read);
  }
  
  close(sockfd);
  return NULL;
}

int ipCalculator(ServerObj res[], int portno, char* servername) {
  pthread_t tid[MAXTHREAD];
  std::string line;
  int i = 0;

  while (std::cin >> line) {
    std::vector<std::string> ip;
    std::vector<std::string> subnet;

    parseLine(line, '.', ip);
    std::cin >> line;
    parseLine(line, '.', subnet);  

    res[i].adrObj.ip = vectToAddress(ip);
    res[i].adrObj.subnet = vectToAddress(subnet);
    res[i].portno = portno;
    res[i].servername = servername;

    if (pthread_create(&tid[i], NULL, connectToServer, &res[i])) {
      fprintf(stderr, "Error creating thread\n");
      return 1;
    }

    ++i;
  
  }

  for (int k = 0; k < i; k++)
    pthread_join(tid[k], NULL);

  return i;
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }
        
  int portno = atoi(argv[2]);
  char* servername = argv[1];

  static ServerObj res[MAXTHREAD];
  int n = ipCalculator(res, portno, servername);

  printf("\n");
  for (int i = 0; i < n; i++) {
    printInfo(res[i].adrObj);
  }

}

