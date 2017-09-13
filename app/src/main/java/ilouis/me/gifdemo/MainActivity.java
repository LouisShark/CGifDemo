package ilouis.me.gifdemo;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    private InputStream inputStream = null;
    private FileOutputStream outputStream = null;
    Bitmap bitmap;
    GifHandler gifHandler;
    ImageView imageView;
    private File file;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        imageView= (ImageView) findViewById(R.id.image);
        Log.d("MainActivity", "Environment.getExternalStorageDirectory():" + Environment.getExternalStorageDirectory());
        file = new File(Environment.getExternalStorageDirectory(), "demo.gif");
        if (!file.exists()) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        int bytesum = 0;
                        int byteread = 0;
                        file = new File(Environment.getExternalStorageDirectory(), "demo.gif");
                        inputStream = getResources().getAssets().open("demo.gif");
                        outputStream = new FileOutputStream(file);
                        byte[] buffer = new byte[1024];
                        while ((byteread = inputStream.read(buffer)) != -1) {
                            bytesum += byteread;
                            System.out.println(bytesum);
                            outputStream.write(buffer, 0, byteread);
                        }
                        inputStream.close();
                    } catch (Exception e) {
                        System.out.println("复制单个文件操作出错");
                        e.printStackTrace();
                    }
                }
            }).start();
        }
    }
    Runnable runnable = new Runnable() {
        public void run() {
            gifHandler = new GifHandler(file.getAbsolutePath());
            Log.i("LouisShark", "ndkLoadGif: " + file.getAbsolutePath());
            //得到gif   width  height  生成Bitmap
            int width = gifHandler.getWidth();
            int height=gifHandler.getHeight();
            bitmap= Bitmap.createBitmap(width,height, Bitmap.Config.ARGB_8888);
            int delay=gifHandler.updateFrame(bitmap);
            handler.sendEmptyMessageDelayed(1, delay);
        }
    };
    public void ndkLoadGif(View view) {
        new Thread(runnable).start();
    }

    Handler handler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            int delay=gifHandler.updateFrame(bitmap);
            handler.sendEmptyMessageDelayed(1, delay);
            imageView.setImageBitmap(bitmap);
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }
}
