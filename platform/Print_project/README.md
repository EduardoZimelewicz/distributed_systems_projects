#How to

Obs:
 - Dont forget to change file path at PrinterThread line 23
 - Dont Forget to change ip adress at PrintServer line 50 and PrintClient line 24
 - Both are configured in code for connecting in the same network, any change would be in the ip addresses above

First run the server

```
docker build -f Dockerfile.server -t print-server .
```
Then run the client

```
docker build -f Dockerfile.client -t print-client .
```
After that, to see results ssh into the container

```
docker exec -i -t <server-container-name> /bin/bash
```

