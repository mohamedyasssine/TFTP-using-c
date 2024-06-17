#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
 char transfer_mode[10];
 //structure de addresse de serveur
 //address ip de serveur 
//
#define MAX 512
#define BUFSIZE 1024
struct sockaddr_in serv_addr;

void erreur_verification(int r,char *message)
{
  if (r < 0) {
        // affichage de l'erreur
        perror(message);
        exit(-1);
    }
}
 void send_ERROR(int socket,struct sockaddr_in cli_addr,int errorCode)
{
    uint8_t end_string = 0;//on va utiliser à la fin de message d'erreur dans la packet
    char buffer[BUFSIZE];
    char error_message[512];
    uint16_t opcode = htons(5);//l'opcode de packet d'erreur est 5
    uint16_t error_code = htons(errorCode);
    memcpy(buffer, &opcode, 2);//copier l'opcode dans la packet error [opcode(2bit)|code d'erreur (2bit)|message d'erreur|0(fin string)]
    memcpy(buffer + 2, &error_code, 2);
    if(error_code==4)//d'aprés le RFC 135
    {
        strcpy(error_message, "Illegal TFTP operation");

    }
    else if(error_code==1)
    {
        strcpy(error_message, "file not found");
    }
    strcpy(buffer + 2, error_message);//copie de la message d'erreur
    memcpy(buffer + strlen(error_message) + 2, &end_string, 1);//
    int sent_len = sendto(socket,
			      buffer,
			      strlen(error_message) + 4,
			      0,
			      (const struct sockaddr *)&cli_addr,
			      sizeof(cli_addr));
    erreur_verification(sent_len,"erreur lors de l'envoi de message d'erreur au client");
}
void send_ACK(int server_socket, uint16_t numero_block,struct sockaddr_in serv_addr)
{
    int longueur=0;
    char buffer[BUFSIZE];
    //l'ocpcode de packet ACK est 4
    uint16_t opcode = htons(4);
    //on affecte l(opcode au tompon de l'indice 0 à 1 (2bit)
    memcpy(buffer, &opcode, 2);
    longueur+=2;
    //par la suite on copie le numéro de block qu'est 2 bits
    numero_block = htons(numero_block);
    memcpy(buffer +longueur,&numero_block,2);
    longueur+=2;
    //
   int sent_len = sendto(server_socket, buffer,BUFSIZE,0,(const struct sockaddr *)&serv_addr,sizeof(serv_addr));
   erreur_verification(sent_len, "erreur d'envoi de ACK");
}
void send_RRQ(int server_socket,struct sockaddr_in addr,char *nom_fichier)
{
        
        int len = 0;//longueur de buffer
        char buffer[BUFSIZE];//le buffer va etre notre trame à transférer ,
                             //il contient [opcode(2bit),nomfichier,0(fin de nomfichier),MOde de transfére,0]
        uint16_t opcode = htons(1);//l'opcode de RRQ est 1 (d'aprés le RFC 135)
        uint8_t end_string = 0;
        memcpy(buffer, &opcode, 2); //copier l'opcode de RRQ d+ans le buffer 
        len+=2;//on ajout la longueur d'opcode au longueur final de buffer
        strcpy(buffer + len, nom_fichier);//on ajoute le nom fichier dans notre buffer(trame)
        len += strlen(nom_fichier);
        memcpy(buffer + len, &end_string, 1);//on ajoute la fin de nom fichier
        len++;
        strcpy(buffer + len, transfer_mode); //on ajoute le mode de transfére dans notre trame de donnée(buffer)    
        len += strlen(transfer_mode);
        memcpy(buffer + len, &end_string, 1);
        len++;
        // Ajout de l'option "bigfile"
        char *bigfile_option = "bigfile";
        char *bigfile_value = "1"; // On active l'option "bigfile"
        strcpy(buffer + len, bigfile_option);
        len += strlen(bigfile_option);
        memcpy(buffer + len, &end_string, 1);
        len++;
        strcpy(buffer + len, bigfile_value);
        len += strlen(bigfile_value);
        memcpy(buffer + len, &end_string, 1);
        len++;
        /*dans la ligne suivante on utilise la fonction sendto de la bib socket.h pour envoyer
        la requette RRQ (buffer) à la serveur en utilisant la socket client 
        et l'addresse de serveur est défini
        dans la structure sockaddr */
        int sendlen=sendto(server_socket, buffer, len, 0, (struct sockaddr*)&addr, sizeof(addr));
        //erreur_verification(sent_len, "erreur d'envoi de RRQ");
    for (int i = 0; i < len; i++) {
        printf("%02X ", buffer[i]); // Affichage du contenu en hexadécimal
    }
    printf("\n");
    
    printf("RRQ packet sent successfully\n");

}
void send_WRQ(int server_socket, char *nom_fichier,struct sockaddr_in addr)
{
        int len = 0;//longueur de buffer
        char buffer[BUFSIZE];//le buffer va etre notre trame à transférer ,
                             //il contient [opcode(2bit),nomfichier,0(fin de nomfichier),MOde de transfére,0]
        uint16_t opcode = htons(2);//l'opcode de WRQ est 2 (d'aprés le RFC 135)
        uint8_t end_string = 0;
        memcpy(buffer, &opcode, 2); //copier l'opcode de WRQ d+ans le buffer 
        len+=2;//on ajout la longueur d'opcode au longueur final de buffer
        strcpy(buffer + len, nom_fichier);//on ajoute le nom fichier dans notre buffer(trame)
        len += strlen(nom_fichier);
        memcpy(buffer + len, &end_string, 1);//on ajoute la fin de nom fichier
        len++;
        strcpy(buffer + len, transfer_mode); //on ajoute le mode de transfére dans notre trame de donnée(buffer)    
        len += strlen(transfer_mode);
        memcpy(buffer + len, &end_string, 1);
        len++;
        // Ajout de l'option "bigfile"
        char *bigfile_option = "bigfile";
        char *bigfile_value = "1"; // On active l'option "bigfile"
        strcpy(buffer + len, bigfile_option);
        len += strlen(bigfile_option);
        memcpy(buffer + len, &end_string, 1);
        len++;
        strcpy(buffer + len, bigfile_value);
        len += strlen(bigfile_value);
        memcpy(buffer + len, &end_string, 1);
        len++;
        /*dans la ligne suivante on utilise la fonction sendto de la bib socket.h pour envoyer
        la requette RRQ (buffer) à la serveur en utilisant la socket client 
        et l'addresse de serveur est défini
        dans la structure sockaddr */
        int sent_len = sendto(server_socket, buffer,len,0,( struct sockaddr *)&addr,sizeof(addr));
        erreur_verification(sent_len, "erreur d'envoi de wRQ");
}
void transferer_DATA(int client_socket ,struct sockaddr_in client_addr,char nom_fichier[512])
{
    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);
    //avec ipv4 et udp comme protocole
    FILE *src_file;
    src_file = fopen(nom_fichier, "r"); //ouvrir le fichier en mode reading 
    if (src_file == NULL)
    {
        printf("erreur d'ouverture de fichier\n envoie de packet d'erreur");
        send_ERROR(client_socket,client_addr,1);
        exit(1);
    }
    else
    {
        //ici on va faire l'opération de l'envoi des data au client
        /*la structure de packet de data est [opcode (2bit)|numéro de block(2bit)|data(n bit)]*/
        char buffer[BUFSIZE];//tompon qui va contenir le packet data 
        memset(buffer, 0, BUFSIZE);

        uint16_t opcode; 
        int send_len; //le longueur de packet data à envoyer 
        size_t client_size = sizeof(client_addr);//longueur de addresse ip de client
        // Déclaration d'un caractère pour stocker temporairement les données du fichier source
        char c;
	    int i = 4;/*on commence par l'indexe 4 car les deux premier bit vont etre l'opcode 
        et les deux bit suivant vont etre le numéro de block(numéro de packet)*/
        uint16_t numero_block = 1;//on va initialiser les numero des block de 1
        /*maintenant on va découper le fichier sur des packet jusqu'a the EOF est trouvé 
        EOF :est le marqueur de fin de fichier 
        */
        do {
            c = fgetc(src_file);//fonction prédefinie permet d'extraire le caractére suivant de ficheir
            if (c != EOF)
		{
			// copier le c dans 
			buffer[i] = c;
			i++;
		}
        if (i == MAX + 4 || c == EOF) {
        /*MAX : la valeur maximum des caractére peut
                                         etre dans la partie data :512+2bit d'opcode+2 bit de nombre de block*/
            opcode = htons(3); //opcode de data est 3
			uint16_t block = htons(numero_block);
			memcpy(buffer, &opcode, 2);
			memcpy(buffer + 2, &block, 2);
            send_len =sendto(client_socket,buffer,i,0,(const struct sockaddr *) &client_addr,sizeof(client_addr));
            erreur_verification(send_len,"erreur lord de l'envoi de packet");
        
        //remetrre le tompon à 0  
        memset(buffer, 0, BUFSIZE);
        /*pour chaque envoi d'un packet de data le serveur doit attendre la packet ACK 
          pour envoyer la prochaine packet data */
        int recv_len = recvfrom(client_socket, (char *)buffer,BUFSIZE,0, (struct sockaddr *)&client_addr,(socklen_t *) & client_size);
        erreur_verification(recv_len,"erreur lor de recevoir la packet ACK");
        memcpy(&block, (uint16_t *) & buffer[2], 2);//on copie la numéro de block contenu dans le tompon da,s ma variable block
		block = ntohs(block);
        if (block != numero_block)
        {
			fclose(src_file);
      shutdown(client_socket, SHUT_RDWR);
      close(client_socket);
			exit(-1);
        }
        //initialisation de l'index de tompon 
        i = 4;
		numero_block++;//incrémentation de numero de packet
        }
    }	while (c != EOF);
    }

    //clear le tompon
	memset(buffer, 0, BUFSIZE);
    //fermeture de fichier
    fclose(src_file);
    /*fermeture de socket*/
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
    exit(0);
}
void put(char *nom_fichier,int client_socket, struct sockaddr_in serv_addr)
{
  char nom_fichier_client[516];
  int longueur_addresse = sizeof(serv_addr);//on va utiliser longueur_addresse par la suite dans la fonction 
  char nom_fichier_serveur[516];
  //on va utiliser la variable opcode pour tester sur la réponse serveur par la suite
        uint16_t opcode;
        //buffer est un tampon pour stocker la réponse du serveur
        char buffer[BUFSIZE];
  send_WRQ(client_socket,nom_fichier,serv_addr);
  //recvfrom est une fonction prédefinit dans la bib socket.h pour récuperer la reponse de serveur 
  int recv_len = recvfrom(client_socket,buffer,BUFSIZE,0,(struct sockaddr *)&serv_addr,(socklen_t *) & longueur_addresse);
  erreur_verification(recv_len,"erreur de recevoi un message aprés qu'on a envoyé WRQ");
  memcpy(&opcode, (uint16_t *) & buffer, 2);
	opcode = ntohs(opcode);
  if (opcode == 5) //cas de packet d'erreur
	{
        /* je vais par la suite afficher le message d'erreur qui contient dans le tompon à partie*/
    fprintf(stderr, "File Not found\n");
    exit(1);
}
  else if(opcode == 6)
  {

        int optionFound = 0; // Indicateur pour l'option "bigfile" trouvée avec valeur "1"
    char *ptr = buffer + 2; // Début de la zone des options (après l'opcode)
    while (ptr < buffer + recv_len) {
        if (strcmp(ptr, "bigfile") == 0) { // Si on trouve l'option "bigfile"
            ptr += strlen(ptr) + 1; // On passe au contenu de l'option
            if (strcmp(ptr, "1") == 0) { // Si la valeur de l'option est "1"
                optionFound = 1; // L'option "bigfile" avec valeur "1" a été trouvée
                break;
            }
        }
        ptr += strlen(ptr) + 1; // Passer à la prochaine option
    }

    if (optionFound) {
        printf("Option 'bigfile' avec valeur '1' trouvée dans OACK.\n");
        // Continuez le traitement avec cette option activée
            transferer_DATA(client_socket,serv_addr,nom_fichier);
    } else {
        printf("Option 'bigfile' avec valeur '1' non trouvée dans OACK.\n");
        // Gérez le cas où l'option "bigfile" n'est pas confirmée par le serveur
        send_ERROR(client_socket,serv_addr,1);

    }

  }
}

