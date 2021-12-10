package edu.montana.csci.csci366.archivecat.archiver.runners;

import edu.montana.csci.csci366.archivecat.archiver.jobs.AbstractDownloadJob;
import edu.montana.csci.csci366.archivecat.archiver.jobs.DownloadJob;

import java.util.List;
import java.util.concurrent.*;

public class ThreadPoolJobRunner implements DownloadJobRunner {
    public void executeJobs(List<? extends DownloadJob> downloadJobs) {
        // TODO implement - use a ThreadPoolExecutor with 10 threads to execute the jobs
        CountDownLatch latch = new CountDownLatch(downloadJobs.size());
        ExecutorService pool = Executors.newFixedThreadPool(10);
        for (DownloadJob job : downloadJobs)
            pool.submit(() -> {
                try {
                    job.run();
                } catch (Exception e) {
                    throw new RuntimeException(e);
                }
                latch.countDown();
            });
            try {
                latch.await();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
        }
    }


