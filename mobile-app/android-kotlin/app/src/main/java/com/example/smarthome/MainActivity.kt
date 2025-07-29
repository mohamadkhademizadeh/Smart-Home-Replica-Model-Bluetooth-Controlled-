\
package com.example.smarthome

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.content.pm.PackageManager
import android.os.Build
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.nio.charset.Charset
import java.util.UUID
import kotlin.concurrent.thread

class MainActivity : AppCompatActivity() {

    private lateinit var txtStatus: TextView
    private lateinit var txtEnv: TextView
    private var socket: BluetoothSocket? = null
    private var out: OutputStream? = null
    private var `in`: InputStream? = null

    private val SPP_UUID: UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb")
    private val adapter: BluetoothAdapter? = BluetoothAdapter.getDefaultAdapter()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        txtStatus = findViewById(R.id.txtStatus)
        txtEnv = findViewById(R.id.txtEnv)

        findViewById<Button>(R.id.btnConnect).setOnClickListener { connect() }
        findViewById<Button>(R.id.btnAuth).setOnClickListener { sendLine("AUTH 1234") }
        findViewById<Button>(R.id.btnLightOn).setOnClickListener { sendLine("LIGHT ON") }
        findViewById<Button>(R.id.btnLightOff).setOnClickListener { sendLine("LIGHT OFF") }
        findViewById<Button>(R.id.btnDoorOpen).setOnClickListener { sendLine("DOOR OPEN") }
        findViewById<Button>(R.id.btnDoorClose).setOnClickListener { sendLine("DOOR CLOSE") }
        findViewById<Button>(R.id.btnReadEnv).setOnClickListener { sendLine("READ ENV") }
    }

    private fun ensurePerms(): Boolean {
        val needed = mutableListOf<String>()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                needed += Manifest.permission.BLUETOOTH_CONNECT
            }
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
                needed += Manifest.permission.BLUETOOTH_SCAN
            }
        } else {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                needed += Manifest.permission.ACCESS_COARSE_LOCATION
            }
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                needed += Manifest.permission.ACCESS_FINE_LOCATION
            }
        }
        return if (needed.isNotEmpty()) {
            ActivityCompat.requestPermissions(this, needed.toTypedArray(), 1)
            false
        } else true
    }

    private fun connect() {
        if (!ensurePerms()) return
        if (adapter == null || !adapter.isEnabled) {
            alert("Bluetooth is off or not available")
            return
        }
        val paired = adapter.bondedDevices
        if (paired.isEmpty()) {
            alert("No paired devices. Pair with your HC-05 first (PIN 1234/0000).")
            return
        }
        val names = paired.map { it.name ?: it.address }.toTypedArray()
        AlertDialog.Builder(this)
            .setTitle("Select device")
            .setItems(names) { _, which ->
                val dev = paired.elementAt(which)
                connectToDevice(dev)
            }.show()
    }

    private fun connectToDevice(dev: BluetoothDevice) {
        thread {
            try {
                runOnUiThread { txtStatus.text = "Connecting to ${dev.name ?: dev.address}..." }
                val sock = dev.createRfcommSocketToServiceRecord(SPP_UUID)
                sock.connect()
                socket = sock
                out = sock.outputStream
                `in` = sock.inputStream
                runOnUiThread { txtStatus.text = "Connected" }
                listenForLines()
            } catch (e: IOException) {
                runOnUiThread { txtStatus.text = "Connect failed: ${e.message}" }
            }
        }
    }

    private fun listenForLines() {
        val input = `in` ?: return
        val buf = StringBuilder()
        val b = ByteArray(256)
        while (socket?.isConnected == true) {
            val n = try { input.read(b) } catch (e: IOException) { break }
            if (n <= 0) break
            val s = String(b, 0, n, Charset.forName("UTF-8"))
            for (ch in s) {
                if (ch == '\n') {
                    val line = buf.toString().trim()
                    buf.clear()
                    if (line.startsWith("{")) {
                        runOnUiThread { txtEnv.text = line }
                    } else {
                        runOnUiThread { txtStatus.text = line }
                    }
                } else if (ch != '\r') {
                    buf.append(ch)
                }
            }
        }
        runOnUiThread { txtStatus.text = "Disconnected" }
    }

    private fun sendLine(line: String) {
        thread {
            try {
                val o = out ?: return@thread
                o.write((line + "\n").toByteArray(Charset.forName("UTF-8")))
                o.flush()
            } catch (e: IOException) {
                runOnUiThread { txtStatus.text = "Send failed: ${e.message}" }
            }
        }
    }

    private fun alert(msg: String) {
        AlertDialog.Builder(this).setMessage(msg).setPositiveButton("OK", null).show()
    }
}
