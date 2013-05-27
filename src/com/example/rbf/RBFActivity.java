package com.example.rbf;
import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.highgui.Highgui;
import org.opencv.imgproc.Imgproc;


import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.os.Bundle;
import android.os.Environment;
import android.app.Activity;

import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.util.Log;
import android.view.KeyEvent;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;
 
public class RBFActivity extends Activity implements CvCameraViewListener2 {

	private CameraBridgeViewBase mOpenCvCameraView;
	private Mat mRgba, mGray;
	private TextToSpeech tts;
	//public static Integer lines=1;
   @Override
   public void onCreate(Bundle savedInstanceState) {
      super.onCreate(savedInstanceState);
     		setContentView(R.layout.activity_rbf);	
      getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
      mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.nativeCamera);
      mOpenCvCameraView.setCvCameraViewListener(this);
      tts=new TextToSpeech(this, new OnInitListener() {
			
  		

			@Override
			public void onInit(int status) {
				// TODO Auto-generated method stub
				switch(status){
				case TextToSpeech.SUCCESS:
					int result = tts.setLanguage(Locale.US);

					if (result == TextToSpeech.LANG_MISSING_DATA
							|| result == TextToSpeech.LANG_NOT_SUPPORTED) {
						Log.e("TTS", "This Language is not supported");
					} else {
						
										
						
					}
				break;
				default:
					Log.e("TTS","Loading Error");
				}
					
			}
		});
	
	
   }

	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i("OpenCV", "OpenCV loaded successfully");
                    System.loadLibrary("myjni");
                    mOpenCvCameraView.enableView();
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };
   
   @Override
   public void onPause()
   {
       super.onPause();
       if (mOpenCvCameraView != null)
           mOpenCvCameraView.disableView();
   }

   @Override
   public void onResume()
   {
       super.onResume();
       OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_3, this, mLoaderCallback);
   }

   public void onDestroy() {
       super.onDestroy();
       if (mOpenCvCameraView != null)
           mOpenCvCameraView.disableView();
   }
	
	
	@Override
	public void onCameraViewStarted(int width, int height) {
		mRgba = new Mat(height, width, CvType.CV_8UC4);
		mGray = new Mat(height, width, CvType.CV_8UC1);
		
	}

	@Override
	public void onCameraViewStopped() {
		mRgba.release();
	}
	public native int processingImage(long mat,long mat2);
	public native void perspectiveImage(long mat);
	@Override
	public Mat onCameraFrame(CvCameraViewFrame inputFrame) {
		mRgba = inputFrame.rgba();
		mGray = inputFrame.gray();
		Mat temp = new Mat(mOpenCvCameraView.getHeight(), mOpenCvCameraView.getWidth(), CvType.CV_8UC4);
		
		  
		  
		if(!tts.isSpeaking()){
		int direction=processingImage(mRgba.getNativeObjAddr(),mGray.getNativeObjAddr());
		
		if(direction==0){
			tts.speak("Capturing Please Wait", TextToSpeech.QUEUE_FLUSH, null);
			 File path = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);
			  String filename = "Anuj.png";
			  File file = new File(path, filename);
			  if(Highgui.imwrite(file.toString(),mRgba))
				  Log.d("Saving", "SUCCESS");
			  else
			    Log.e("Saving", "Fail");
			 
			  
			  
		}else if(direction==3)
			tts.speak("Right.", TextToSpeech.QUEUE_ADD, null);
		else if(direction==4)
			tts.speak("Bottom", TextToSpeech.QUEUE_ADD, null);
		else if(direction==6)
			tts.speak("Left..", TextToSpeech.QUEUE_ADD, null);
		else if(direction==5)
			tts.speak("Top.. ", TextToSpeech.QUEUE_ADD, null);
		else if(direction==111)
			tts.speak("Up... ", TextToSpeech.QUEUE_ADD, null);
		else if(direction==-111)
			tts.speak("Down..", TextToSpeech.QUEUE_ADD, null);
			
		}
		
		return mRgba;
	}

	@Override
	public boolean onKeyLongPress(int keyCode, KeyEvent event) {
		// TODO Auto-generated method stub
		
		if (keyCode == KeyEvent.KEYCODE_MENU) {
		
		}
		return super.onKeyLongPress(keyCode, event);
	}
	 

}
