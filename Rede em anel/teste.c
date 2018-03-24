#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <pthread.h>
#include "fila.h"
 
#define SERVER "192.168.1.3"
#define SERVER2 
#define SERVER3
#define SERVER4 
#define AUXLEN 9999  //Max length of auxfer
#define TAMBUFFER 512
#define PORT 8744 //The port on which to listen for incoming data
 
int tempozera;

struct bastao
{
    int ACK;  //DEUS NAO QUIS PASSAR A PRIMEIRA VARIAVEL DESSA STRUCT
    char origem[140];
    char destino[140];
    int prioridade;
    int eh_bastao;
};

//struct mensagem *temp = malloc(sizeof(struct mensagem));

struct argumentos
{
    struct sockaddr_in server;
    int socket;
};

int esta_com_bastao;  //1 = estao com bastao 0 nao
int passar_bastao; //1 = pode passar 0 nao pode
pthread_mutex_t lock;

void die(char *s)
{
    perror(s);
    exit(1);
}

char *pegar_ip()
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    
    strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);
    
    return(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
}

void *tempo()
{  
    while(1)
    {
        if(esta_com_bastao == 0)
        {
            pthread_mutex_lock(&lock);
            tempozera = 0;
            pthread_mutex_unlock(&lock); 
        }
        else
        {
            sleep(1);
            pthread_mutex_lock(&lock);
            tempozera++;
            pthread_mutex_unlock(&lock); 
        }
    }
}

void *recebe_dados(void *parametros)
{
    struct sockaddr_in server, cliente;
    int s, i, slen=sizeof(server),recv_len;
    char buffer[TAMBUFFER];
    struct mensagem *aux = malloc(sizeof(struct mensagem));
    struct mensagem aux2;
    struct argumentos *args = (struct argumentos*)parametros;
    server = args->server;
    s = args->socket;

    while(1)
    {
        //fflush(stdout); //limpar buffer do teclado
        //recebe dados
        if ((recv_len = recvfrom(s, (struct mensagem*)aux, (1024+sizeof(aux)), 0, (struct sockaddr *) &cliente, &slen)) == -1)
        {
            die("recvfrom()");
        }
        if((strlen(aux->destino) != 0) && (aux->eh_bastao == 0))   
        {
            if(strcmp(pegar_ip(),aux->destino) == 0)
            {   
                //print details of the client/peer and the data received
               // printf("Received packet from %s:%d\n", inet_ntoa(cliente.sin_addr), ntohs(cliente.sin_port));
                printf("%s %s \n" , aux->mensagem,inet_ntoa(cliente.sin_addr));
                memset(aux,'\0', sizeof(*aux));
                aux->eh_bastao = 0;
                //envia meio que um ACK
                usleep(10000);
                if (sendto(s, aux, recv_len, 0, (struct sockaddr*) &cliente, slen) == -1)
                {
                    die("1sendto()");
                }
            }
            else
            {
                strcpy(aux2.mensagem,aux->mensagem);
                strcpy(aux2.origem,aux->origem);
                strcpy(aux2.destino,aux->destino);
                aux2.prioridade = aux->prioridade;
                aux2.eh_bastao = 0;
                
                if (inet_aton(SERVER , &server.sin_addr) == 0) 
                {
                    fprintf(stderr, "inet_aton() failed\n");
                    exit(1);
                }

                if (sendto(s, (struct mensagem*)&aux2, (1024+sizeof(aux2)) , 0 , (struct sockaddr *) &server, sizeof server)==-1)
                {
                    die("2sendto()");
                }
                usleep(100000);
                memset(buffer,'\0', TAMBUFFER);
                //recebe se enviou com sucesso
                if (recvfrom(s, buffer, TAMBUFFER, 0, (struct sockaddr *) &server, &slen) == -1)
                {
                    die("recvfrom()");
                }

                memset(aux,'\0', sizeof(*aux));
                aux->eh_bastao = 0;
                //e envia denovo para avisar que recebeu
                if (sendto(s, aux, recv_len, 0, (struct sockaddr*)&cliente, slen) == -1)
                {
                    die("3sendto()");
                }
            }
        }
        else
        {
                //memset(aux,'\0', TAMBUFFER);                 
                //envia meio que um ACK
                aux->eh_bastao = 1;
                if (sendto(s, aux, recv_len, 0, (struct sockaddr*) &cliente, slen) == -1)
                {
                    die("4sendto()");
                }
        }
    }
}

