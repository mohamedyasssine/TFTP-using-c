# Documentation du fichier serveur.c

Le fichier `serveur.c` contient le code source du serveur dans notre projet de communication client-serveur utilisant des sockets UDP.

## Fonctions principales

### main()

La fonction `main()` crée un socket UDP pour le serveur, le lie à une adresse IP et un port spécifié (entrés par l'utilisateur lors de l'exécution du programme), puis appelle la fonction `ecouter_packet` pour écouter les paquets provenant des clients.

### ecouter_packet()

La fonction `ecouter_packet()` est la fonction principale qui écoute les paquets entrants et appelle les fonctions appropriées en fonction de l'opcode du paquet. Elle décide si le paquet entrant est une requête de lecture (RRQ) ou d'écriture (WRQ) et appelle les fonctions `transferer_DATA` ou `recevoir_DATA` respectivement. Elle gère également les erreurs d'opcode en envoyant un paquet d'erreur approprié avec la fonction `send_ERROR`.

## Fonctions utilitaires

- `transferer_DATA()`: Fonction qui gère le transfert de données vers un client.
- `recevoir_DATA()`: Fonction qui gère la réception de données d'un client.
- `send_ERROR()`: Fonction utilitaire pour envoyer un paquet d'erreur (ERROR) au client en cas de problème.

###serveur monothread 
implémentation de select dans la main:
 fonction select nous permet de détecter quels descripteurs de fichiers ont une activité d'entrée/sortie prête à être lue ou écrite sans bloquer le processus

