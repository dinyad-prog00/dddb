import * as net from "net"
import { uriParser } from "../utils";
import Db from "./db";

class Dddb {
    defaultDbName = "test";
    client = new net.Socket();

    //to be removed
    uri?: string

    /** Connect to the dddb server  */
    connect(uri: string, onConnect?: () => void, onError?: (error: Error) => void) {

        //to be removed
        
        this.uri = uri
        console.log("1",this.uri)

        const { host, port, dbname, username, password } = uriParser(uri)

        return new Promise((resolve, reject) => {
            const onLoginAnswerReceived = (data: any) => {
                const answer = Buffer.from(data).toString();
                if (answer === "Authentication successful!") {
                    console.log(answer)
                    if (onConnect) {
                        onConnect();
                    }
                    this.client.removeListener('data', onLoginAnswerReceived)
                    resolve(true);
                } else {
                    this.client.removeListener('data', onLoginAnswerReceived)
                    if (onError) {
                        onError(new Error(answer))
                    }
                    resolve(false)
                }
            }
            this.client.connect(port, host, () => {

                const auth = {
                    type: "LOGIN",
                    username,
                    password
                }

                this.client.write(JSON.stringify(auth));



                this.client.on('data', onLoginAnswerReceived);

                this.defaultDbName = dbname;

            });

            this.client.on('error', (err) => {
                if (onError) {
                    onError(err)
                }
                this.client.removeListener('data', onLoginAnswerReceived)
                resolve(false);
            });
        });
    }

    /** Get a db */
    db(dbName?: string) {
        console.log("2",this.uri)
        return new Db(this.client, dbName ? dbName : this.defaultDbName,this.uri)
    }
}


export default Dddb;