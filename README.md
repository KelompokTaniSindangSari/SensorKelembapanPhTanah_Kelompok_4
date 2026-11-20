# Sistem Monitoring pH dan Kelembapan Tanah

## Deskripsi Proyek

Proyek ini merupakan sistem monitoring kondisi tanah yang menggunakan ESP32 sebagai mikrokontroler untuk membaca tingkat pH dan kelembapan tanah. Data yang diperoleh dari kedua sensor diproses untuk menghasilkan nilai pengukuran dan status kondisi tanah.

Hasil pengukuran ditampilkan melalui LCD I2C dan Serial Monitor. Selain itu, sistem juga terhubung dengan platform Blynk melalui jaringan Wi-Fi sehingga data pH, kelembapan, dan status kondisi tanah dapat dikirim ke dashboard Blynk.

## Tujuan

- Mengukur tingkat pH tanah menggunakan sensor pH.
- Mengukur tingkat kelembapan tanah menggunakan sensor kelembapan.
- Menampilkan hasil pengukuran secara langsung melalui LCD.
- Mengirimkan data hasil pengukuran ke dashboard Blynk.
- Mengelompokkan kondisi tanah berdasarkan nilai pH dan kelembapannya.

## Komponen yang Digunakan

1. ESP32 sebagai mikrokontroler utama.
2. Sensor pH tanah untuk mengukur tingkat keasaman tanah.
3. Sensor kelembapan tanah untuk mengukur kadar kelembapan tanah.
4. LCD I2C 16x2 untuk menampilkan hasil pengukuran.
5. Kabel jumper dan komponen pendukung.
6. Jaringan Wi-Fi untuk menghubungkan ESP32 dengan Blynk.

## Struktur Folder

```text
SensorKelembapanPhTanah_Kelompok_4/
│
├── docs/
│   ├── catatan_kalibrasi.md
│   ├── panduan_blynk.md
│   ├── panduan_mitra.md
│   └── wiring.md
│
├── firmware/
│   ├── senorkelembaban.ino
│   ├── config.h
│   ├── adc_util.h
│   ├── sensor_ph.h
│   ├── sensor_kelembapan.h
│   ├── evaluasi.h
│   ├── lcd_display.h
│   ├── cloud.h
│   └── secrets.h.example
│
├── .gitignore
└── README.md
```

## Penjelasan Program

Program utama berada pada file `senorkelembaban.ino`, sedangkan fungsi pembacaan sensor, pengolahan data, tampilan LCD, dan koneksi Blynk dipisahkan ke dalam beberapa file header agar kode lebih terstruktur.

### 1. Program Utama (`senorkelembaban.ino`)

Mengatur keseluruhan proses kerja sistem, mulai dari inisialisasi ESP32, pembacaan kedua sensor, pengolahan data, hingga pengiriman hasil monitoring.

Program menggunakan BlynkTimer untuk menjalankan proses pembacaan sensor secara berkala setiap 1.500 milidetik.

### 2. Konfigurasi (`config.h`)

Berisi pengaturan pin perangkat keras, alamat LCD, virtual pin Blynk, serta interval pembacaan sensor.

Konfigurasi pin yang digunakan:
- Sensor pH: GPIO 34.
- Sensor kelembapan: GPIO 35.
- LCD SDA: GPIO 21.
- LCD SCL: GPIO 22.
- Alamat LCD I2C: 0x27.

### 3. Pengolahan ADC (`adc_util.h`)

Berfungsi membaca nilai analog dari sensor pH. Pembacaan dilakukan sebanyak 10 sampel, kemudian diurutkan menggunakan metode Bubble Sort.

Enam nilai tengah digunakan untuk menghitung rata-rata agar hasil pembacaan lebih stabil.

### 4. Pembacaan Sensor pH (`sensor_ph.h`)

Mengolah nilai ADC dari sensor pH menjadi nilai tegangan dan kemudian dikonversi menjadi nilai pH menggunakan rumus kalibrasi:

`pH = (-5.70 × Tegangan) + 21.24`

Hasil pengolahan meliputi nilai ADC, tegangan, dan nilai pH tanah.

### 5. Pembacaan Sensor Kelembapan (`sensor_kelembapan.h`)

Membaca nilai analog sensor kelembapan sebanyak 20 sampel, kemudian menghitung nilai rata-ratanya.

Nilai ADC dikonversi menjadi persentase kelembapan berdasarkan nilai kalibrasi:
- Nilai kering: 3500.
- Nilai basah: 1500.

