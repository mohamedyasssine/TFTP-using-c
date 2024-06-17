  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <arpa/inet.h>
#include <sys/time.h>
  #include <sys/select.h>
#define MAX 512
#include <sys/time.h>
  #define BUFSIZE 1024
  struct sockaddr_in serv_addr;
  int listener;
  char *base;
  int erreur_verification(int r, char *message) {
      if (r < 0) {
          perror(message);
          return -1;
      }
      return 0;
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
    
   int sent_len = sendto(server_socket, buffer,BUFSIZE,0,(const struct sockaddr *)&serv_addr,sizeof(serv_addr));
   //erreur_verification(sent_len, "erreur d'envoi de ACK");
   
}
void send_OACK(int server_socket, struct sockaddr_in client_addr, char* options[], int num_options) {
        //paquet OACK est sur la forme suvante :[opcode|option1|0|valeur1...|optionn|0]
    char buffer[BUFSIZE];
    int longueur = 0;
    uint16_t opcode = htons(6); // L'opcode pour OACK est 6
    uint8_t end_string = 0; // Caractère de fin de chaîne
    
    // Copier l'opcode dans le buffer
    memcpy(buffer, &opcode, 2);
    longueur += 2;
    
    for (int i = 0; i < num_options; i += 2) { // On s'attend à ce que options[] soit une liste de paires option-valeur
        // Ajouter le nom de l'option
        int option_len = strlen(options[i]);
        memcpy(buffer + longueur, options[i], option_len);
        longueur += option_len;
        memcpy(buffer + longueur, &end_string, 1);
        longueur++;
           // Ajouter la valeur de l'option
        int value_len = strlen(options[i + 1]);
        memcpy(buffer + longueur, options[i + 1], value_len);
        longueur += value_len;
        memcpy(buffer + longueur, &end_string, 1);
        longueur++;
    }
    
    // Envoyer le paquet OACK
    int sent_len = sendto(server_socket, buffer, longueur, 0, (const struct sockaddr *)&client_addr, sizeof(client_addr));
    // erreur_verification(sent_len, "Erreur d'envoi de OACK");
    printf("OACK envoyé.\n");
}

  int recevoir_DATA(int client_socket ,struct sockaddr_in serv_addr,char nom_fichier[512] ,char* options[], int num_options)
  {
        char nom_fichier_serveur[516];
      
	    char nom_fichier_client[516];
        strcpy(nom_fichier_serveur,nom_fichier);
        //on va utiliser la variable opcode pour tester sur la réponse serveur par la suite
        uint16_t opcode;
        //buffer est un tampon pour stocker la réponse du serveur
        char buffer[BUFSIZE];
        memset(buffer, 0, BUFSIZE);
        
    // Définition de l'option "bigfile" et sa valeur "1"
    // Appel modifié de send_OACK pour inclure l'option "bigfile"
        send_OACK(client_socket, serv_addr, options, num_options);
        int longueur_addresse = sizeof(serv_addr);//on va utiliser longueur_addresse par la suite dans la fonction 
        //recvfrom est une fonction prédefinit dans la bib socket.h pour récuperer la reponse de serveur 
        int recv_len = recvfrom(client_socket,buffer,BUFSIZE,0,(struct sockaddr *)&serv_addr,(socklen_t *) & longueur_addresse);
        erreur_verification(recv_len,"set time out no packet recu (set time out)");
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
    fprintf(stderr, "Error");
   return -1;	
	} 
    else if (opcode == 3)	//cas de packet de data
    {
        uint16_t nombre_block; //onva  recupérer le numéro de block  dans cette variable
        memcpy(&nombre_block, (uint16_t *) & buffer[2], 2);
        nombre_block = ntohs(nombre_block);
        FILE *fichier_client = fopen(nom_fichier_serveur, "w");/*ouvrir un fichier en mode ecriture pour stocker le data
                                           de tompon dans ce fichier */
     if (fichier_client == NULL) 
    {
    fprintf(stderr, "Error while opening the destination file: errno ");
   return -1;
    
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
 
    fclose(fichier_client);
    }
    return 0;
  
  }

  int transferer_DATA(int client_socket ,struct sockaddr_in client_addr,char nom_fichier[512], char* options[], int num_options)
{
    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);
    //avec ipv4 et udp comme protocole
    // Définition de l'option "bigfile" et sa valeur "1"
       

    // Appel modifié de send_OACK pour inclure l'option "bigfile"
        send_OACK(client_socket, client_addr, options, num_options);
        //on attend l'ack
        size_t client_size = sizeof(client_addr);//longueur de addresse ip de client
        uint16_t opcode; 

        int recv_len = recvfrom(client_socket, (char *)buffer,BUFSIZE,0, (struct sockaddr *)&client_addr,(socklen_t *) & client_size);
        erreur_verification(recv_len,"erreur lor de recevoir la packet ACK");
        memcpy(&opcode, (uint16_t *) & buffer, 2);
	    opcode = ntohs(opcode);
            FILE *src_file;

    if(opcode==5)
    {
        send_ERROR(client_socket,client_addr,8);

    }
    else if (access(nom_fichier, F_OK) != 0)
    {
        printf(" erreur d'ouverture de fichier\n envoie de packet d'erreur");
        send_ERROR(client_socket,client_addr,1);
        return -1;
    }
    else if(opcode==4)
    {
        src_file = fopen(nom_fichier, "r"); //ouvrir le fichier en mode reading 
        //ici on va faire l'opération de l'envoi des data au client
        /*la structure de packet de data est [opcode (2bit)|numéro de block(2bit)|data(n bit)]*/
        char buffer[BUFSIZE];//tompon qui va contenir le packet data 
        memset(buffer, 0, BUFSIZE);
        
        uint16_t opcode; 
        int send_len; //le longueur de packet data à envoyer 
         client_size = sizeof(client_addr);//longueur de addresse ip de client
        // Déclaration d'un caractère pour stocker temporairement les données du fichier source
        char c;
	    int i = 4;/*on commence par l'indexe 4 car les deux premier bit vont etre l'opcode 
        et les deux bit suivant vont etre le numéro de block(numéro de packet)*/
        uint16_t numero_block = 1;//on va initialiser les numero des block de 1
        /*maintenant on va découper le fichier sur des packet jusqu'a the EOF est trouvé 
        EOF :est le marqueur de fin de fichier 
        */
        do
        {
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
            send_len =sendto(client_socket,buffer,i,0,( struct sockaddr *) &client_addr,sizeof(client_addr));
            erreur_verification(send_len,"erreur lord de l'envoi de packet");
            
        //remetrre le tompon à 0  
        memset(buffer, 0, BUFSIZE);
        /*pour chaque envoi d'un packet de data le serveur doit attendre la packet ACK 
          pour envoyer la prochaine packet data */
        int recv_len = recvfrom(client_socket, (char *)buffer,BUFSIZE,0, (struct sockaddr *)&client_addr,(socklen_t *) & client_size);
        erreur_verification(recv_len,"erreur lor de recevoir la packet ACK");
        memcpy(&block, (uint16_t *) & buffer[2], 2);//on copie la numéro de block contenu dans le tompon da,s ma variable block
		block = ntohs(block);
        
        //initialisation de l'index de tompon 
        i = 4;
		numero_block++;//incrémentation de numero de packet
        }
    }while(c!=EOF);
        }

    //clear le tompon
	//memset(buffer, 0, BUFSIZE);
    //fermeture de fichier
    fclose(src_file);
    /*fermeture de socket*/
    //shutdown(client_socket, SHUT_RDWR);
   // close(client_socket);
