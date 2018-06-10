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
    }

    @Override
    public void run()  {
        try {
            while (true) {
                if (buffer.size() > 0) {
                    File output = new File("./output.txt");
                    PrintRequest req = buffer.take();
                    System.out.println("Printer "+id+" is printing");
                    sleep(20000);
                    FileWriter writer = new FileWriter(output, true);
                    writer.write("printer"+id+"("+req.name + ": " + req.message + ")\n");
		    System.out.println("printer "+id+" is done printing");
                    writer.close();
                    
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
