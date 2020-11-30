package ai.seventhsense.facedetection;

import android.content.Context;
import android.graphics.Bitmap;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * A Face Detector based on libfacedetection library
 */
public class LFDDetector {
    static {
        System.loadLibrary("seventh-sense-ai-facedetection");
    }
    private long nativeHandle;
    private static HashMap<Long, LFDDetector> instances = new HashMap<>();
    private Context mContext;

    private native void initialize(int maxSide);

    /**
     * Dispose of native objects and release memory
     */
    public native void close();

    private native float[] detectNative(Bitmap image, Bitmap crop);

    private LFDDetector(Context context) {
        this.mContext = context;
    }

    /**
     * Get an instance of a Face Detector based on libfacedetection library. Should only be called from a single thread.
     * @param context application context
     * @param maxSide large images are resized such that their longer side matches this value. Lower values increase speed of the detection while compromising slightly on accuracy.
     * @return instance of the Lib Face Detection Detector
     */
    public static synchronized LFDDetector getInstance(Context context, int maxSide) {
        Long threadId = Thread.currentThread().getId();
        if(!instances.containsKey(threadId)) {
            LFDDetector detector = new LFDDetector(context);
            instances.put(threadId, detector);
            detector.initialize(maxSide);
        }
        return instances.get(threadId);
    }

    /**
     * Get an instance of a Face Detector based on libfacedetection library. Should only be called from a single thread.
     * @param context application context
     * @return instance of the Lib Face Detection Detector with default maxSide of 160
     */
    public static synchronized LFDDetector getInstance(Context context) {
        return LFDDetector.getInstance(context, 160);
    }

    /**
     * Get's the largest face detection in the image
     * @param image the Bitmap to use for face detection. It must be RGBA_8888 or RGB_565
     * @return a detection object if a face was present, null otherwise
     */
    public Detection detect(Bitmap image) {
        Bitmap.Config conf = Bitmap.Config.ARGB_8888; // see other conf types

        // this creates a MUTABLE bitmap
        Bitmap thumbnail = Bitmap.createBitmap(112, 112, conf);

        float[] results = detectNative(image, thumbnail);

        ArrayList<Detection> detections = new ArrayList<>();
        for (int index = 0; index < results.length; index += 15) {
            float left = results[index];
            float top = results[index + 1];
            float right = results[index + 2];
            float bottom = results[index + 3];
            float l1x = results[index + 4];
            float l1y = results[index + 5];
            float l2x = results[index + 6];
            float l2y = results[index + 7];
            float l3x = results[index + 8];
            float l3y = results[index + 9];
            float l4x = results[index + 10];
            float l4y = results[index + 11];
            float l5x = results[index + 12];
            float l5y = results[index + 13];
            float score = results[index + 14];
            Detection detection = new Detection(left, top, right, bottom,
                    l1x, l1y, l2x, l2y, l3x, l3y, l4x, l4y, l5x, l5y, score, thumbnail);
            detections.add(detection);
        }
        if(detections.size() != 0) {
            return detections.get(0);
        }
        return null;
    }
}