void get(char *nom_fichier,int client_socket, struct sockaddr_in serv_addr)
{

        char nom_fichier_serveur[516];
        strcpy(nom_fichier_serveur,nom_fichier);
	    char nom_fichier_client[516];
        //on va utiliser la variable opcode pour tester sur la réponse serveur par la suite
        uint16_t opcode;
        //buffer est un tampon pour stocker la réponse du serveur
        char buffer[BUFSIZE];
        send_RRQ(client_socket,serv_addr,nom_fichier);
        int longueur_addresse = sizeof(serv_addr);//on va utiliser longueur_addresse par la suite dans la fonction 
        //recvfrom est une fonction prédefinit dans la bib socket.h pour récuperer la reponse de serveur 
        int recv_len = recvfrom(client_socket,buffer,BUFSIZE,0,(struct sockaddr *)&serv_addr,(socklen_t *) & longueur_addresse);
        erreur_verification(recv_len,"erreur de recevoi un message aprés qu'on a envoyé RRQ");
        /*ces deux ligne copie l'opcode qui contient dans le buffer 
        dans la variable  opcode en utilisant la méthode opcode*/
        memcpy(&opcode, (uint16_t *) & buffer, 2);
	    opcode = ntohs(opcode);
        /*maintenant on va tester sur l'opcode sachant que
         1:RRQ message
         2 :WRQ
         3:DATA
         4:ACK
         5:ERROR
         */
    if (opcode == 5) //cas de packet d'erreur
	{
        /* je vais par la suite afficher le message d'erreur qui contient dans le tompon à partie*/
    fprintf(stderr, "file not found");	
    exit(1);	
	} 
    else if (opcode == 6)	//cas de packet de oack
    {
        int optionFound = 0; // Indicateur pour l'option "bigfile" trouvée avec valeur "1"
    char *ptr = buffer + 2; // Début de la zone des options (après l'opcode)
    while (ptr < buffer + recv_len) {
        if (strcmp(ptr, "bigfile") == 0) { // Si on trouve l'option "bigfile"
            ptr += strlen(ptr) + 1; // On passe au contenu de l'option
            if (strcmp(ptr, "1") == 0) { // Si la valeur de l'option est "1"
                optionFound = 1; // L'option "bigfile" avec valeur "1" a été trouvée
                break;
            }
        }
        ptr += strlen(ptr) + 1; // Passer à la prochaine option
    }
        uint16_t nombre_block; //onva  recupérer le numéro de block  dans cette variable
     memcpy(&nombre_block, (uint16_t *) & buffer[2], 2);
        nombre_block = ntohs(nombre_block);

    if (optionFound) {
        printf("Option 'bigfile' avec valeur '1' trouvée dans OACK.\n");
        // Continuez le traitement avec cette option activée
         send_ACK(client_socket, nombre_block,serv_addr);
    } else {
        printf("Option 'bigfile' avec valeur '1' non trouvée dans OACK.\n");
        // Gérez le cas où l'option "bigfile" n'est pas confirmée par le serveur
        send_ERROR(client_socket,serv_addr,1);

    }
        FILE *fichier_client = fopen(nom_fichier_serveur, "w");/*ouvrir un fichier en mode ecriture pour stocker le data
                                           de tompon dans ce fichier */
    send_ACK(client_socket, 0,serv_addr);
    int recv_len = recvfrom(client_socket,buffer,BUFSIZE,0,(struct sockaddr *)&serv_addr,(socklen_t *) & longueur_addresse);

     if (fichier_client == NULL) 
    {
    fprintf(stderr, "Error while opening the destination file: errno ");
    exit(-1);
    
    }
    /* la packet de  data d'opcode 3 est constitué selon cette structure 
    [opcode(2bit)|blck_number(2bit)|data]
    donc pour récupérer les donnée envoyé par le serveur on doit commencer par l'indice 4
    donc la boucle for par la suite récupére les donné de packet data dans la fichier client
    */
    int i = 0;
		for (i = 0; i < recv_len - 4; i++) {
			// 

            fputc(buffer[i + 4], fichier_client);
		}
             //à chaque fois qu'un packet data arrive on doit l'aquiter par un ACK 
    send_ACK(client_socket, nombre_block,serv_addr);
    /* la taille de packet data est 512 octer or que il ya un entete de 4 octet 
    donc on va récuperer à chaque fois le packet et une fois qu'on a un packet <512 
    c'est à dire c'est la derniére packet donc la boucle while va sortir
    */

    while (recv_len == 516) {
//tompon 
		recv_len = recvfrom(client_socket,(char *)buffer,BUFSIZE,0,(struct sockaddr *)&serv_addr,(socklen_t *) & longueur_addresse);

        erreur_verification(recv_len,"erreur de recevour de packet data");
        memcpy(&nombre_block, (uint16_t *) & buffer[2], 2);// on copie le numéro de blocl dans la variable nombre block
		nombre_block = ntohs(nombre_block);//convertit des nim"ro de packer réseau en  locales.
     //on a expliquer ce bloc précédement 
     int i = 0;
			for (i = 0; i < recv_len - 4; i++) {
				fputc(buffer[i + 4], fichier_client);
			}
			send_ACK(client_socket, nombre_block,serv_addr);
     }
     /*shutdown en C est utilisée pour arrêter l'envoi et/ou la 
     réception de données sur un socket.*/
    shutdown(client_socket, SHUT_RDWR);
    close(client_socket);
    fclose(fichier_client);
    }
}

int main(int argc, char *argv[])
{
  // Defining the IP and Port
   if (argc !=5)
    {
        printf("nombre d'argument invalide \n usage : .\a.out get|put [nom_fichier server_ip] [server_port]");
        return -1;
    }
  char *nom_fichier = argv[2];
    char *ip = argv[3];
    const int port = atoi(argv[4]);
  // Defining variables
  int server_sockfd,client_socket;
  struct sockaddr_in server_addr;
  // Creating a UDP socket
  server_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_sockfd < 0)
  {
    perror("[ERROR] socket error");
    exit(1);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);
  // Reading the text file
  // Sending the file data to the server
  if (strcmp(argv[1], "get") == 0)
     {
     get(nom_fichier,server_sockfd,server_addr);


     }  
     else if(strcmp(argv[1], "put") == 0) 
     {
      put(nom_fichier,server_sockfd,server_addr);
     }
     else 
     {
        printf("commande introuvable get | put");
     }

  printf("[SUCCESS] Data transfer complete.\n");
  printf("[CLOSING] Disconnecting from the server.\n");
  close(server_sockfd);
  return 0;
}
