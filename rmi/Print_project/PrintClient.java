import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class PrintClient {
    public static void main(String[] args) throws MalformedURLException, RemoteException, NotBoundException {

        try {
            if(System.getSecurityManager() == null){
                System.setSecurityManager(new SecurityManager());
            }
    
            String message = "Message from PrintClient\n";
            Print printer = (Print) Naming.lookup("//localhost/PrintServer");
            System.out.println(printer.printMessage(message));
        } 

        catch (Exception e) {
            System.out.println("Error looking up registry");
            e.printStackTrace();
        }
    }
}