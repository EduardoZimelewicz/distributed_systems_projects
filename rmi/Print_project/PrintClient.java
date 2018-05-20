import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Scanner;


public class PrintClient {
    public static ArrayList<Thread> clients = new ArrayList<Thread>(); //size must be 5
    public static ArrayList<Thread> buffer = new ArrayList<Thread>(); //size must be 3
    //only 2 printers  

    public static void main(String[] args) throws MalformedURLException, RemoteException, 
        NotBoundException, InterruptedException, IOException {
        
        if(System.getSecurityManager() == null){
            System.setSecurityManager(new SecurityManager());
        }

        Scanner keyboard = new Scanner(System.in);
        System.out.println("What is your name?");
        String name = keyboard.nextLine();
        System.out.println("What would you like to print?(Type end to stop)");
        String message = keyboard.nextLine();
        while(!message.equals("end")){
            Print printer = (Print) Naming.lookup("//localhost/PrintServer");
            System.out.println(printer.printMessage(name, message));
            System.out.println("What would you like to print next?(Type end to stop)");
            message = keyboard.nextLine();
        }
        keyboard.close();

        
        /*for (int itr = 0; itr < 5; itr++){
            clients.add(new ClientThread(itr)); 
        }

        for (int itr = 0; itr < 5; itr++){
            clients.get(itr).start();
            //clients.get(itr).sleep(100);
            System.out.println("Client Thread " + itr + " started");
        }*/
    }
}