//CRIAR OUTRO THREAD PARA RECEBER O BASTAO,
//vai precisar de uma variavel global para 
//indicar para a thread de enviar mensagem
//que esta com o bastao 
void *recebe_bastao(void *parametros)
{
    struct sockaddr_in server, cliente;
    int s, i, slen=sizeof(server),recv_len;
    char buffer[TAMBUFFER];
    struct bastao *aux = malloc(sizeof(struct bastao));
    struct bastao aux2;
    struct argumentos *args = (struct argumentos*)parametros;
    server = args->server;
    s = args->socket;
    aux->eh_bastao = 1;
    while(1)
    {
        if(esta_com_bastao != 1)  //se ja estiver com bastao nao precisa receber outro :)
        {
            //se nao estiver com o bastao vai tentar receber o bastao
            if ((recv_len = recvfrom(s, (struct bastao*)aux, (1024+sizeof(aux)), 0, (struct sockaddr *) &cliente, &slen)) == -1)
            {
                die("recvfrom()");
            }
            passar_bastao = 0;
            if((aux->eh_bastao == 1))
            {
                pthread_mutex_lock(&lock);
                tempozera = 0;
                pthread_mutex_unlock(&lock);  
            	printf("\nagr ta com o bastao\n");
                pthread_mutex_lock(&lock);
                esta_com_bastao = 1;
                pthread_mutex_unlock(&lock);           
            }
            memset(aux,'\0', sizeof(*aux));
            aux->eh_bastao = 1;
            //envia meio que um ACK
            if (sendto(s, aux, recv_len, 0, (struct sockaddr*) &cliente, slen) == -1)
            {
                die("aasendto()");
            }
        }//E SE JA TIVER COM O BASTAO:
        if(tempozera > 1)
        {
	        if((passar_bastao == 1) || (tempozera == 10))   //passa bastao para frente se o tempo acabar e se a fila de mensagem acabar
	        {
	        	strcpy(aux2.origem,pegar_ip());
	            strcpy(aux2.destino,SERVER);
	            aux2.eh_bastao = 1;
	            if (inet_aton(SERVER , &server.sin_addr) == 0) 
	            {
	                fprintf(stderr, "inet_aton() failed\n");
	                exit(1);
	            }
	            if (sendto(s, (struct bastao*)&aux2, (1024+sizeof(aux2)) , 0 , (struct sockaddr *) &server, sizeof server)==-1)
	            {
	                die("5sendto()");
	            }
	            //usleep(10000);
	            memset(buffer,'\0', TAMBUFFER);
	            //recebe se enviou com sucesso
	            if (recvfrom(s, buffer, TAMBUFFER, 0, (struct sockaddr *) &server, &slen) == -1)
	            {
	                die("recvfrom()");
	            }       
	            pthread_mutex_lock(&lock);
	            esta_com_bastao = 0;
	            pthread_mutex_unlock(&lock);   
	        }
    	}
    }
}

void *envia_dados(void *parametros)
{
    struct sockaddr_in server;
    int s, slen=sizeof(server),recv_len;
    char buffer[TAMBUFFER];
    char teste[TAMBUFFER];
    struct mensagem message;
    struct mensagem *Itemretirado;
    struct argumentos *args = (struct argumentos*)parametros;
    server = args->server;
    s = args->socket;
    heap_t *h = (heap_t *)calloc(1, sizeof (heap_t));
    while(1)
    {       
        printf("Digite: \n");
        fflush(stdout);
        fgets(teste, TAMBUFFER, stdin);
        sscanf(teste,"%d %s",&message.prioridade, message.mensagem);
        strcpy(message.destino,"192.168.1.3");
        strcpy(message.origem,pegar_ip());
        push(h,message);
        if(esta_com_bastao == 1)
        {
            pthread_mutex_lock(&lock);
            passar_bastao = 0;
            pthread_mutex_unlock(&lock); 
            //enquanto a fila estiver com mensagem e ainda estiver com o bastao
            //continua enviando mensagem
            while((esta_com_bastao != 0) && (h->len != 0))
            {
                Itemretirado = pop(h);
                strcpy(message.mensagem,Itemretirado->mensagem);
                strcpy(message.origem,Itemretirado->origem);
                strcpy(message.destino,Itemretirado->destino);
                message.prioridade = Itemretirado->prioridade;
    			message.eh_bastao = 0; //indica que essa estrutura nao é a do bastao

                //envia a mensagem
                if (sendto(s, (struct mensagem*)&message, (1024+sizeof(message)) , 0 , (struct sockaddr *) &server, slen)==-1)
                {
                    die("7sendto()");
                }
                usleep(1000); //faz com que o programa fique sincronizado        
                memset(buffer,'\0', TAMBUFFER);       
                if (recvfrom(s, buffer, TAMBUFFER, 0, (struct sockaddr *) &server, &slen) == -1)
                {
                    die("recvfrom()");
                }
            }
            //usleep(100000);
            pthread_mutex_lock(&lock);
            passar_bastao = 1;
            pthread_mutex_unlock(&lock);   
        }
    }
}

int main(void)
{
    struct argumentos args;
    struct sockaddr_in server, cliente;
    int s;  
    pthread_t cria_thread,cria_thread2,cria_thread3,cria_thread4;
    
    //cria o UDP socket
    if ((s=socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
     
    //zera a estrutura
    memset((char *) &server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //binda o socket 
    if( bind(s , (struct sockaddr*)&server, sizeof(server) ) == -1)
    {
        die("bind");
    }

    //coloca o endereço da maquina que pode transmitir dados
    if (inet_aton(SERVER , &server.sin_addr) == 0) 
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    //esta_com_bastao = 1;  //só um servidor vai ter isso
    //passar_bastao = 0;
    args.server = server;
    args.socket = s;
    tempozera = 0;
    //cria os processos, um é o server e outro o client
    pthread_mutex_init(&lock, NULL);
    if(pthread_create(&cria_thread, NULL, envia_dados, (void *)&args))
    {
        die("Erro ao criar thread");
    }
    if(pthread_create(&cria_thread2, NULL, recebe_dados, (void *)&args))
    {
        die("Erro ao criar thread");
    }
    if(pthread_create(&cria_thread3, NULL, recebe_bastao, (void *)&args))
    {
        die("Erro ao criar thread");
    }
    if(pthread_create(&cria_thread4, NULL, tempo, NULL))
    {
        die("Erro ao criar thread");
    }
   
    pthread_join( cria_thread, NULL );
    pthread_join( cria_thread2, NULL );
    pthread_join( cria_thread3, NULL );
    pthread_join( cria_thread4, NULL );
    close(s);
    pthread_exit(NULL);
    return 0;
}
