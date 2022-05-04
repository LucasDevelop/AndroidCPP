package com.lucas.androidcpp

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.lucas.androidcpp.databinding.ActivityMainBinding
import com.lucas.androidcpp.jni.JniThread
import kotlin.reflect.KClass

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        start(AudioPlayerActivity::class.java)

//        binding.sampleText.setOnClickListener {
//            JniThread().createJniThread()
//        }
//        binding.vMutex.setOnClickListener {
//            JniThread().mutexSample()
//        }
        binding.vAudioPlayer.setOnClickListener {
            startActivity(Intent(this,AudioPlayerActivity::class.java))
        }
    }

    fun start(clazz:Class<*>){
        startActivity(Intent(this,clazz))
    }


}