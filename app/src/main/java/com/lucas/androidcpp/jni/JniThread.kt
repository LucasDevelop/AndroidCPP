package com.lucas.androidcpp.jni

class JniThread {
    companion object{
        init {
            System.loadLibrary("androidcpp")
        }
    }

    external fun createJniThread()

    external fun mutexSample()
}