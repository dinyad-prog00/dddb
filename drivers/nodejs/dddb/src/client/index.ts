import { Socket } from "net";
import { Payload } from "../types";

const sendRequest = (client: Socket, req: Payload) => {

    return new Promise((resolve, reject) => {
        client.write(JSON.stringify(req));

        let dataBuffer = Buffer.from('');

        const onError = (err: any) => {
            client.removeListener('data', onDataReceived)
            reject(err);
        }

        const onDataReceived = (data: any) => {

            dataBuffer = Buffer.concat([dataBuffer, data]);
            console.log("received: ", dataBuffer.toString())

            const jsonData = JSON.parse(dataBuffer.toString());

            client.removeListener('data', onDataReceived)
            client.removeListener('error', onError);



            if (jsonData.type === "ok" || jsonData.type === "str") {
                resolve(jsonData.data);
            } else {
                reject(jsonData.message);
            }
        };

        const toclearTimeout = () => {
            client.removeListener('data', toclearTimeout)
            clearTimeout(timeoutId);
        }

        client.on('data', onDataReceived);

        client.on('error', onError);


        const timeout = 5000;
        const timeoutId = setTimeout(() => {
            client.removeListener('data', onDataReceived); // Remove the data listener
            client.removeListener('error', onError); // Remove the data listener

            reject("Timeout: Data not received within 5s.");
        }, timeout);

        // Clear the timeout if data is received before the timeout expires
        client.on('data', toclearTimeout);
    });


}

export {
    sendRequest
}