import { start } from 'repl';
import pgk from "dddb"
const { Dddb } = pgk
const dddb = new Dddb()
let db;

const colls = ["tasks", "books", "students"];

dddb.connect("dddb://localhost:33301/test", () => {
    console.log("Connected successfully !")

    function customEval(cmd, context, filename, callback) {
        // Utilisez une fonction asynchrone pour évaluer la commande
        (async () => {
            try {
                const result = await eval(cmd);
                callback(null, result); // Renvoie le résultat à afficher
            } catch (error) {
                callback(error); // Renvoie une erreur en cas d'erreur
            }
        })();
    }

    const myRepl = start({
        prompt: 'test> ',
        //eval: customEval,
    });
    myRepl.context.db = dddb.db()
    myRepl.context.greet =  function () {
        return 'Hello, world!';
    };
    colls.forEach((it) => myRepl.context.db[it] = myRepl.context.db.model(it))

    myRepl.on('exit', () => {
        console.log('Exiting REPL');


    });


})



