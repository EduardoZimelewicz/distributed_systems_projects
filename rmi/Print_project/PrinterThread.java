import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;



public class PrinterThread extends Thread {
    private int id;
    private ArrayBlockingQueue<PrintRequest> buffer;

    public PrinterThread(int id, ArrayBlockingQueue<PrintRequest> buffer) {
        this.id = id;
        this.buffer = buffer;
        start();
    }

    @Override
    public void run()  {
        try {
            while (true) {
                if (buffer.size() > 0) {
                    File output = new File("/home/paulo/distributed_systems_projects/rmi/Print_project/output.txt");
                    PrintRequest req = buffer.take();
                    FileWriter writer = new FileWriter(output, true);
                    writer.write("printer"+id+"("+req.name + ": " + req.message + ")\n");
                    System.out.println("printed");
                    writer.close();
                    sleep(20000);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}