package edu.montana.csci.csci366.archivecat.archiver;

import edu.montana.csci.csci366.archivecat.archiver.jobs.AbstractDownloadJob;
import edu.montana.csci.csci366.archivecat.archiver.jobs.DownloadJob;
import edu.montana.csci.csci366.archivecat.archiver.runners.DownloadJobRunner;
import edu.montana.csci.csci366.archivecat.archiver.runners.InThreadJobRunner;
import edu.montana.csci.csci366.archivecat.archiver.runners.ThreadedJobRunner;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;


public class Archiver {

    private static final Logger LOGGER = LoggerFactory.getLogger(Archiver.class);

    private final DownloadJobRunner _jobExecutor;
    private final Archive _archive;
    private final String _url;

    public Archiver(String url, DownloadJobRunner executor) throws Exception {
        // compute SHA1 of URL for location in archives
        _url = url;
        _jobExecutor = executor;
        _archive = new Archive(url);
    }

    public Archiver(String url) throws Exception {
        this(url, new InThreadJobRunner());
    }

    public void archive() throws IOException {
        LOGGER.info("Beginning archive of " + _url);

        // create a new archive for this
        Archive archive = new Archive(_url);

        // download the content
        var doc = Jsoup.connect(_url).get();

        //TODO - iterate over all the images, links and javascript files and
        // create download jobs for them
        List<DownloadJob> downloadJobs = new LinkedList<>();
        // this gets the css links from the html
        Elements links = doc.select("link");
        for (Element link : links){
            AbstractDownloadJob download = AbstractDownloadJob.getJobFor(link, archive);
            if(download != null) {
                downloadJobs.add(download);
            }
        }

        // get the images from the html
        Elements imgs = doc.select("img");
        for(Element img : imgs){
            AbstractDownloadJob download = AbstractDownloadJob.getJobFor(img, archive);
            if(download != null){
                downloadJobs.add(download);
            }
        }

        // get the javascript from the html
        Elements scripts = doc.select("script");
        for(Element script : scripts){
            AbstractDownloadJob download = AbstractDownloadJob.getJobFor(script, archive);
            if(download != null){
                downloadJobs.add(download);
            }
        }


        // submit download jobs
        _jobExecutor.executeJobs(downloadJobs);

        // update DOM in the main thread (why?)
        for (var downloadJob : downloadJobs) {
            downloadJob.updateElement();
        }

        // store to index.html
        archive.saveFile("index.html", doc.outerHtml().getBytes());

        LOGGER.info("Saved to " + archive.getRoot());
    }

    public String getBaseSHA() {
        return _archive.getBaseSHA();
    }
}
