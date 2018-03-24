#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> 
#include <unistd.h>
/* Cabeçalhos para socket_ em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTA 7777
#define ADDRESS "127.0.0.1"

struct sockaddr_in remoto;

int main(int argc, char **argv)
{
	/* Descritor para comunicação socket_ */
	if(argc != 2)
	{
		printf("Erro \n");
		printf("Uso correto: './cliente nomedoarquivo.tipo \n");
		return 0;
	}
	int socket_;

	int size;
	char buffer[4096];

	printf("Cliente do serviço de socket_!\n");

	/* A maioria das funções do cliente é semelhante as funções do servidor */
	socket_= socket(AF_INET, SOCK_STREAM, 0);

	if((socket_ == -1)) {

		perror("ERRO: socket_!\n");
		exit(EXIT_FAILURE);
	}

	/* Prenchendo a estrutura de dados  */
	remoto.sin_family= AF_INET;
	remoto.sin_port= htons(PORTA); //Converte portas para endereço de rede
	remoto.sin_addr.s_addr= inet_addr(ADDRESS); //Endereço do Servidor

	memset(remoto.sin_zero, 0x0, 8);

	int size_remoto= sizeof(remoto);

	/* Conectando a conexão - MAN CONNECT*/
	connect(socket_, (struct sockaddr*) &remoto, size_remoto);

	/* Arquivo para mandar */
	FILE *arquivo;
	unsigned long fsize; //tamanho do arquivo

	arquivo = fopen(argv[1], "rb");  //lendo em modo binario 
	if (arquivo == NULL) 
	{
	    printf("Arquivo nao encontrado!\n");
	    return 1;
	}
	else 
	{
	    printf("Arquivo encontrado: %s\n", argv[1]);

	    fseek(arquivo, 0, SEEK_END);
	    fsize = ftell(arquivo);
	    rewind(arquivo);

	    printf("Arquivo contem %ld bytes!\n", fsize);
	    printf("Enviando agora...\n");
	}

    if(send(socket_, argv[1] , strlen(argv[1]) , 0) < 0)  //envia nome do arquivo
    {
        printf("Send failed\n");
        return 1;
    }

	/* Função que quebra arquivo em pedacos de 4096 bytes e envia um por um */
	while(1)
	{
		if((size = recv(socket_, buffer, 4096, 0)) > 0)
		{
			buffer[size]= '\0';
			printf("Mensagem do servidor: %s\n", buffer);
		}

		//le arquivo até 4096 bytes e coloca num buffer		
		int bytes_lido = fread(buffer, sizeof(char),sizeof(buffer), arquivo);
		if (bytes_lido == 0) // terminou de ler do arquivo
        	break;

        if (bytes_lido < 0) 
	    {
	        perror("Erro ao ler do arquivo:"); 
	    }

	    void *p = buffer;   //p vai servir para localizar onde estamos no buffer
	    while(bytes_lido > 0)  //send/write pode nao mandar todo o buffer por isso esse while para mandar até que o buffer zere
	    {
	    	//envia o buffer com 4096 bytes
	    	int bytes_enviado = send(socket_, buffer, bytes_lido,0);  
	    	printf("Mandando arquivo\n");
	        if (bytes_enviado <= 0) 
	        {
	            perror("ERROR writing to socket\n");
	        }
	        bytes_lido -= bytes_enviado;
	        p += bytes_enviado;
	    }
		
		memset(buffer, 0x0, 4096);
	}

	fclose(arquivo);
	close(socket_);
	printf("\nCliente finalizado!\n");

	return EXIT_SUCCESS;
}
