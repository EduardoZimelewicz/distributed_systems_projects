import java.rmi.Remote;
import java.rmi.RemoteException;
import java.io.IOException;

public interface Print extends Remote {
   public String printMessage (String message) throws RemoteException, IOException;
}