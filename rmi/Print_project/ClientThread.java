import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class ClientThread extends Thread {
    private int id;

    public ClientThread(int id){
        this.id = id;
    }

    public void run(){
        try {
            if(System.getSecurityManager() == null){
                System.setSecurityManager(new SecurityManager());
            }
    
            String message = "Message from PrintClient " + id + "\n" ;
            Print printer = (Print) Naming.lookup("//localhost/PrintServer");
           //System.out.println(printer.printMessage(message));
        } 

        catch (Exception e) {
            System.out.println("Error looking up registry");
            e.printStackTrace();
        }
    }
}