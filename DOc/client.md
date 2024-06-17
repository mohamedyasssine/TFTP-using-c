# Documentation du fichier client.c

Le fichier `client.c` contient le code source du client dans notre projet de communication client-serveur utilisant des sockets UDP.

## Fonctions principales

### main()

La fonction `main()` est la fonction principale du programme client. Elle crée une socket UDP, initialise l'adresse du serveur, puis appelle les fonctions `get()` ou `put()` en fonction de la commande passée en argument (get pour récupérer un fichier du serveur, put pour envoyer un fichier au serveur).

### get()

La fonction `get()` est responsable de l'envoi d'une requête de lecture (RRQ) au serveur pour récupérer un fichier. Elle appelle `send_RRQ()` pour envoyer la requête, puis attend et traite les paquets DATA envoyés par le serveur.

### put()

La fonction `put()` est responsable de l'envoi d'une requête d'écriture (WRQ) au serveur pour envoyer un fichier. Elle appelle `send_WRQ()` pour envoyer la requête, puis envoie les paquets DATA au serveur en réponse aux ACK reçus en appelant la fonction `transferer_DATA()`.

## Fonctions utilitaires

- `send_RRQ()`: Envoie une requête de lecture au serveur avec le nom du fichier demandé.
- `send_WRQ()`: Envoie une requête d'écriture au serveur avec le nom du fichier à écrire.
- `send_ACK()`: Envoie un paquet d'acquittement (ACK) au serveur avec le numéro de bloc correspondant.
- `transferer_DATA()`: Transfère les données du fichier entre le client et le serveur.


