package ilouis.me.gifdemo;

import android.graphics.Bitmap;

/**
 * Created by LouisShark on 2017/9/12.
 * this is on ilouis.me.gifdemo.
 */

public class GifHandler {
    private long gifPoint;


    public GifHandler(String path) {
        this.gifPoint = load(path);
    }

    public long load(String path) {
        gifPoint = loadFile(path);
        return gifPoint;
    }

    //    long  C 指针
    private   native  long loadFile(String Path);

    private   native  int getW(long gifPoint);

    private   native  int getH(long gifPoint);
    //绘制函数
    private  native int updateFrame(Bitmap bitmap,long gifPoint);
    public int getWidth() {
        return getW(gifPoint);
    }
    public int getHeight() {
        return getH(gifPoint);

    }
    public int updateFrame(Bitmap bitmap) {
        return updateFrame(bitmap,gifPoint);
    }

    static {
        System.loadLibrary("gif");
    }

}
