# Documentation du fichier serveur_multithread.c

Le fichier `serveur_multithread.c` contient le code source du serveur dans notre projet de communication client-serveur utilisant des sockets UDP.

## Fonctions principales

### main()

La fonction `main()` crée un socket UDP pour le serveur, le lie à une adresse IP et un port spécifié (entrés par l'utilisateur lors de l'exécution du programme), puis appelle la fonction `ecouter_packet` pour écouter les paquets provenant des clients.

### ecouter_packet()

La fonction `ecouter_packet()` est la fonction principale qui écoute les paquets entrants et crée un thread client pour traiter . Si le paquet entrant est une requête de lecture (RRQ) ou d'écriture (WRQ) elle crée socket client ainsi qu'un thread appelant la  fonctions `client_thread()`. Elle gère également les erreurs d'opcode en envoyant un paquet d'erreur approprié avec la fonction `send_ERROR`.

### client_thread()

La fonction `client_thread()` est la fonction principale des threads clients, elle récupère le paquet reçu avant la création du thread et vérifie si c'est une requête de lecture (RRQ) ou d'écriture (WRQ) et appelle les fonctions `transferer_DATA` ou `recevoir_DATA` respectivement, puis aprés l'exécution des fonctions libère les ressources et termine le thread.

## Fonctions utilitaires

- `transferer_DATA()`: Fonction qui gère le transfert de données vers un client.
- `recevoir_DATA()`: Fonction qui gère la réception de données d'un client.
- `send_ERROR()`: Fonction utilitaire pour envoyer un paquet d'erreur (ERROR) au client en cas de problème.


