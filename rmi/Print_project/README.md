#How to

Obs:
 - Dont forget to change file path at PrinterThread line 23
 - Dont forget to change ip adress at PrintServer line 50 and PrintClient line 24

- At the project root dir, run the compile.sh as
```
./compile
```
- Then, run the sobe_servidor.sh as
```
./sobe_servidor
```

- Finally, run the "sobe_cliente.sh" as
```
./sobe_cliente
```
- Dont forget to kill rmiregistry at port 1099
```
fuser -k 1099/tcp
```

