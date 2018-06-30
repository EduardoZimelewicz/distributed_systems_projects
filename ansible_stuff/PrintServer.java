import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.Thread.State;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;


public class PrintServer extends UnicastRemoteObject implements Print {

    private static final long serialVersionUID = 1L;
    private File output;
    //private FileWriter writer;
    private static ArrayBlockingQueue<PrintRequest> buffer;

    public PrintServer() throws RemoteException {
        super();
    }

    @Override
    public String printMessage(String name, String message) throws RemoteException, IOException, InterruptedException {
        output = new File("./output.txt");

        if (!output.exists()) {
            output.createNewFile();
        }
        if (buffer.size() == 3) {
            return "\nBuffer is full. Try again later.";
        }
        
        else{
            PrintRequest req = new PrintRequest(name,message);
            
            buffer.put(req);
            return "\nYour message will be printed soon.";
        }
    }

    public static void main(String[] args) {
        try {
            if (System.getSecurityManager() == null) {
                System.setSecurityManager(new SecurityManager());
            }
            buffer = new ArrayBlockingQueue<PrintRequest>(3);
            System.setProperty("java.rmi.server.hostname","18.228.36.148");
            Naming.rebind("/PrintServer", new PrintServer());
            System.out.println("PrintServer bound");
            PrinterThread printer0 = new PrinterThread(0, buffer);
            PrinterThread printer1 = new PrinterThread(1,buffer);
            printer0.start();
            printer1.start();
        } catch (Exception e) {
            System.out.println("failed to bound");
            e.printStackTrace();
        }
    }
}
