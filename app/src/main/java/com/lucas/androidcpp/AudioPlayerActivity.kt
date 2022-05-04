package com.lucas.androidcpp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import com.lucas.androidcpp.jni.AudioPCMPlayer
import com.lucas.androidcpp.jni.AudioPlayer
import java.io.File

class AudioPlayerActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_audio_player)
        requestPermissions(arrayOf(
            android.Manifest.permission.MODIFY_AUDIO_SETTINGS,
//            android.Manifest.permission.INTERNET,
//            android.Manifest.permission.RECORD_AUDIO,
        ),33)

        findViewById<View>(R.id.v_play).setOnClickListener {
            AudioPCMPlayer.createAudioPlayer()
//            AudioPCMPlayer.setFile(File(filesDir,"16000_s16le_c2.pcm").absolutePath)
            AudioPCMPlayer.setFile(File(filesDir,"pcm_cache.pcm").absolutePath)
            AudioPCMPlayer.play()
        }

        findViewById<View>(R.id.v_play_mp3).setOnClickListener {
            AudioPlayer.createAudioPlayer()
            AudioPlayer.setFile(File(filesDir,"danceMonkey.mp3").absolutePath)
            AudioPlayer.play()
        }

    }

    override fun onDestroy() {
        super.onDestroy()
        AudioPCMPlayer.stop()
        AudioPCMPlayer.release()
        AudioPlayer.stop()
        AudioPlayer.release()
    }
}