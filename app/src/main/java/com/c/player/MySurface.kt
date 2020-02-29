package com.c.player

import android.content.Context
import android.graphics.SurfaceTexture
import android.util.AttributeSet
import android.view.Surface
import android.view.TextureView

class MySurface(mContext: Context,attrs: AttributeSet?):TextureView(mContext,attrs),TextureView.SurfaceTextureListener
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
        surfaceTextureListener = this
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

    override fun onSurfaceTextureSizeChanged(
        surface: SurfaceTexture?,
        width: Int,
        height: Int
    ) {
        _updateDisplaySize()
    }

    override fun onSurfaceTextureUpdated(surface: SurfaceTexture?) {
    }

    override fun onSurfaceTextureDestroyed(surface: SurfaceTexture?): Boolean {
        return true
    }

    override fun onSurfaceTextureAvailable(
        surface: SurfaceTexture?,
        width: Int,
        height: Int
    ) {
        _setSurface(Surface(surface!!))
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