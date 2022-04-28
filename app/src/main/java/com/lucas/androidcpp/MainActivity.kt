package com.lucas.androidcpp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.lucas.androidcpp.databinding.ActivityMainBinding
import com.lucas.androidcpp.jni.JniThread

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.sampleText.setOnClickListener {
            JniThread().createJniThread()
        }
        binding.vMutex.setOnClickListener {
            JniThread().mutexSample()
        }
    }


}