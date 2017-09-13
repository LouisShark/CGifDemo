#include <jni.h>
#include <string>
#include <gif_lib.h>
#include <malloc.h>
#include <android/log.h>
#include <android/bitmap.h>

#define TAG "LOUIS_LOG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define  argb(a, r, g, b) ( ((a) & 0xff) << 24 ) | ( ((b) & 0xff) << 16 ) | ( ((g) & 0xff) << 8 ) | ((r) & 0xff)
extern "C" {

typedef struct GifBean {
    //当前帧数
    int current_frame;
    //下一帧的播放时长 数组
    int *delay_times;
    //总帧数
    int total_frame;
} GifBean;


JNIEXPORT jlong JNICALL
Java_ilouis_me_gifdemo_GifHandler_loadFile(JNIEnv *env, jclass type, jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    int erro;
    GifFileType *gifFileType = DGifOpenFileName(path, &erro);
    //初始化结构体变量
    DGifSlurp(gifFileType);
    GifBean *gifBean = (GifBean *) malloc(sizeof(GifBean));
    memset(gifBean, 0, sizeof(GifBean));
    gifFileType->UserData = gifBean;
    gifBean->delay_times = (int *) malloc(sizeof(int) * gifFileType->ImageCount);
    memset(gifBean->delay_times, 0, sizeof(int) * gifFileType->ImageCount);
    //播放时长赋值
    ExtensionBlock *extensionBlock;
    for (int i = 0; i < gifFileType->ImageCount; ++i) {
        SavedImage savedImage = gifFileType->SavedImages[i];
        for (int j = 0; j < savedImage.ExtensionBlockCount; ++j) {
            if (GRAPHICS_EXT_FUNC_CODE == savedImage.ExtensionBlocks[j].Function) {
                //图形控制扩展快
                extensionBlock = &savedImage.ExtensionBlocks[j];
                break;
            }
        }
        if (extensionBlock) {
            //第三个字节表示gif编码的第五个字节 第二个表示编码中的第六个字节
            int fra_delay = ((extensionBlock->Bytes[2] << 8) | extensionBlock->Bytes[1]) * 10;
            gifBean->delay_times[i] = fra_delay;
        }
    }
    gifBean->total_frame = gifFileType->ImageCount;
    env->ReleaseStringUTFChars(path_, path);
    return (jlong) gifFileType;
}

JNIEXPORT jint JNICALL
Java_ilouis_me_gifdemo_GifHandler_getH(JNIEnv *env, jclass type, jlong gifPoint) {
    GifFileType *gifFileType = (GifFileType *) gifPoint;
    return gifFileType->SHeight;
}
JNIEXPORT jint JNICALL
Java_ilouis_me_gifdemo_GifHandler_getW(JNIEnv *env, jclass type, jlong gifPoint) {
    GifFileType *gifFileType = (GifFileType *) gifPoint;
    return gifFileType->SWidth;
}

//绘制图形
void drawFrame(GifFileType *gifFileType, GifBean *gifBean, AndroidBitmapInfo *info, void *pixels) {
    //拿到当前帧
    SavedImage savedImage = gifFileType->SavedImages[gifBean->current_frame];
    //图像标识符
    GifImageDesc gifImageDesc = savedImage.ImageDesc;
    //某一个像素位置
    int pointPixel;
    GifByteType gifByteType;
    ColorMapObject *colorMapObject = gifImageDesc.ColorMap;
    GifColorType colorType;
    int *px = (int *) pixels;
    px = (int *) ((char *)px + info->stride * gifImageDesc.Top);
    int *line;
    for (int y = gifImageDesc.Top; y < gifImageDesc.Top + gifImageDesc.Height; ++y) {
        line = px;
        for (int x = gifImageDesc.Left; x < gifImageDesc.Left + gifImageDesc.Width; ++x) {
            pointPixel = (y - gifImageDesc.Top) * gifImageDesc.Width + (x - gifImageDesc.Left);
            //某个像素的argb 但是不能直接用因为返回的是char 但是是4个字节
            gifByteType = savedImage.RasterBits[pointPixel];
            colorType = colorMapObject->Colors[gifByteType];
            line[x] = argb(255, colorType.Red, colorType.Green, colorType.Blue);
        }
        //向下一行
        px = (int *) ((char *)px + info->stride);
    }

}
JNIEXPORT jint JNICALL
Java_ilouis_me_gifdemo_GifHandler_updateFrame(JNIEnv *env, jobject instance, jobject bitmap,
                                              jlong gifPoint) {
    GifFileType *gifFileType = (GifFileType *) gifPoint;
    GifBean *gifBean = (GifBean *) gifFileType->UserData;
    AndroidBitmapInfo bitmapInfo;
    AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    //锁定画布 一副bitmap
    void *pixels;
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    //绘制图形
    drawFrame(gifFileType, gifBean, &bitmapInfo, pixels);
    //控制当前播放量
    gifBean->current_frame += 1;
    LOGE("当前帧%d", gifBean->current_frame);
    if (gifBean->current_frame >= gifBean->total_frame -1) {
        gifBean->current_frame = 0;
        LOGE("重新开始 %d", gifBean->current_frame);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    return gifBean->delay_times[gifBean->current_frame];
}

}