#include "dns_request.h"

/**
 * Perform a DNS query
 */
void dns_request(char* hostname, const char* dns_server, int query_type, int brute) {

    unsigned char *host = (unsigned char*)&hostname[0];

    unsigned char buf[65536], *qname, *reader;
    int i, j, stop;

    struct sockaddr_in a;
    struct RES_RECORD answers[20];
    struct sockaddr_in dest;

    struct DNS_HEADER *dns = NULL;
    struct QUESTION *qinfo = NULL;
    
    #if defined WIN32
        SOCKET s;
        WSADATA wsa;
        WSAStartup(MAKEWORD(2,2), &wsa);
    #elif defined __linux__
        int s;
    #elif defined __APPLE__
        int s;
    #endif

    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //UDP packet for DNS queries

    dest.sin_family = AF_INET; // iPv4
    dest.sin_port = htons(53);// Порт 53
    dest.sin_addr.s_addr = inet_addr(dns_server);

    // Setting a timeout
    int tv_sec = (int) config->timeout;
    int tv_usec = (int) ((config->timeout - tv_sec) * 1000000);
    struct timeval timeout={tv_sec, tv_usec}; // Устанавливаем таймаут
    setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
    setsockopt(s,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(struct timeval));

    // Set the DNS structure for standard queries
    dns = (struct DNS_HEADER *) &buf;

    dns->id = 1;
    dns->qr = 0;
    dns->opcode = 0;
    dns->aa = 0;
    dns->tc = 0;
    dns->rd = 1;
    dns->ra = 0;
    dns->z = 0;
    dns->ad = 0;
    dns->cd = 0;
    dns->rcode = 0;
    dns->q_count = htons(1);
    dns->ans_count = 0;
    dns->auth_count = 0;
    dns->add_count = 0;

    // Pointer to the query portion
    qname = (unsigned char *) &buf[sizeof(struct DNS_HEADER)];
    change_to_dns_name_format(qname, host);
    qinfo = (struct QUESTION *) &buf[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1)];

    qinfo->qtype = htons(query_type); // Type of the query A, NS, etc
    qinfo->qclass = htons(1); // It's internet

    sendto(s, (char *) buf, sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION), 0,
           (struct sockaddr *) &dest, sizeof(dest));

    //Receive the answer
    i = sizeof dest;
    recvfrom(s, (char *) buf, 65536, 0, (struct sockaddr *) &dest, (socklen_t *) &i);

    dns = (struct DNS_HEADER *) buf;

    // DNS_HEADER and the request body ignored
    reader = &buf[sizeof(struct DNS_HEADER) + (strlen((const char *) qname) + 1) + sizeof(struct QUESTION)];

    // Start reading answers
    stop = 0;

    for (i = 0; i < ntohs(dns->ans_count); i++) {
        answers[i].name = read_name(reader, buf, &stop);
        reader = reader + stop;

        answers[i].resource = (struct R_DATA *) (reader);
        reader = reader + sizeof(struct R_DATA);

        if (ntohs(answers[i].resource->type) == 1) // If it's an ipv4 address
        {
            long *p;
            answers[i].rdata = (unsigned char *) malloc(ntohs(answers[i].resource->data_len)+4);

            for (j = 0; j < ntohs(answers[i].resource->data_len); j++) {
                answers[i].rdata[j] = reader[j];
            }

            answers[i].rdata[ntohs(answers[i].resource->data_len)] = '\0';

            reader = reader + ntohs(answers[i].resource->data_len);
            p = (long *) answers[i].rdata;
            a.sin_addr.s_addr = (*p);
            if (brute) {
                printf("\n%s\t %s", inet_ntoa(a.sin_addr), host);
            }
            free(answers[i].rdata);
        } else {
            if (!brute) {
                answers[i].rdata = read_name(reader, buf, &stop);
                reader = reader + stop;
                printf("    %s\n", answers[i].rdata);
                free(answers[i].rdata);
            }
        }
        free(answers[i].name);
    }
}

/**
 * This will convert www.google.com to 3www6google3com
 */
void change_to_dns_name_format(unsigned char *dns, unsigned char *host) {
    int lock = 0, i;
    if (host[strlen((char *) host) - 1] != '.') strcat((char *) host, ".");

    for (i = 0; i < strlen((char *) host); i++) {
        if (host[i] == '.') {
            *dns++ = i - lock;
            for (; lock < i; lock++) {
                *dns++ = host[lock];
            }
            lock++; //or lock=i+1;
        }
    }
    *dns++ = '\0';
}

u_char *read_name(unsigned char *reader, unsigned char *buffer, int *count) {
    unsigned char *name;
    unsigned int p = 0, jumped = 0, offset;
    int i, j;

    *count = 1;
    name = (unsigned char *) malloc(256);

    //read the names in 3www6google3com format
    while (*reader != 0) {
        if (*reader >= 192) {
            offset = (*reader) * 256 + *(reader + 1) - 49152; // 49152 = 2^14 + 2^15
            reader = buffer + offset - 1;
            jumped = 1; // We have jumped to another location -> counting wont go up
        } else {
            name[p++] = *reader;
        }

        reader = reader + 1;

        if (jumped == 0) {
            *count = *count + 1; // We haven't jumped to another location -> we can count up
        }
    }

    name[p] = '\0'; // String complete
    if (jumped == 1) {
        *count = *count + 1; // Number of steps we actually moved forward in the packet
    }

    // Now convert 3www6google3com0 to www.google.com
    for (i = 0; i < (int) strlen((const char *) name); i++) {
        p = name[i];
        for (j = 0; j < (int) p; j++) {
            name[i] = name[i + 1];
            i = i + 1;
        }
        name[i] = '.';
    }
    return name;
}
