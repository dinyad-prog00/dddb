import { Recoverable, start } from "repl";
import { runInNewContext } from "vm";
import { processTopLevelAwait } from "node-repl-await";
import pgk from "dddb"


const repl = (host, port) => {
    const { Dddb } = pgk
    const dddb = new Dddb()

    function isRecoverableError(error) {
        if (error.name === 'SyntaxError') {
            return /^(Unexpected end of input|Unexpected token)/.test(error.message);
        }
        return false;
    }

    const colls = ["tasks", "books", "students"];

    dddb.connect(`dddb://admin:yeto20@${host}:${port}/test`, () => {
        console.log("Connected successfully !")
        async function myEval(code, context, filename, callback) {
            code = processTopLevelAwait(code) || code;

            try {
                let result = await runInNewContext(code, context);
                callback(null, result);
            } catch (e) {
                if (isRecoverableError(e)) {
                    callback(new Recoverable(e));
                } else {
                    console.log("\u001b[31m%s\u001b[0m: %s", e.name, e.message);
                    callback(null);
                }
            }
        }

        const myRepl = start({ prompt: 'test> ', eval: myEval });
        myRepl.context.greet = async function () {
            return 'Hello, world!';
        };

        myRepl.context.db = new Proxy(dddb.db(), {
            get(target, prop) {
                if (!target[prop]) {
                    target[prop] = target.model(prop)
                }
                return target[prop];
            }
        })
        colls.forEach((it) => myRepl.context.db[it] = myRepl.context.db.model(it))

        myRepl.on('exit', () => {
            console.log('Exiting REPL');
            dddb.client.destroy();
        });
    })
}

export default repl;