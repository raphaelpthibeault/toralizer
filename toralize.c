#include "toralize.h"

int connect(int app_sockfd, const struct sockaddr *app_s, socklen_t app_addrlen) {
    int s;
    struct sockaddr_in sock;
    Req *req;
    char buf[ressize];
    Res *res;
    int success;
    int (*og_connect)(int, const struct sockaddr *, socklen_t);

    og_connect = dlsym(RTLD_NEXT, "connect");
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }

    sock.sin_family = AF_INET;
    sock.sin_port = htons(PROXY_PORT);
    sock.sin_addr.s_addr = inet_addr(PROXY);

    if (og_connect(s, (struct sockaddr *)&sock, sizeof(sock))) {
        perror("connect");
        return -1;
    }

    printf("Connected to proxy\n");
    req = request((struct sockaddr_in *)app_s);
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

    printf("Connected through the proxy.\n");

    dup2(s, app_sockfd);
    free(req);

    return 0;
}

Req *request(const struct sockaddr_in *app_sock) {
    Req *req;

    req = malloc(reqsize);
    req->vn = 4;
    req->cd = 1;
    req->dstport = app_sock->sin_port;
    req->dstip = app_sock->sin_addr.s_addr;

    strncpy((char*)req->userid, USERNAME, 8);

    return req;
}
