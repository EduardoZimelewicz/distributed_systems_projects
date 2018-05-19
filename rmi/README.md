#How to run:

obs: created to run in linux environment

- Open one terminal

- Compile all java files:
```
javac *.java
```
- Start the rmiregistry by typing:
```
rmiregistry &
```
- In the same terminal, run the server by typing:
```
java -Djava.security.policy="/home/${your_user}/distributed_systems_projects/rmi/security.policy" HelloServer
```
- Open another terminal, run the client side by typing:
```
java -Djava.security.policy="/home/${your_user}/distributed_systems_projects/rmi/security.policy" HelloClient
```

