import * as net from "net"
import { uriParser } from "../utils";
import Db from "./db";

class Dddb {
    defaultDbName = "test";
    client = new net.Socket();

    /** Connect to the dddb server  */
    connect(uri: string, onConnect?: () => void, onError?: (error: Error) => void) {
        const { host, port, dbname } = uriParser(uri)
        return new Promise((resolve, reject) => {
            this.client.connect(port, host, () => {
                if (onConnect) {
                    onConnect();
                }
                this.defaultDbName = dbname;
                resolve(true);
            });

            this.client.on('error', (err) => {
                if (onError) {
                    onError(err)
                }
                reject(err);
            });
        });
    }

    /** Get a db */
    db(dbName?: string) {
        return new Db(this.client,dbName ? dbName : this.defaultDbName)
    }
}


export default Dddb;