/*
 * proxy.c - ICS Web proxy
 * ID: 517030910183
 * NAME: Hou Zhengtong
 */

#include "csapp.h"
#include <stdarg.h>
#include <sys/select.h>

/*
 * Function prototypes
 */
void *thread(void *vargp);
int parse_uri(char *uri, char *target_addr, char *path, char *port);
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr, char *uri, size_t size);
void doit(int fd, struct sockaddr_in *clientaddr);
int build_request(char *request, rio_t *rio, char *method, char *uri, char *version, int *request_body);
int build_response(char *response, rio_t *rio, int *response_body);

/* return after printing a warning message when I/O fails */
ssize_t Rio_readnb_w(rio_t *rp, void *usrbuf, size_t n);
ssize_t Rio_readlineb_w(rio_t *rp, void *usrbuf, size_t maxline);
ssize_t Rio_writen_w(int fd, void *usrbuf, size_t n);

struct threadfd {
    int connfd;
    struct sockaddr_in clientaddr;
};

sem_t mutex;
/*
 * main - Main routine for the proxy program
 */
int main(int argc, char **argv)
{
    /* Check arguments */
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(0);
    }

    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    struct threadfd *vargp;
    pthread_t tid;
    Sem_init(&mutex, 0, 1);

    listenfd = Open_listenfd(argv[1]);

    /* Start listen for the connection */
    while (1)
    {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        
        vargp = Malloc(sizeof(struct threadfd));
        vargp->connfd = connfd;
        vargp->clientaddr = *(struct sockaddr_in *)&clientaddr;
        
        Pthread_create(&tid, NULL, thread, vargp);
    }

    Close(listenfd);
    exit(0);
}

/*
 * parse_uri - URI parser
 *
 * Given a URI from an HTTP proxy GET request (i.e., a URL), extract
 * the host name, path name, and port.  The memory for hostname and
 * pathname must already be allocated and should be at least MAXLINE
 * bytes. Return -1 if there are any problems.
 */
int parse_uri(char *uri, char *hostname, char *pathname, char *port)
{
    char *hostbegin;
    char *hostend;
    char *pathbegin;
    int len;

    if (strncasecmp(uri, "http://", 7) != 0)
    {
        hostname[0] = '\0';
        return -1;
    }

    /* Extract the host name */
    hostbegin = uri + 7;
    hostend = strpbrk(hostbegin, " :/\r\n\0");
    if (hostend == NULL)
        return -1;
    len = hostend - hostbegin;
    strncpy(hostname, hostbegin, len);
    hostname[len] = '\0';

    /* Extract the port number */
    if (*hostend == ':')
    {
        char *p = hostend + 1;
        while (isdigit(*p))
            *port++ = *p++;
        *port = '\0';
    }
    else
    {
        strcpy(port, "80");
    }

    /* Extract the path */
    pathbegin = strchr(hostbegin, '/');
    if (pathbegin == NULL)
    {
        pathname[0] = '\0';
    }
    else
    {
        pathbegin++;
        strcpy(pathname, pathbegin);
    }

    return 0;
}

/*
 * format_log_entry - Create a formatted log entry in logstring.
 *
 * The inputs are the socket address of the requesting client
 * (sockaddr), the URI from the request (uri), the number of bytes
 * from the server (size).
 */
void format_log_entry(char *logstring, struct sockaddr_in *sockaddr,
                      char *uri, size_t size)
{
    time_t now;
    char time_str[MAXLINE];
    unsigned long host;
    unsigned char a, b, c, d;

    /* Get a formatted time string */
    now = time(NULL);
    strftime(time_str, MAXLINE, "%a %d %b %Y %H:%M:%S %Z", localtime(&now));

    /*
     * Convert the IP address in network byte order to dotted decimal
     * form. Note that we could have used inet_ntoa, but chose not to
     * because inet_ntoa is a Class 3 thread unsafe function that
     * returns a pointer to a static variable (Ch 12, CS:APP).
     */
    host = ntohl(sockaddr->sin_addr.s_addr);
    a = host >> 24;
    b = (host >> 16) & 0xff;
    c = (host >> 8) & 0xff;
    d = host & 0xff;

    /* Return the formatted log entry string */
    sprintf(logstring, "%s: %d.%d.%d.%d %s %zu", time_str, a, b, c, d, uri, size);
}

void *thread(void *vargp)
{
    Pthread_detach(Pthread_self());
    struct threadfd *varg = (struct threadfd *)vargp;
    int connfd = varg->connfd;
    struct sockaddr_in clientaddr = varg->clientaddr;
    doit(connfd, &clientaddr);
    Free(vargp);
    Close(connfd);
    return NULL;
}

