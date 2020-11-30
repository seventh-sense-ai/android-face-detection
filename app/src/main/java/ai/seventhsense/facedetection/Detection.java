package ai.seventhsense.facedetection;

import android.graphics.Bitmap;
import android.graphics.Point;

/**
 * Class representing a single face detection
 */
public class Detection {
    /**
     * Left x1 of the bounding box containing the face
     */
    public int left;

    /**
     * Top y1 of the bounding box containing the face
     */
    public int top;

    /**
     * Right x2 of the bounding box containing the face
     */
    int right;

    /**
     * Bottom y2 of the bounding box containing the face
     */
    int bottom;

    /**
     * Area (in pixels) of the bounding box
     */
    public int area;

    /**
     * Score of the detection (0.00 to 1.00)
     */
    public float score;

    /**
     * Co-ordinates of the eye towards the viewers left in the image
     */
    public Point leftEye;

    /**
     * Co-ordinates of the eye towards the viewers right in the image
     */
    public Point rightEye;

    /**
     * Co-ordinates of the nose
     */
    public Point nose;

    /**
     * Co-ordinates of the mouth corner towards the viewers left in the image
     */
    public Point leftMouth;

    /**
     * Co-ordinates of the mouth corner towards the viewers right in the image
     */
    public Point  rightMouth;

    /**
     * Thumbnail of the detected face that can be efficiently accepted by Seventh Sense' Facial Recognition algorithms
     */
    public Bitmap thumbnail;

    Detection(float left, float top, float right, float bottom,
                     float l1x, float l1y, float l2x, float l2y, float l3x, float l3y,
                     float l4x, float l4y, float l5x, float l5y, float score, Bitmap thumbnail) {
        this.left = Math.round(left);
        this.top = Math.round(top);
        this.right = Math.round(right);
        this.bottom = Math.round(bottom);
        this.score = score / 100f;
        this.thumbnail = thumbnail;
        leftEye = new Point(Math.round(l1x), Math.round(l1y));
        rightEye = new Point(Math.round(l2x), Math.round(l2y));
        nose = new Point(Math.round(l3x), Math.round(l3y));
        leftMouth = new Point(Math.round(l4x), Math.round(l4y));
        rightMouth = new Point(Math.round(l5x), Math.round(l5y));

        area = (this.bottom - this.top) * (this.right - this.left);
    }
}
