const net = require('net');

// Définir les informations de connexion au serveur du SGBD
const serverHost = '192.168.1.38'; // Remplacez par l'adresse IP ou le nom du serveur
const serverPort = 3331; // Remplacez par le port du serveur

// Créer une seule instance de connexion
const client = new net.Socket();

// Fonction pour établir la connexion
function connectToServer() {
    return new Promise((resolve, reject) => {
        client.connect(serverPort, serverHost, () => {
            console.log('Connecté au serveur');
            resolve();
        });

        client.on('error', (err) => {
            reject(err);
        });
    });
}

// Fonction pour envoyer une commande au serveur et attendre la réponse
function sendCommand(command) {
    return new Promise((resolve, reject) => {
        client.write(command);

        let dataBuffer = Buffer.from('');

        client.on('data', (data) => {
            dataBuffer = Buffer.concat([dataBuffer, data]);
            const jsonData = dataBuffer.toString();
            resolve(jsonData);
        });

        // client.on('end', () => {
        //     const jsonData = dataBuffer.toString();
        //     resolve(jsonData);
        // });

        client.on('error', (err) => {
            reject(err);
        });
    });
}

// Exemple d'utilisation
(async () => {
    try {
        // Établir la connexion
        await connectToServer();

        // Envoyer une commande et attendre la réponse
        const response = await sendCommand(JSON.stringify({type:"req",collection:"students", verb:"GET",db:"test"}));

        // Utiliser la réponse
        console.log('Réponse reçue :', response);

        // Vous pouvez réutiliser la connexion pour d'autres requêtes ici

        // Fermer la connexion lorsque vous avez terminé
        client.end();
    } catch (err) {
        console.error('Erreur de connexion :', err);
    }
})();
