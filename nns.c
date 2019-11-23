/* Attenction: p0rk program detected! ***WARNING*** ***WARNING*** ***WARN.........0ink!*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pwd.h>
#include <string.h>

int nscan (unsigned long padip, int nuastart,int nuaend,int ports);
void external_login(unsigned long padip,char login[25],char password[25]);
unsigned long resolve(char *host);

unsigned long resolve(char *host)
{
  struct hostent *he;
  long i;

  
  if((i=inet_addr(host))<0) // E' un ip gia' di suo oppure no? 
      /* Zi' lo e', proviamo a risolverlo */
      if((he=gethostbyname(host))==NULL)
      return(0); // nada, no resolve
    else
      return(*(unsigned long *)he->h_addr); // ok ekko l'ip

  return(i); // ritorno gia' l'ip
}

char *logfile;
int fdr;
int fdw;
char buf[1024];
int s;
int dnic,zbn;
void main(int argc,char **argv)  
{

 	int ports;
        long int nuastart,nuaend,nuadiff;
        char *ph;
        unsigned long padip;
        printf("NaiL NUA Scanner Inet->Cisco/pad v1.00 - nobody everywhere\n");
        /* Meno di 5 parametri ? help!:) */
        if(argc<6) {
            printf("Use: nns <hostname> <number of free x25 channels> <dnic or 0> <starting NUA> <\n");
            printf("Final NUA> [logfile]");
            exit(200);
         }
         /* Ne abibamo addirittuera 6 (il logfile) ? akkiappiamolo in una var a pparte*/
         logfile=malloc(400);
         if(argc>6) { 
               strcpy(logfile,argv[6]);
               printf("Using logfile: %s\n",logfile);
         }  
         else
               logfile=NULL;
         /* Atoi = da stringa a intero*/
        
         ports=atoi(argv[2]);
         zbn = 1;         
         
         if(!strcmp(argv[3],"0")) zbn =0;
         dnic = atoi(argv[3]);

         nuastart = atoi(argv[4]);
         nuaend = atoi(argv[5]);         
         if(nuaend<nuastart) {
                /* Qui li scambio */
                nuadiff = nuaend;
                nuaend=nuastart;
                nuastart=nuadiff;
         }                 
         nuadiff = nuaend - nuastart;
         if( nuadiff < 0 || nuadiff > 65530 )
         {
             printf("Error in NUA difference, please use a start > than a final \n");
             printf("And don't exceed 65530 nuas at a time\n");
             exit(201);
         }
         /* Mi akkiappo l'hostnamukolo  e lo rizolvo*/
         strcpy(ph,argv[1]);

         printf("Resolving pad hostname ...\n");          
         padip=resolve(ph);
         if(padip==0) {
               printf("Unable to resolve pad hostname\n");
               exit(202);
         }
         /* Rikiamo la procedura di zkan */
         ports = nscan(padip,nuastart,nuaend,ports);
         fprintf(stderr,"Totally found nuas: %d\nExiting.\n",ports);
         exit(0);
}

                 
                         	
int nscan (unsigned long padip, int nuastart,int nuaend,int ports)
{
       /* This program is made to pad throught a cisco right now */
       /* So we will ask for a login and a password */
      char login[25];
      char password[25];
      char prmpt[50];
      int i,fn;
      /* Kiedo al tizio davanti al pc ke si fa le pippe mentali kon sto progghie
          di darmi login e password per il cisko */
      printf("Cisco login: ");
      fflush(stdout);
      fgets(login,25,stdin);
      printf("Cisco password: ");
      fflush(stdout);
      fgets(password,25,stdin);
      printf("\nConnecting.........");
      fflush(stdout);
     external_login(padip,login,password);
      if(logfile)
      freopen(logfile,"w",stdout);

      for(i=0; i < strlen(buf); i++) {
                  if(buf[i] == '>') break;
                  prmpt[i]=buf[i];
      }
      prmpt[i]='\0';
      sleep(3);
      fprintf(fdw,"\n"); fflush(fdw);
      sleep(2);           
      fn = 0;
       /* Qui parte la scansione vereppropria */
              fgets(buf,512,fdr);  /* Questo e' il Trying .. bla bla bla */
#ifdef DEBUG
      printf("%s",buf);
#endif

      for(i=nuastart;i<nuaend;i++) {
             if(!zbn) {
                 if (dnic!=0) 
                 fprintf(fdw,"pad %d%d\n",dnic,i);
                 else
                 fprintf(fdw,"pad %d\n",i);
             } 
             else
                 fprintf(fdw,"pad 0%d%d\n",dnic,i);
              fflush(fdw);
              fgets(buf,512,fdr);  /* Questo e' il Trying .. bla bla bla */
#ifdef DEBUG
      printf("%s",buf);
#endif
              fgets(buf,512,fdr);  /* Questo e' il Trying .. bla bla bla */
#ifdef DEBUG
      printf("%s",buf);
#endif
              if((strstr(buf,"pen")!=NULL)) {
                  if(!zbn) {
                 if (dnic!=0) 
                 printf("NUA Found: %d%d\n",dnic,i);
                 else
                 printf("NUA Found: %d\n",i);
             } 
             else
                 printf("NUA Found: 0%d%d\n",dnic,i);

                  close(fdr);
                  close(fdw);
                  shutdown(s,2);
                  external_login(padip,login,password);
                  fn++;
                  continue;
             }else {
                  fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
                  fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
             
           if(!zbn) {
                 if (dnic!=0)
                 printf("NUA Dead: %d%d\n",dnic,i);
                 else
                 printf("NUA Dead: %d\n",i);
             }
             else
                 printf("NUA Dead: 0%d%d\n",dnic,i);
          }
          fflush(stdout);
     }      
            printf("Scan ended!\n");
            return fn;
 }








void external_login(unsigned long padip,char login[25],char password[25])
{
            
      struct sockaddr_in sin;
      sin.sin_family=AF_INET; // Internet TCP/IP
      sin.sin_port = htons(23); // Porta 23 
      sin.sin_addr.s_addr = padip; // Hostname->ip
      s = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); // Kreo il soket
       if((connect(s,(struct sockaddr *)&sin,sizeof(sin)))==-1) { // mi connetto
            printf("Failed!\n"); // non s'e' connesso
            printf("Aborting.......\n");
            exit(203);
      }
      fdr=fdopen(s,"r"); // assegno ai file la scrittura e la lettura sul socket
      fdw=fdopen(s,"w");
      while(1) {
        fgets(buf,512,fdr); // Akkiappo dal socket
#ifdef DEBUG
      printf("%s",buf);
#endif
        if((strstr(buf,"Verification"))!=NULL) // Se trova la stringa verification allora esce  
              break;                           //  dal ciclo altrimenti ripija dal socket
      }

      fprintf(fdw,"\n%s%s\n\n",login,password); fflush(fdw);          
      fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
      fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
      fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
      fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
      fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
      fgets(buf,512,fdr);
#ifdef DEBUG
      printf("%s",buf);
#endif
      if((strstr(buf,">"))==NULL) {
           printf("Login failed!\n");
           exit(205);
      }
      printf("Logged in\n");
      fgets(buf,512,fdr); 
#ifdef DEBUG
      printf("%s",buf);
#endif

}
