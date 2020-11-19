#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <queue>

#define MAXTHREAD 100

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
  int currthread;
		
} AddressObj;

static pthread_mutex_t lock;
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
static pthread_cond_t startTurn = PTHREAD_COND_INITIALIZER;
static int currentTally = 0;
static int threadno = 0;
static int startTally = 0;
static int startno = 0;
static std::deque<AddressObj> res;

Address networkAddress(Address ip, Address subnet) {  
  Address network;
  for (int i = 0; i < 4; i++)
    network.val[i] = ip.val[i] & subnet.val[i];
  return network;
}

Address broadcastAddress(Address ip, Address subnet) {
  Address broadcast;
  for (int i = 0; i < 4; i++)
    broadcast.val[i] = ip.val[i] | (~subnet.val[i] & 255);
  return broadcast;
}

Address copyAddress(Address orig) {
  Address copy;
  for (int i = 0; i < 4; i++)
    copy.val[i] = orig.val[i];
  return copy;
}

AddressObj copyAddressObj(AddressObj orig) {
  AddressObj copy;
  copy.ip = copyAddress(orig.ip);
  copy.subnet = copyAddress(orig.subnet);
  copy.network = copyAddress(orig.network);
  copy.broadcast = copyAddress(orig.broadcast);
  copy.minHost = copyAddress(orig.minHost);
  copy.maxHost = copyAddress(orig.maxHost);
  copy.hosts = orig.hosts;
  copy.currthread = orig.currthread;
  return copy;
}

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

void* netCalculator(void *adrObj_void_ptr) {

  AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
  adrObj->network = networkAddress(adrObj->ip, adrObj->subnet);
  adrObj->minHost = copyAddress(adrObj->network);
  ++adrObj->minHost.val[3]; 

  return NULL;
}

void* broadCalculator(void *adrObj_void_ptr) {
  
  AddressObj *adrObj = (AddressObj *) adrObj_void_ptr;
  adrObj->broadcast = broadcastAddress(adrObj->ip, adrObj->subnet);
  adrObj->maxHost = copyAddress(adrObj->broadcast);
  --adrObj->maxHost.val[3];

  return NULL;
}

void* addressCalculator(void *intPtr) {

  AddressObj addrObj;

  pthread_mutex_lock(&lock);
  int check = startno++;
  
  while (check != startTally) {
    pthread_cond_wait(&startTurn, &lock);
  }

  addrObj = res.front(); 
  res.pop_front();
  addrObj.currthread = threadno++;
  startTally++;
  pthread_cond_broadcast(&startTurn);
  pthread_mutex_unlock(&lock);

  addrObj.hosts = numberOfHosts(addrObj.subnet);

  pthread_t netthread;
  pthread_t broadthread;

  if ((pthread_create(&netthread, NULL, netCalculator, &addrObj)) < 0) {
    printf("ERROR on thread\n");
    exit(0);
  }

  pthread_join(netthread, NULL);

  if ((pthread_create(&broadthread, NULL, broadCalculator, &addrObj)) < 0) {
    printf("ERROR on thread\n");
    exit(0);
  }

  pthread_join(broadthread, NULL);

  pthread_mutex_lock(&lock);
  while (addrObj.currthread != currentTally) {
    pthread_cond_wait(&waitTurn, &lock);
  }

  res.push_back(addrObj);
  currentTally++;
  pthread_cond_broadcast(&waitTurn);
  pthread_mutex_unlock(&lock);

	return NULL;
}

void parseLine(std::string const &str, const char split, std::vector<std::string> &tokens) {
	size_t start;
	size_t end = 0;

	while ((start = str.find_first_not_of(split, end)) != std::string::npos) {
		end = str.find(split, start);
		tokens.push_back(str.substr(start, end - start));
	}
}

Address vectToAddress(std::vector<std::string> vect) {
	Address ret;
	ret.val[0] = std::stoi(vect[0]);
	ret.val[1] = std::stoi(vect[1]);
	ret.val[2] = std::stoi(vect[2]);
	ret.val[3] = std::stoi(vect[3]);
	return ret;
}

int IPcalculator(int n) {
	pthread_t tid[MAXTHREAD];
	std::string line;
	int i = 0;

	while (std::cin >> line) {
		std::vector<std::string> adr;
		std::vector<std::string> ip;
		std::vector<std::string> subnet;

		parseLine(line, '.', ip);
		std::cin >> line;
		parseLine(line, '.', subnet);	

    pthread_mutex_lock(&lock);
    AddressObj next;
    next.ip = vectToAddress(ip);
		next.subnet = vectToAddress(subnet);
    res.push_back(next);
    pthread_mutex_unlock(&lock);
  }

  for (int i = 0; i < n; i++) {
		if (pthread_create(&tid[i], NULL, addressCalculator, &i)) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}
		
	for (int k = 0; k < n; k++) {
		pthread_join(tid[k], NULL);
	}

  return n;
}

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

int main() {

  pthread_mutex_init(&lock, NULL);
  int n;
  std::cin >> n;
  
  IPcalculator(n);

	printf("\n");
	for (int i = 0; i < n; i++) {
    printInfo(res.front());
    res.pop_front();
	}

}

