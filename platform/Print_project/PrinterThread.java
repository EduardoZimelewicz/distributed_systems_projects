import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.Calendar;

public class PrinterThread extends Thread {
    private int id;
    private ArrayBlockingQueue<PrintRequest> buffer;
    private int qtyRequests;
    private long totalTimeSpendInRequests;
    private Calendar calendar;

    public PrinterThread(int id, ArrayBlockingQueue<PrintRequest> buffer) {
        this.id = id;
        this.buffer = buffer;
        this.qtyRequests = 0;
        this.totalTimeSpendInRequests = 0;
    }

    @Override
    public void run()  {
        try {
            while (true) {
                if (buffer.size() > 0) {
                    long init = System.currentTimeMillis();
                    File output = new File("./output.txt");
                    PrintRequest req = buffer.take();
                    System.out.println("Printer "+id+" is printing");
                    sleep(1000);
                    FileWriter writer = new FileWriter(output, true);
                    writer.write("printer"+id+"("+req.name + ": " + req.message + ")\n");
		            System.out.println("printer "+id+" is done printing");
                    writer.close();
                    totalTimeSpendInRequests += System.currentTimeMillis() - init;
                    qtyRequests++;
                    System.out.printf("Printed %d: %d requests in %d miliseconds\n", id, qtyRequests, totalTimeSpendInRequests);
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
