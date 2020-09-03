#include <iostream>
#include <string>

class Address {
	public:
	unsigned int val[4];

	Address(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4) {
		val[0] = v1;
		val[1] = v2;
		val[2] = v3;
		val[3] = v4;
	}
};

Address networkAddress(Address ip, Address subnet) {
	unsigned int v1 = ip.val[0] & subnet.val[0];
	unsigned int v2 = ip.val[1] & subnet.val[1];
	unsigned int v3 = ip.val[2] & subnet.val[2];
	unsigned int v4 = ip.val[3] & subnet.val[3];
	return Address(v1, v2, v3, v4);
}

Address broadcastAddress(Address ip, Address subnet) {
	unsigned int v1 = ip.val[0] | (~subnet.val[0] & 255);
	unsigned int v2 = ip.val[1] | (~subnet.val[1] & 255);
	unsigned int v3 = ip.val[2] | (~subnet.val[2] & 255);
	unsigned int v4 = ip.val[3] | (~subnet.val[3] & 255);
	return Address(v1, v2, v3, v4);
}

Address minHostAddress(Address network) {
	return Address(network.val[0], network.val[1], network.val[2], network.val[3] + 1);
}

Address maxHostAddress(Address broadcast) {
	return Address(broadcast.val[0], broadcast.val[1], broadcast.val[2], broadcast.val[3] - 1);
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

void printAddress(Address adr) {
	printf("%d.%d.%d.%d\n", adr.val[0], adr.val[1], adr.val[2], adr.val[3]);
}

void printInfo(Address ip, Address subnet) {
	printf("IP Address: ");
	printAddress(ip);
	printf("Subnet: ");
	printAddress(subnet);
	printf("Network: ");
	Address network = networkAddress(ip, subnet);
	printAddress(network);
	printf("Broadcast: ");
	Address broadcast = broadcastAddress(ip, subnet);
	printAddress(broadcast);
	printf("HostMin: ");
	printAddress(minHostAddress(network));
	printf("HostMax: ");
	printAddress(maxHostAddress(broadcast));
	printf("# Hosts: %d\n", numberOfHosts(subnet));
}

int main() {

	Address ip(192, 168, 100, 24);
	Address subnet(255, 255, 255, 248);
	printInfo(ip, subnet);
}

