#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


int main(int argc, char *argv[])
{
	int serverSocket = 0, clientSocket = 0;
   	struct sockaddr_in serv_addr, client_addr;
   	char fileSz[10];
   	char fileData[1024];
   	char fileName[20];
   	socklen_t sz;

   	FILE *file;
	
	
	/* Passo 1 - Criar Socket */
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("socket: ");
		exit(1);
    }

	/* Passo 2 - Configurar estrutura sockaddr_in */
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 
   
    /* Passo 3 - Associar socket com a estrutura sockaddr_in     */
    if (bind(serverSocket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
   	perror("bind: ");
	exit(1);
	}
	
	/* Passo 4 - Tornar o servidor ativo  */
	
	listen(serverSocket, 10);

	
	while(1)
	{
			/* Passo 5 - Aguardar conexão do cliente.  */
		printf("#################################################\n");
		printf("Esperando conexão\n\n");
    	if ((clientSocket = accept(serverSocket, (struct sockaddr*)&client_addr, &sz)) < 0)
    	{
			perror("accept: ");
			exit(1);
		}
		printf("Cliente: %s:%d conectou\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port);
		
		memset(fileName, 0, sizeof(fileName));

		recv(clientSocket, fileName, sizeof(fileName), 0);

		printf("Acessando: %s\n", fileName);

		file = fopen(fileName,"r");

		if(file == NULL)
		{
			printf("Não foi possivel abrir o arquivo\n");
			perror("");
		}   
		else
		{
			memset(fileSz, 0, sizeof(fileSz));

			int fileSize = filesize(file);
	
			printf("Tamannho do arquivo: %d\n", fileSize);

			snprintf(fileSz, sizeof(fileSz), "%d",fileSize);

			send(clientSocket, fileSz, sizeof(fileSz), 0);

			printf("Enviando arquivo ...\n");

			while(1)
    		{
				memset(fileData, '\0', sizeof(fileData));
	
				int fileLen = fread(fileData, 1, sizeof(fileData)-1, file);

				if(fileLen > 0)
				{
					send(clientSocket, fileData, sizeof(fileData), 0);
				}
				else if(fileLen < sizeof(fileData))
				{
					printf("Arquivo Enviado\n\n");
					fclose(file);
        			break;
				}
			}
		}
		printf("Encerrando conexão\n");
		close(clientSocket);
	}
}


int filesize(FILE* fp){
	int curr = ftell(fp);
	int sz;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return sz;
}
