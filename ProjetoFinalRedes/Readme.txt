Projeto final de Redes de computadores 1
Professora Michele Nogueira
Alunos: Laion Luis Ferreira        GRR20158911
		Matheus Armani Maldonado   GRR20158597

Modificações implementadas nos arquivos de laboratorio:
ClienteMain:
1-Leitura do arquivo em modo binario, "rb", para qualquer tipo de arquivo;
2-Envia nome do arquivo, que esta em argv[1], passado por parametro na linha de comando;
3-Entra num while que le o arquivo em blocos de 4096 bytes, usando a funcao fread, e para cada bloco lido
  entra num outro while para mandar esse bloco para o servidor, esse while é necessario pois as funcões send ou write
  nem sempre conseguem mandar todos os bytes. No while há um calculo que se usa o valor retornado por fread, que é todo
  o bloco que quer enviar, e o valor retornado por write, se caso nao mande todos os bytes, é o numero de bytes que 
  conseguiu transferir, assim pelo calculo se caso nao enviar todos os bytes o programa vai saber de onde parou e 
  continuar até enviar tudo;
4-Fecha arquivo e socket.

ServidorMain:
1-Recebe o nome do arquivo enviado pelo cliente;
2-Cria arquivo com o nome recebido em modo binario "wb";
3-Entra no while que começa a receber os blocos do arquivo até que o cliente termine a conexão,
  recebe o bloco e escreve no arquivo com a funcao fwrite e manda para o cliente uma mensagem falando que 
  esta recebendo o arquivo.
4-Fecha arquivo e socket.

OBS: Como o protocolo TCP/IP ja lida com acknowledgements/resends/flow control/etc, entao não nos preocupamos 
	 em implementar verificação de erros.

MODO DE USAR:(sempre executar o servidor antes do cliente)

No computador onde quer que seja servidor: 
Para compilar:
		gcc ServidorMain.c -o servidor
Para executar:
		./servidor

NO computador do cliente:  ver ip e colocar no arquivo ClienteMain.c
Para compilar:
		gcc ClienteMain.c -o cliente
Para executar:
		./cliente nomedoarquivo.tipo

obs: nomedoarquivo.tipo, o arquivo precisa estar no diretorio do cliente.