return 0;
}
void ecouter_packet(int listener,struct sockaddr_in client_addr,struct sockaddr_in serv_addr)
{
    //la socket client 
    printf("on ecoute");
    size_t client_size = sizeof(client_addr);

    //longueur de packet recu depuis le client 
    int recv_len;
    char buffer[BUFSIZE];
    memset(buffer, 0, BUFSIZE);
    char nom_fichier[512];//nom fichier recu dans la packet
    //mode de transfert de fichier (octer/netascci...) de packet recu
    char mode[10];
    //l'ocode de packet recu
    uint16_t opcode;
        recv_len =recvfrom(listener, (char *)buffer, BUFSIZE, 0,(struct sockaddr *)&client_addr, (socklen_t *) &client_size);
        //erreur_verification(recv_len,"");
        /*STRUCTURE DE RRQ ET WRQ PACKET [OPCODE(2bit)|nomfichier|0(fin de string|mode|0)]*/
        //recevoir le data (la packet)
        memcpy(&opcode, (uint16_t *) & buffer, 2); //on copie l'opcode de packet qui contient 2 bit dans le tompon
        opcode = ntohs(opcode);   
        strcpy(nom_fichier, buffer + 2);//on copie le nom de fichier depuis le tompon 
        strcpy(mode, buffer + 3 + strlen(nom_fichier));//ici on copie le nom fichier 
        if(opcode!=1&&opcode!=2)//si le packet recu n'est pas RRQ ni WRQ alors c'est un erreur 
        {
            printf("invalide opcode (numero) de packet ");
            send_ERROR(listener,client_addr,4);
            //envoyer_packet d'erreur a la client sent_error(); 
            /*mais ca dépend de message à envoyer donc on doit passer le message par paramétre
            et ca dépend de la code erreor car chaque erreur à 
            une code spécifique à mettre dans la packet donc on doit passer le code aussi en paramétre 
            */
        }
         char *ptr = buffer + 2 + strlen(nom_fichier) + 1 + strlen(mode) + 1;
        char *end = buffer + recv_len;
        char *options[2] = {0}; // Tableau pour stocker l'option et sa valeur
        int num_options = 0;

        while (ptr < end) {
            if (strcmp(ptr, "bigfile") == 0 && ptr + strlen(ptr) + 1 < end && strcmp(ptr + strlen(ptr) + 1, "1") == 0) {
                options[0] = "bigfile";
                options[1] = "1";
                num_options = 2;
                printf("Option 'bigfile' avec valeur '1' trouvée.\n");
                break;
            }
            ptr += strlen(ptr) + 1 + strlen(ptr + strlen(ptr) + 1) + 1; // Passer à la prochaine paire option-valeur
        }
            if(opcode==1)
             {
                transferer_DATA(listener,client_addr,nom_fichier, options, num_options);
                
                //appeller la méthode résponsable à la transfért
             }
             else if(opcode==2)
             {
                recevoir_DATA(listener,client_addr,nom_fichier ,options, num_options);  
             }
}

  int main(int argc, char *argv[])
  {
if(argc!=3)
{
    printf("erreur nombre d'argument invalide");
    return 1;
}
    // Defining the IP and Port
    char* ip = argv[1];
    const int port = atoi(argv[2]);
    // Defining variables
    int server_sockfd,client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFSIZE];
    int e;

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

    e = bind(server_sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (e < 0)
    {
      perror("[ERROR] bind error");
      exit(1);
    }
    
    fd_set master;
    fd_set read_fds;
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(server_sockfd, &master);
   while(1)
   {
        int fd_max = server_sockfd;

    struct timeval timeout;
        timeout.tv_sec = 5; // Spécifiez ici la durée du timeout en secondes
        timeout.tv_usec = 0;
    read_fds = master;
    int select_result = select(fd_max + 1, &read_fds, NULL, NULL, &timeout);
    erreur_verification(select_result, "[ERREUR] Erreur lors de la sélection des descripteurs de fichiers");
    
    if (FD_ISSET(server_sockfd, &read_fds)) {  
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        ecouter_packet(server_sockfd,client_addr,server_addr);
   }   
  
  }
    close(server_sockfd);
    return 0;
  }