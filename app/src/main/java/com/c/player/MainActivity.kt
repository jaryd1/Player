package com.c.player

import android.Manifest
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.os.CountDownTimer
import android.os.Environment
import android.view.View
import android.view.WindowManager
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*

class MainActivity : AppCompatActivity() {
    var userSeeking = false
    var time:CountDownTimer? = null
    var test = false
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
        window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN)
        setContentView(R.layout.activity_main)
        // Example of a call to a native method
        bar.setOnSeekBarChangeListener(object:SeekBar.OnSeekBarChangeListener{
            override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                    if(userSeeking){
                        seekBar?.progress = progress
                        ct.setText(generateStandardTime(progress))
                        btn.setText(progress.toString())
                    }
            }

            override fun onStartTrackingTouch(seekBar: SeekBar?) {
                userSeeking = true
            }

            override fun onStopTrackingTouch(seekBar: SeekBar?) {
                val position = seekBar!!.progress/seekBar!!.max.toFloat()*surface.duration
                surface.SeekPosition(position)
                userSeeking = false;
            }

        })
        btn.setOnClickListener {

        }
        btn.visibility = View.GONE

        surface.resigerCall(object :MySurface.TimeCallBack{
            override fun onGetVidoeSize(width: Int, height: Int) {
                runOnUiThread{
//                    val screenWidth = windowManager.defaultDisplay.width
//                    var desireHeight = screenWidth*height.toFloat()/width.toFloat()
//                    Log.e("desire height",desireHeight.toString())
//                    val layoutparams = surface.layoutParams
//                    layoutparams.height = desireHeight.toInt()
//                    surface.layoutParams = layoutparams
                }

            }

            override fun showLoading() {
                runOnUiThread {
                }
            }

            override fun hideLoading() {
                runOnUiThread {
                }
            }

            override fun callbackDuration(duration: Float) {
                bar?.max = duration.toInt()
            }

            override fun onProgressCallBack(currentProgress: Long,bufferProgress:Float, duration: Float) {
                runOnUiThread {
                    if(test.not()){
                        time?.start();
                        test = true;
                    }
                    if(userSeeking.not()) {
                        val pro = (currentProgress / 1000).toInt()
                        bar.progress = pro
                        bar.secondaryProgress = bufferProgress.toInt()
                        ct.setText(generateStandardTime(currentProgress.toInt()/1000))
                        dt.setText(generateStandardTime(duration.toInt()))
                    }
                }
            }

        })
    }

    override fun onResume() {
        super.onResume()
        check()
//        surface?.prepare()
        surface.setDataSource(
            Environment.getExternalStorageDirectory().absolutePath
        +"/tc.flv"
        )

//        surface!!.setDataSource(
//           "http://vfx.mtime.cn/Video/2019/02/04/mp4/190204084208765161.mp4"
//        )
    }

    fun generateStandardTime(totalSeconds: Int): String {
        val seconds = totalSeconds % 60
        val minutes = totalSeconds / 60 % 60
        val hours = minutes / 3600 % 60
        return if (hours > 0) {
            String.format("%02d:%02d:%02d", hours, minutes, seconds)
        } else {
            String.format("%02d:%02d", minutes, seconds)
        }
    }


    private fun check() {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.M){
            if(checkSelfPermission(android.Manifest.permission
                    .READ_EXTERNAL_STORAGE) !=PackageManager.PERMISSION_GRANTED){
                requestPermissions(arrayOf(Manifest.permission.READ_EXTERNAL_STORAGE),100)
            }
            if(checkSelfPermission(android.Manifest.permission
                            .WRITE_EXTERNAL_STORAGE) !=PackageManager.PERMISSION_GRANTED){
                requestPermissions(arrayOf(Manifest.permission.WRITE_EXTERNAL_STORAGE),100)
            }
            if(checkSelfPermission(android.Manifest.permission
                    .CAMERA) !=PackageManager.PERMISSION_GRANTED){
                requestPermissions(arrayOf(Manifest.permission.CAMERA),100)
            }
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */

}
