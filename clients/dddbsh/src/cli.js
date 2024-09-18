#!/usr/bin/env node
import { program } from "commander";

import repl from "./repl.js";

program.name("dddbsh")
program.option("--host <host>", "Server to connect to", "localhost")
program.option("--port <port>", "Port to connect to", "33301")
program.option("-d, --database <database>", "Database to connect to", 'test')
program.option("-u, --username <username>", "Username for authentication")
program.option("-p, --password <password>", "Password for authentication")
program.showSuggestionAfterError()
program.showHelpAfterError()

program.action((options) => {
    repl(options.host, options.port, options.username, options.password, options.database);
})

program.parse(process.argv);


