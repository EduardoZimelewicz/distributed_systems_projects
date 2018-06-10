import java.io.IOException;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.Scanner;


public class PrintClient {  

    public static void main(String[] args) throws MalformedURLException, RemoteException, 
        NotBoundException, InterruptedException, IOException {
        
        if(System.getSecurityManager() == null){
            System.setSecurityManager(new SecurityManager());
        }

        Scanner keyboard = new Scanner(System.in);
        System.out.println("What is your name?\n");
        String name = keyboard.nextLine();
        System.out.println("\nWhat would you like to print?(Type end to stop)\n");
        String message = keyboard.nextLine();
        while(!message.equals("end")){
            Print printer = (Print) Naming.lookup("//172.17.0.2/PrintServer");
            System.out.println(printer.printMessage(name, message));
            System.out.println("\nWhat would you like to print next?(Type end to stop)\n");
            message = keyboard.nextLine();
        }
        keyboard.close();
    }
}
