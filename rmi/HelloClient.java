import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class HelloClient {
    public static void main(String[] args) throws MalformedURLException, RemoteException, NotBoundException {

        try {
            if(System.getSecurityManager() == null){
                System.setSecurityManager(new SecurityManager());
            }
    
            String name = "HelloClient";
            Hello hell = (Hello) Naming.lookup("//localhost/HelloServer");
            System.out.println(hell.sayHello(name));
        } 

        catch (Exception e) {
            System.out.println("Error looking up registry");
            e.printStackTrace();
        }
    }
}