package edu.montana.csci.csci366.archivecat.archiver.jobs;

import edu.montana.csci.csci366.archivecat.archiver.Archive;
import org.jsoup.nodes.Element;

import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;

public class ImageDownloadJob extends AbstractDownloadJob {

    private String _fullPathToImage;

    public ImageDownloadJob(Element element, Archive archive) {
        super(element, archive);
    }

    @Override
    public void downloadResource() throws Exception {
        // TODO implement - hint: use the 'Content-Type' header of the
        //                  response to determine the type of image and
        //                  give it the correct file ending
        var baseName = getArchive().computeSHA1(getURL());

        var client = HttpClient.newHttpClient();
        var request = HttpRequest.newBuilder(URI.create(getURL())).build();
        HttpResponse<byte[]> response = client.send(request, HttpResponse.BodyHandlers.ofByteArray());

        String contentTYpe = response.headers().firstValue("Content-Type").get();// image/gif or image/jpeg
        String s1 = contentTYpe.substring(contentTYpe.indexOf("/")+1);
        s1.trim();

        if(s1.equals("gif")){
            _fullPathToImage = getArchive().saveFile(baseName + ".gif", response.body());
        }else {
            _fullPathToImage = getArchive().saveFile(baseName + ".jpeg", response.body());
        }
    }

    @Override
    public void updateElement() {
        // TODO implement
        getElement().attr("src", "/" + _fullPathToImage);
    }

    @Override
    protected String getURL() {
        // TODO implement
        // getting the absolute path of the img source in an img tag
        return getElement().attr("abs:src");
    }
}