import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;

public class PrintClient {
    public static ArrayList<Thread> clients = new ArrayList<Thread>(); //size must be 5
    public static ArrayList<Thread> buffer = new ArrayList<Thread>(); //size must be 3
    //only 2 printers  

    public static void main(String[] args) throws MalformedURLException, RemoteException, 
        NotBoundException, InterruptedException {
        
        if(System.getSecurityManager() == null){
            System.setSecurityManager(new SecurityManager());
        }

        for (int itr = 0; itr < 5; itr++){
            clients.add(new ClientThread(itr)); 
        }

        for (int itr = 0; itr < 5; itr++){
            clients.get(itr).start();
            //clients.get(itr).sleep(100);
            System.out.println("Client Thread " + itr + " started");
        }
    }
}