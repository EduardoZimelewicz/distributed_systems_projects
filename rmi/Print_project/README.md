#How to

obs: the same as hello example, but there are a few modifications

- Change the path where the file is created in the PrintServer class

- There is security.policy file in this dir, use it instead of the hello example

- Change the paths for printServer & printClient

- If occurs a unmarshalling exception, kill the rmiregistry with:
´´´
fuser -k 1099/tcp
´´´
