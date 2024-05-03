#include "toralize.h"

int main(int argc, char *argv[]) {
    char *host;
    int port, s;
    struct sockaddr_in sock;
    Req *req;
    char buf[ressize];
    Res *res;
    int success;
    char tmp[512];

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return -1;
    }

    host = argv[1];
    port = atoi(argv[2]);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (connect(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("connect");
        return -1;
    }

    printf("Connected to proxy\n");
    req = request(host, port);
    write(s, req, reqsize);

    memset(buf, 0, ressize);
    if (read(s, buf, ressize) < 1) {
        perror("read");
        free(req);
        close(s);
        return -1;
    }

    res = (Res *)buf;
    success = (res->cd == 90);
    if (!success) {
        fprintf(stderr, "Unable to traverse"
                        " the proxy, error code:  %d\n",
                        res->cd);
        free(req);
        close(s);
        return -1;
    }

    printf("Successfully connected through the proxy to %s:%d\n", host, port);

    memset(tmp, 0, 512);
    snprintf(tmp, 511, ""
                       "HEAD / HTTP/1.0\r\n"
                       "Host: www.google.ca/search?q=foo\r\n"
                       "\r\n");

    int sent_bytes = write(s, tmp, strlen(tmp));
    if (sent_bytes < 0) {
        perror("Write failed");
        return -1;
    }

    memset(tmp, 0, 512);
    int received_bytes = read(s, tmp, 511);
    if (received_bytes < 0) {
        perror("Read failed");
        return -1;
    }

    tmp[received_bytes] = '\0';
    printf("'%s'\n", tmp);



    free(req);
    close(s);

    return 0;
}

Req *request(const char* dstip, const int dstport) {
    Req *req;

    req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = htons(dstport);
    req->dstip = inet_addr(dstip);

    strncpy(req->userid, USERNAME, 8);

    return req;
}
