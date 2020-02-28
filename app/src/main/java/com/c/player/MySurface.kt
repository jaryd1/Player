package com.c.player

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.view.Surface
import android.view.SurfaceHolder

class MySurface(mContext: Context,attrs: AttributeSet?):GLSurfaceView(mContext,attrs),SurfaceHolder.Callback
            {


    constructor(mContext: Context):this(mContext,null)


    var surface:Surface? = null
    var path:String?= null
    var duration:Float = 0f
    var callBack:TimeCallBack? = null
    companion object {

        // Used to load the 'native-lib' library on application startup.
        init {
            System.loadLibrary("player")

        }

    }

    init {
        holder.addCallback(this)
    }

    fun resigerCall(callBack: TimeCallBack){
        this.callBack = callBack
    }

     private fun call(currentProgress: Long,bufferProgress: Float,duration: Float){
        callBack?.onProgressCallBack(currentProgress, bufferProgress, duration)
    }

    private fun showLoading(){
        callBack?.showLoading()
    }

    private fun hideLoading(){
        callBack?.hideLoading()
    }

    private fun getVideoSize(width: Int,height: Int){
        callBack?.onGetVidoeSize(width, height)
    }

    override fun surfaceChanged(p0: SurfaceHolder?, p1: Int, p2: Int, p3: Int) {
        _updateDisplaySize()
    }

    override fun surfaceDestroyed(p0: SurfaceHolder?) {
    }

    override fun surfaceCreated(p0: SurfaceHolder?) {
        _setSurface(p0!!.surface)
    }

    fun setDataSource(source:String){
        _setDataSource(source)
        _prepareAsync()
    }

    fun prepare(){
        _prepareAsync()
    }


    fun updataDuration(duration:Float){
        this.duration = duration;
        callBack?.callbackDuration(duration)
    }



    private external fun _setSurface(surface: Surface):Int
    private external fun _prepareAsync():Int
    private external fun _setDataSource(source:String):Int
    private external fun _updateDisplaySize()
    external fun SeekPosition(position:Float)
    external fun play():Int

    interface TimeCallBack{
        fun onProgressCallBack(currentProgress:Long, bufferProgress:Float,duration: Float)
        fun callbackDuration(duration: Float)
        fun showLoading()
        fun hideLoading()
        fun onGetVidoeSize(width:Int,height:Int);
    }
}