#pragma once

#ifdef __linux__
    #include <arpa/inet.h>
#elif defined _WIN32
    #include <winsock2.h>
    #define socklen_t int
#elif defined __APPLE__
    #include <arpa/inet.h>
#endif

#include "config.h"

//Types of DNS resource records
#define T_A 1 // Ipv4 address
#define T_NS 2 // Name server

void dns_request(char* hostname, const char* dns_server, int query_type, int brute);
void change_to_dns_name_format(unsigned char *dns, unsigned char *host);  // www.google.com -> 3www6google3com
u_char *read_name(unsigned char *reader, unsigned char *buffer, int *count);

// https://www2.cs.duke.edu/courses/fall16/compsci356/DNS/DNS-primer.pdf
struct DNS_HEADER {
    unsigned short id; // Identification number
    unsigned char rd :1; // Recursion desired
    unsigned char tc :1; // Truncated message
    unsigned char aa :1; // Authoritative answer
    unsigned char opcode :4; // Purpose of message
    unsigned char qr :1; // Query response flag
    unsigned char rcode :4; // Response code
    unsigned char cd :1; // Checking disabled
    unsigned char ad :1; // Authenticated data
    unsigned char z :1; // Its z! reserved
    unsigned char ra :1; // Recursion available
    unsigned short q_count; // Number of question entries
    unsigned short ans_count; // Number of answer entries
    unsigned short auth_count; // Number of authority entries
    unsigned short add_count; // Number of resource entries
};

// Constant sized fields of query structure
struct QUESTION {
    unsigned short qtype;
    unsigned short qclass;
};

// Constant sized fields of the resource record structure
#pragma pack(push, 1)
struct R_DATA {
    unsigned short type;
    unsigned short _class;
    unsigned int ttl;
    unsigned short data_len;
};
#pragma pack(pop)

// Pointers to resource record contents
struct RES_RECORD {
    unsigned char *name;
    struct R_DATA *resource;
    unsigned char *rdata;
};
