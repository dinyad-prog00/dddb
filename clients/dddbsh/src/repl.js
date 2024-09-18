import { Recoverable, start } from "repl";
import { runInNewContext } from "vm";
import { processTopLevelAwait } from "node-repl-await";
import { displayList } from "./utils.js";
import pgk from "dddb"

console.log2 = console.log
console.log = () => { }

const repl = (host, port, username = "admin", password = "admin010", dbname = "test") => {
    const { Dddb } = pgk
    const dddb = new Dddb()

    function isRecoverableError(error) {
        if (error.name === 'SyntaxError') {
            return /^(Unexpected end of input|Unexpected token)/.test(error.message);
        }
        return false;
    }

    

    dddb.connect(`dddb://${username}:${password}@${host}:${port}/${dbname}`, () => {
        console.log2("Connected successfully !")
        async function myEval(code, context, filename, callback) {
            code = processTopLevelAwait(code) || code;

            try {
                let result = await runInNewContext(code, context);
                if (result){
                    callback(null, result);
                }else{
                    callback(null);
                }
            } catch (e) {
                if (isRecoverableError(e)) {
                    callback(new Recoverable(e));
                } else {
                    console.log2("\u001b[31m%s\u001b[0m: %s", e.name, e.message);
                    callback(null);
                }
            }
        }

        const myRepl = start({ prompt: dbname + '> ', eval: myEval});

        myRepl.context = new Proxy(myRepl.context, {
            get(target, prop) {

                if (prop === "dbs") {
                    target[prop] = (async () => {
                        return displayList( await dddb.getDatabases() )
                    })();

                }
                else if (prop == "collections") {
                    target[prop] = (async () => {
                        return displayList( await dddb.db().getCollections() )
                    })();
                }
                else if (!target[prop]) {
                    console.log2("\u001b[31mUnknown command\u001b[0m")
                }
                return target[prop];
            }
        });

        myRepl.context.dbs = ""
        myRepl.context.collections = ""

        myRepl.context.createdb = async (name) => { return await dddb.createDatabase(name) }

        myRepl.context.db = new Proxy(dddb.db(), {
            get(target, prop) {
                if (!target[prop]) {
                    target[prop] = target.model(prop)
                }
                return target[prop];
            }
        })


        dddb.db().getCollections().then((colls)=>colls.forEach((it) => myRepl.context.db[it] = myRepl.context.db.model(it))) 

        myRepl.on('exit', () => {
            console.log2('Exiting REPL');
            dddb.client.destroy();
        });
    }, (err) => {
        console.log2("\u001b[31mError de connexion: \u001b[0m", err.message)
    })
}

export default repl;