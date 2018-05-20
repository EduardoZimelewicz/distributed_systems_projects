import java.rmi.Remote;
import java.rmi.RemoteException;
import java.io.IOException;

public interface Print extends Remote {
   public String printMessage (String name, String message) throws RemoteException, IOException, InterruptedException;
}