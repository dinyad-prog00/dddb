exec : 
	gcc src/main.c src/store/store.c  src/helpers/helpers.c  src/server/utils.c lib/cjson/cJSON.c  src/server/server.c -g -levent -o main  && ./main
run : 
	./main

client : 
	gcc src/clients/dddbsh/client.c  src/clients/dddbsh/utils.c lib/cjson/cJSON.c src/helpers/helpers.c -o dddbsh -lreadline -lhistory
	
