#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> 
/* Cabeçalhos para socket_ em Linux */
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORTA 7778

/* Estrutura servidor local */
struct sockaddr_in local;
/* Estrutura cliente remoto */
struct sockaddr_in remoto;

int main(int argc, char **argv)
{
	/* Descritor para comunicação socket_ */
	int socket_;
	int client_descritor;

	int size;
	char buffer[4096];
	char nome_arquivo[400];

	/* socket_ sobre o TCP - MAN socket_ */
	socket_= socket(AF_INET, SOCK_STREAM, 0);

	if((socket_ == -1)) {

		perror("ERRO: socket_ Descritor!\n");
		exit(EXIT_FAILURE);
	}
	else {
		printf("Serviço de socket_ inicializado!\n");
	}

	/* Prenchendo a estrutura de dados do servidor */
	local.sin_family= AF_INET;
	local.sin_port= htons(PORTA); //Converte portas para endereço de rede

	memset(local.sin_zero, 0x0, 8);

	/* Função bind, utilizada somente no servidor do socket_ - MAN BIND */
	int bind_server= bind(socket_, (struct sockaddr*) &local, sizeof(local));

	if(bind_server == -1) {

		perror("ERRO: Bind Servidor!\n");
		exit(EXIT_FAILURE);
	}

	/* Colocando a porta em escuta - Definido 1 conexão por vez - MAN LISTEN */
	listen(socket_, 1);

	int size_remoto= sizeof(remoto);

	/* Recebendo conexão - MAN ACCEPT */
	client_descritor= accept(socket_, (struct sockaddr*) &remoto, &size_remoto);

	if(client_descritor == -1) {

		perror("ERRO: Accept Servidor!\n");
		exit(EXIT_FAILURE);
	}

	/* Enviando mensagem ao cliente */
	strcpy(buffer, "Bem Vindo!");

	FILE *arquivo;

	size = recv(client_descritor, nome_arquivo, sizeof(nome_arquivo),0); //recebe nome do arquivo
	nome_arquivo[size] = '\0';

	if((arquivo = fopen(nome_arquivo, "wb")) == NULL) //escreve em modo binario
    {
        perror("Erro na abertura do arquivo");
        exit(1);    
    }

	int n;
	int aux;

	/* Função que recebe o arquivo em pedaços de 4096 bytes e vai */
	/* esqrevendo em um arquivo */
	if(send(client_descritor, buffer, strlen(buffer), 0))
	{
		printf("Esperando mensagem do cliente...\n");
		/* Função de recebimento read*/
		while(1)
		{
			//Limpando o Buffer antes de receber a mensagem
			memset(buffer, 0x0, 4096);
			n = recv(client_descritor,buffer,4096,0);
			buffer[n]= '\0';   //para nao aparecer caracteres estranhos na saida
			
			//se n for 0 o cliente terminou a conexao
			if(n==0){
				close(client_descritor);
				break;
			}
			if(n < 0) 
				perror("ERRO ao ler do socket:");
			//escreve buffer q acabou de receber no arquivo
			//aux = fputs(buffer,arquivo);
		    aux = fwrite(buffer,1, n,arquivo);
		    if(aux < 0) 
		    	perror("ERRO aro escrever no arquivo:");

		    n = send(client_descritor,"Recebendo arquivo...",30,0);
		    if(n < 0) 
		    	perror("ERROR ao escrever no socket:");
		}
	}

	fclose(arquivo);
	close(socket_);
	printf("Serviço de socket_ finalizado!\n");

	return EXIT_SUCCESS;
}