int build_request(char *request, rio_t *rio, char *method, char *uri, char *version, int *request_body)
{
    int n;
    char buf[MAXLINE];
    sprintf(request, "%s /%s %s\r\n", method, uri, version);

    while((n = Rio_readlineb_w(rio, buf, MAXLINE)) > 0){
        if(strncasecmp(buf, "Content-Length", 14) == 0){
            *request_body = atoi(buf + 15);
        }

        sprintf(request, "%s%s", request, buf);

        if(!strcmp(buf, "\r\n")){
            break;
        }
    }
    if(n <= 0){
        return 0;
    }

    return 1;

}

int build_response(char *response, rio_t *rio, int *response_body)
{
    int n;
    char buf[MAXLINE];

    while((n = Rio_readlineb_w(rio, buf, MAXLINE)) > 0){
        if(strncasecmp(buf, "Content-Length", 14) == 0){
            *response_body = atoi(buf + 15);
        }

        sprintf(response, "%s%s", response, buf);

        if(!strcmp(buf, "\r\n")){
            break;
        }
    }
    if(n <= 0){
        return 0;
    }

    return 1;
}

void doit(int fd, struct sockaddr_in *clientaddr)
{
    int serverfd;
    rio_t client_rio, server_rio;
    int requestbody_size = 0, responsebody_size = 0, responseheader_size = 0;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], pathname[MAXLINE], port[MAXLINE];
    char request[MAXLINE], response[MAXLINE], logstring[MAXLINE];

    /* read request line and headers */
    Rio_readinitb(&client_rio, fd);
    if(Rio_readlineb_w(&client_rio, buf, MAXLINE) == 0){
        return;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    if (parse_uri(uri, hostname, pathname, port) != 0)
    {
        return;
    }

    /* build request header from cilent */
   if(build_request(request, &client_rio, method, pathname, version, &requestbody_size) == 0){
        return;
    }

    /* send request header to server */
    serverfd = Open_clientfd(hostname, port);
    Rio_readinitb(&server_rio, serverfd);

    if(Rio_writen_w(serverfd, request, strlen(request)) == 0){
        Close(serverfd);
        return;
    }

    /* send request body to server */
    if(requestbody_size > 0){
        int n, tmp = requestbody_size;
        while(tmp > 0){
            n = Rio_readnb_w(&client_rio, buf, 1);
            if(n <= 0 && tmp != 0){
                Close(serverfd);
                return;
            }
            if(Rio_writen_w(serverfd, buf, 1) != 1){
                Close(serverfd);
                return;
            }
            tmp--;
        }
    }

    /* build response header and send to cilent */
    if(build_response(response, &server_rio, &responsebody_size) == 0){
        Close(serverfd);
        return;
    }
    responseheader_size = strlen(response);
    if (Rio_writen_w(fd, response, responseheader_size) == 0)
    {
        Close(serverfd);
        return;
    }

    /* get response body and send to client */
    if(responsebody_size > 0){
        int n, tmp = responsebody_size;
        while(tmp > 0){
            n = Rio_readnb_w(&server_rio, response, 1);
            if(n <= 0 && tmp != 0){
                Close(serverfd);
                return;
            }
            if(Rio_writen_w(fd, response, 1) != 1){
                Close(serverfd);
                return;
            }
            tmp--;
        }
    }


    /* send formatted log entry to client */
    format_log_entry(logstring, clientaddr, uri, responseheader_size + responsebody_size);
    P(&mutex); /* protect printf as required(although I got full score without that) */
    printf("%s\n", logstring);
    V(&mutex);
    Close(serverfd);
}

ssize_t Rio_readnb_w(rio_t *rp, void *userbuf, size_t n)
{
    ssize_t rw;

    if ((rw = rio_readnb(rp, userbuf, n)) < 0)
    {
        fprintf(stderr, "Readnb error\n");
        rw = 0;
    }

    return rw;
}

ssize_t Rio_readlineb_w(rio_t *rp, void *userbuf, size_t maxlen)
{
    ssize_t rw;
    
    if((rw = rio_readlineb(rp, userbuf, maxlen)) < 0 ){
        fprintf(stderr, "Readlineb error\n");
        rw = 0;
    }
    return rw;
}

ssize_t Rio_writen_w(int fd, void *userbuf, size_t n)
{
    ssize_t rw;

    if ((rw = rio_writen(fd, userbuf, n)) != n){
        fprintf(stderr, "Writen error\n");
        rw = 0;
    }
    return rw;
}
