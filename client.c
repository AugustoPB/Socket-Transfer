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
    int clientSocket = 0, n = 0;
    char fileData[1024];
    char fileName[20];
    char fileSz[10];
    struct sockaddr_in serv_addr;
    FILE* file;

    if(argc != 4)
    {
        printf("\n Usage: %s <ip of server> <port> <file name>\n",argv[0]);
        return 1;
    } 

    
    /* Passo 1 - Criar socket */
    if((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket: ");
        exit(1);
    } 

    /* Passo 2 - Configura struct sockaddr_in */
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2])); 
    /* converte Ip em formato string para o formato exigido pela struct sockaddr_in*/
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        exit(1);
    } 

    /* Passo 3 - Conectar ao servidor */
    printf("Conectando ao servidor\n");
    if( connect(clientSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       perror("connect: ");
       exit(1);
    } 
    memset(fileName, 0, sizeof(fileName));

   	snprintf(fileName, sizeof(fileName), "%s", argv[3]);

   	printf("Arquivo requerido: %s\n", fileName);

   	send(clientSocket, fileName, sizeof(fileName), 0);

   	memset(fileSz, 0, sizeof(fileSz));

   	recv(clientSocket, fileSz, sizeof(fileSz), 0);

   	int fileSize = atoi(fileSz);

   	printf("Tamanho do arquivo: %d\n", fileSize);

   	file = fopen("copy.txt", "w"); 
    if(NULL == file)
    {
        printf("Não foi possivel criar o arquivo\n");
        perror("");
        close(clientSocket);
        return 1;
    }

    int bytesReceived = 0;
    printf("Copiando arquivo ...\n");

    while(bytesReceived < fileSize)
    {
    	memset(fileData, 0, sizeof(fileData));

    	bytesReceived = bytesReceived + recv(clientSocket, fileData, sizeof(fileData), 0);

    	//printf("%s\n", fileData);

        fwrite(fileData, 1, strlen(fileData), file);
    }

    if(bytesReceived == 0)
    	printf("Não foi possivel copiar o arquivo\n");
    else
    	printf("Arquivo copiado\n");
    fclose(file);



    return 0;
}