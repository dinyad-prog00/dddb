import { start } from 'repl';

const myRepl = start({
  prompt: 'My REPL> ',
});

myRepl.context.greet = () => {
  console.log('Hello, world!');
};

myRepl.on('exit', () => {
  console.log('Exiting REPL');
});
