package com.lucas.androidcpp.jni

object AudioPlayer {
    init {
        System.loadLibrary("androidcpp")
    }
    external fun createAudioPlayer()
    external fun setFile(filePath:String)

    external fun play()
    external fun pause()
    external fun resume()
    external fun stop()
    external fun release()
}