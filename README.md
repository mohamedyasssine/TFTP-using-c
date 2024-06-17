# Serveur et Client TFTP en C

Ce projet implémente un serveur et un client TFTP en langage C. Le protocole TFTP (Trivial File Transfer Protocol) est un protocole simplifié utilisé pour le transfert de fichiers sur un réseau.

## Utilisation

### Serveur 

1. Ouvrez un terminal dans le dossier `serveur`.
2. Compilez le code source du serveur avec la commande suivante :
    ```
    gcc serveur_multithread.c -o serveur_multithread -lpthread
    ```
3. Exécutez le serveur avec la commande suivante :
    ```
    ./serveur_multithread [adresse_IP] [port]
    ```
    Remplacez `[adresse_IP]` par l'adresse IP sur laquelle le serveur doit écouter et `[port]` par le numéro de port sur lequel le serveur doit écouter.

### Serveur multithread

1. Ouvrez un terminal dans le dossier `serveur`.
2. Compilez le code source du serveur avec la commande suivante :
    ```
    gcc serveur_multithread.c -o serveur_multithread -lpthread
    ```
3. Exécutez le serveur avec la commande suivante :
    ```
    ./serveur_multithread [adresse_IP] [port]
    ```
    Remplacez `[adresse_IP]` par l'adresse IP sur laquelle le serveur doit écouter et `[port]` par le numéro de port sur lequel le serveur doit écouter.
    
### Client

1. Ouvrez un terminal dans le dossier `src`.
2. Compilez le code source du client avec la commande suivante :
    ```
    gcc client.c -o client
    ```
3. Exécutez le client avec la commande suivante pour importer un fichier depuis le serveur :
    ```
    ./client get [nom_fichier] [adresse_IP_serveur] [port_serveur]
    ```
    Remplacez `[nom_fichier]` par le nom du fichier à importer, `[adresse_IP_serveur]` par l'adresse IP du serveur TFTP, et `[port_serveur]` par le numéro de port sur lequel le serveur écoute.

4. Exécutez le client avec la commande suivante pour envoyer un fichier vers le serveur :
    ```
    ./client put [nom_fichier] [adresse_IP_serveur] [port_serveur]
    ```
    Remplacez `[nom_fichier]` par le nom du fichier à envovoyer vers le serveur TFTP, `[adresse_IP_serveur]` par l'adresse IP du serveur, et `[port_serveur]` par le numéro de port sur lequel le serveur écoute.

5. exemple d'utilisasion 
1)get
il ya un fichier test.txt dans le serveur
dans premier terminal :
./serveur 8080 
dans le deuxiéme terminal :
./client get test.txt 127.0.0.1 8080
2)put 
il ya un fichier server.txt dans le src 
dans premier terminal :
./serveur 8080 
dans le deuxiéme terminal :
./client put server.txt 127.0.0.1 8080


