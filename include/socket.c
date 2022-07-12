#ifndef _SOCKET_C_
#define _SOCKET_C_
#define AF_UNIX 1
#define AF_INET 2
#define AF_INET6 10
#define AF_NETLINK 16

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_CLOEXEC 02000000
#define SOCK_NONBLOCK 04000

#define IPPROTO_TCP 6
#define IPPROTO_UDP 17

struct sockaddr_un
{
	unsigned short int family;
	char sun_path[108];
};
struct sockaddr_in
{
	unsigned short int sin_family;
	unsigned short int sin_port;
	unsigned int sin_addr;
	char pad[8];
};
struct sockaddr_in6
{
	unsigned short int sin6_family;
	unsigned short int sin6_port;
	unsigned int sin6_flowinfo;
	unsigned char sin6_addr[16];
	unsigned int sin6_scope_id;
};
struct sockaddr_nl
{
	unsigned short int nl_family;
	unsigned short int pad;
	unsigned int pid;
	unsigned int groups;
};

#define SIOCGIFCONF 0x8912
#define SIOCGIFADDR 0x8915
#define SIOCSIFADDR 0x8916

struct in6_ifreq
{
	unsigned char ifr6_addr[16];
	unsigned int ifr6_prefixlen;
	int ifr6_ifindex;
};
struct ifreq
{
	char name[16];
	struct sockaddr_in addr;
	char pad[8];
};
struct ifconf
{
	int ifc_len;
	void *ifcu_req;
};

#endif
