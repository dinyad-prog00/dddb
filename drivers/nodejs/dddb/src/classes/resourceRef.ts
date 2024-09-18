import { Socket } from "net"
import uriParser from "../utils/uriParser";

class ResourceRef {
    client = new Socket();
    uri: string
    path: string

    constructor(uri: string, path: string) {
        this.uri = uri
        this.path = path
    }

    subscribe(callback: (data: any) => void, onSuccess: () => void, onError: (err: any) => void) {
        const { host, port, dbname, username, password } = uriParser(this.uri)
        return new Promise((resolve, reject) => {

            const onDataReceived = (data: any) => {
                const answer = Buffer.from(data).toString();
                console.log("=========================================yy")
                callback(answer)
            }

            const onSubAnswerReceived = (data: any) => {

                const answer = Buffer.from(data).toString();
                if (answer === "Client subscribed successful!") {
                    console.log(answer)
                    if (onSuccess) {
                        onSuccess();
                    }
                    this.client.removeListener('data', onSubAnswerReceived)
                    this.client.on('data', onDataReceived)
                    console.log("=========================================yy")
                    
                    //resolve(true);
                } else {
                    this.client.removeListener('data', onSubAnswerReceived)
                    if (onError) {
                        onError(new Error(answer))
                    }
                    resolve(false)
                }
            }

            this.client.connect(port, host, () => {
                const sub = {
                    type: "SUB",
                    doc: this.path,
                }
                this.client.write(JSON.stringify(sub));

                this.client.on('data', onSubAnswerReceived);
            })
        })
    }

}

export default ResourceRef