Hasil pengukuran dibatasi pada rentang 0–100%.

### 6. Evaluasi Kondisi Tanah (`evaluasi.h`)

Mengelompokkan hasil pengukuran berdasarkan nilai pH dan persentase kelembapan.

**Klasifikasi pH tanah:**

| Rentang pH | Status |
|---|---|
| pH < 4,5 | Sangat Asam |
| 4,5–6,0 | Asam |
| > 6,0–7,0 | Netral |
| > 7,0–7,5 | Agak Basa |
| pH > 7,5 | Basa |

**Klasifikasi kelembapan tanah:**

| Persentase | Status |
|---|---|
| Di bawah 30% | Kering |
| 30% hingga di bawah 70% | Normal |
| 70% atau lebih | Basah |

### 7. Tampilan LCD (`lcd_display.h`)

Mengatur tampilan informasi pada LCD I2C 16x2.

LCD menampilkan tiga halaman informasi secara bergantian:
1. Nilai pH, tegangan, dan status pH.
2. Nilai kelembapan dan status kelembapan.
3. Nilai ADC sensor pH dan sensor kelembapan.

Pergantian halaman dilakukan setiap 3 detik.

### 8. Koneksi Blynk (`cloud.h`)

Mengatur komunikasi antara ESP32 dan platform Blynk untuk mengirimkan hasil pengukuran sensor ke dashboard.

Data dikirim menggunakan empat virtual pin:

| Virtual Pin | Data |
|---|---|
| V0 | Nilai pH |
| V1 | Persentase kelembapan |
| V2 | Status pH |
| V3 | Status kelembapan |

## Cara Instalasi dan Penggunaan

### 1. Persiapan Perangkat

Pastikan perangkat berikut sudah tersedia:
- Arduino IDE.
- Board ESP32.
- Library Blynk.
- Library LiquidCrystal_I2C.

Library WiFi, Wire, dan Arduino sudah tersedia melalui dukungan board ESP32.

### 2. Konfigurasi Blynk dan Wi-Fi

Masuk ke folder `firmware`, kemudian salin file `secrets.h.example` dan ubah nama salinannya menjadi `secrets.h`.

Isi konfigurasi berikut sesuai dengan akun dan jaringan yang digunakan:

- BLYNK_TEMPLATE_ID
- BLYNK_TEMPLATE_NAME
- BLYNK_AUTH_TOKEN
- SSID Wi-Fi
- Password Wi-Fi

Pastikan file `secrets.h` tidak diunggah ke repository karena berisi informasi rahasia.

### 3. Upload Program

1. Buka file `senorkelembaban.ino` menggunakan Arduino IDE.
2. Pastikan seluruh file header berada dalam folder yang sama.
3. Pilih board ESP32 yang digunakan.
4. Pilih port perangkat ESP32.
5. Hubungkan ESP32 ke komputer.
6. Upload program ke perangkat.

### 4. Menjalankan Monitoring

Setelah program berhasil diunggah, ESP32 akan melakukan proses berikut:

1. Menginisialisasi sensor dan LCD.
2. Menghubungkan perangkat ke jaringan Wi-Fi.
3. Menghubungkan perangkat ke Blynk.
4. Membaca nilai pH dan kelembapan tanah secara berkala.
5. Mengolah hasil pembacaan menjadi nilai pengukuran dan status kondisi tanah.
6. Menampilkan hasil pada LCD dan Serial Monitor.
7. Mengirimkan data ke dashboard Blynk apabila koneksi tersedia.

Jika koneksi Wi-Fi atau Blynk terputus, sistem akan mencoba menghubungkan kembali secara otomatis.

## Catatan Kalibrasi

Nilai kalibrasi sensor dapat disesuaikan melalui file berikut:

- `sensor_ph.h` untuk pengaturan kalibrasi pH.
- `sensor_kelembapan.h` untuk pengaturan nilai ADC kondisi kering dan basah.

Kalibrasi perlu dilakukan berdasarkan hasil pengujian sensor agar pengukuran lebih sesuai dengan kondisi sebenarnya.

## Penutup

Dengan adanya sistem monitoring ini, pengukuran pH dan kelembapan tanah dapat dilakukan secara elektronik menggunakan ESP32. Informasi hasil pengukuran dapat dilihat melalui LCD maupun dashboard Blynk sehingga memudahkan proses pemantauan kondisi tanah.