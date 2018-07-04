#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h> // for struct ifreq
#include <linux/if_ether.h>   // for ETH_P_IP/ETH_P_ARP/ETH_P_ALL
#include <netpacket/packet.h> // for struct sockaddr_ll
#include <sys/ioctl.h>  // for SIOCSIFFLAGS... flags

int main(int argc, char *argv[])
{
	int rc = 0, sockfd;
	struct sockaddr_ll sock_address = {0};
	struct ifreq ifr;
	int ifindex;
	size_t if_name_len;

	if (argc < 2) {
		printf("Usage:\n netconf ethx up|down\n");
		return -1;
	}	

	if_name_len = strlen(argv[1]);

	if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
  		perror("listener: socket ");
  		return -1;
	}
	
	if (if_name_len < sizeof(ifr.ifr_name)) {
    		memcpy(ifr.ifr_name, "eth1",if_name_len);
    		ifr.ifr_name[if_name_len]=0;
	} else {
    		printf("interface name is too long\n");
		close(sockfd);
		return -1;
	}

	if (ioctl(sockfd,SIOCGIFINDEX,&ifr)==-1) {
    		printf("ioctl failed:%s\n",strerror(errno));
		close(sockfd);
		return -1;
	}

	sock_address.sll_family = AF_PACKET;
	sock_address.sll_protocol = htons(ETH_P_ALL);
	sock_address.sll_ifindex = ifr.ifr_ifindex;

	if (bind(sockfd, (struct sockaddr*) &sock_address, sizeof(struct sockaddr_ll)) < 0) {
		perror("bind failed\n");
		close(sockfd);
		return -4;
	}
		
	// bring up/down the ethernet interface
	if (!strcasecmp(argv[2], "up"))
		ifr.ifr_flags |= IFF_UP;
	else if (!strcasecmp(argv[2], "down"))
		ifr.ifr_flags &= ~IFF_UP;
	else {
		printf("invalid argument:%s\n", argv[2]);
		return -1;
	}	

	if (ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
		perror("ioctl failed ");
		close(sockfd);
		return -1;
	}

	printf("%s is %s\n", argv[1], !strcasecmp(argv[2], "up") ? "up" : "down");

	close(sockfd);

	return rc;
}
