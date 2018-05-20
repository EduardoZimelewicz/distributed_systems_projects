import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.rmi.server.UnicastRemoteObject;

public class PrintServer extends UnicastRemoteObject implements Print {
    
    private static final long serialVersionUID = 1L;
    private File output;
    private FileWriter writer;

    public PrintServer() throws RemoteException{
        super();
    }

    @Override
    public String printMessage(String message) throws RemoteException, IOException{
        output = new File("/home/eduardo/distributed_systems_projects/rmi/Print_project/output.txt");

        if(!output.exists()){
            output.createNewFile();
        }

        writer = new FileWriter(output, true);
        writer.write(message);
        writer.close();

        return "Printed to output.txt";
    }

    public static void main (String [] args){
        try {
            if(System.getSecurityManager() == null){
                System.setSecurityManager(new SecurityManager());
            }
    
            Naming.rebind("//localhost/PrintServer", new PrintServer());
            System.out.println("PrintServer bound");
        }
        catch (Exception e){
            System.out.println("failed to bound");
            e.printStackTrace();
        }
    }
}