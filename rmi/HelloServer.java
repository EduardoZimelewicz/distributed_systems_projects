import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

public class HelloServer extends UnicastRemoteObject implements Hello {
    
    private static final long serialVersionUID = 1L;

    public HelloServer() throws RemoteException{
        super();
    }

    @Override
    public String sayHello(String name) throws RemoteException{
        return "Hello from HelloServer to " + name;
    }

    public static void main (String [] args){
        try {
            if(System.getSecurityManager() == null){
                System.setSecurityManager(new SecurityManager());
            }
    
            Naming.rebind("//localhost/HelloServer", new HelloServer());
            System.out.println("HelloServer bound");
        }
        catch (Exception e){
            System.out.println("failed to bound");
            e.printStackTrace();
        }
    }
}