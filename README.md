# Sistem Monitoring pH dan Kelembapan Tanah

## Deskripsi Proyek

Proyek ini merupakan sistem monitoring kondisi tanah yang menggunakan ESP32 sebagai mikrokontroler untuk membaca tingkat pH dan kelembapan tanah. Data dari kedua sensor diproses menjadi nilai pengukuran dan status kondisi tanah.

Hasil pengukuran ditampilkan melalui LCD I2C dan Serial Monitor. Sistem juga terhubung ke platform Blynk melalui jaringan Wi-Fi sehingga data pH, kelembapan, dan status kondisi tanah dapat dikirim ke dashboard Blynk. Apabila pH atau kelembapan berada pada kondisi ekstrem, sistem mengirimkan peringatan (event) ke Blynk.

## Tujuan

- Mengukur tingkat pH tanah menggunakan sensor pH.
- Mengukur tingkat kelembapan tanah menggunakan sensor kelembapan.
- Menampilkan hasil pengukuran secara langsung melalui LCD.
- Mengirimkan data hasil pengukuran ke dashboard Blynk.
- Mengelompokkan kondisi tanah berdasarkan nilai pH dan kelembapannya.
- Mengirimkan peringatan ke Blynk saat kondisi tanah terlalu asam, terlalu basa, terlalu kering, atau terlalu basah.

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
├── sensor.ino
├── config.h
├── adc_util.h
├── sensor_ph.h
├── sensor_kelembapan.h
├── evaluasi.h
├── peringatan.h
├── lcd_display.h
├── cloud.h
├── secrets.h.example
└── README.md
```

## Penjelasan Program

Program utama berada pada file `sensor.ino`, sedangkan pembacaan sensor, evaluasi, peringatan, tampilan LCD, dan koneksi Blynk dipisahkan ke dalam beberapa file header agar kode lebih terstruktur.

### 1. Program Utama (`sensor.ino`)

Mengatur keseluruhan proses kerja sistem: inisialisasi ADC, LCD, Wi-Fi, dan Blynk, kemudian membaca kedua sensor secara berkala.

- Pembacaan sensor dijalankan oleh `BlynkTimer` setiap 1.500 milidetik.
- Pada setiap siklus, hasil pengukuran dicetak ke Serial Monitor (115200 baud), ditampilkan di LCD, diperiksa untuk peringatan, lalu dikirim ke Blynk jika terhubung.
- Jika Wi-Fi gagal terhubung saat startup, sistem tetap berjalan dalam mode lokal (LCD dan Serial Monitor).
- Koneksi Blynk dicoba ulang setiap 5 detik dan koneksi Wi-Fi setiap 10 detik apabila terputus.

### 2. Konfigurasi (`config.h`)

Berisi pengaturan pin, alamat LCD, virtual pin Blynk, jumlah sampel, interval pembacaan, serta batas peringatan.

Konfigurasi pin:

| Fungsi | Pin |
|---|---|
| Sensor pH | GPIO 34 |
| Sensor kelembapan | GPIO 35 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| Alamat LCD I2C | 0x27 |

### 3. Pengolahan ADC (`adc_util.h`)

Membaca nilai analog sensor pH sebanyak 10 sampel, lalu mengurutkannya dengan Bubble Sort. Enam nilai tengah dirata-rata untuk membuang nilai ekstrem sehingga hasil pembacaan lebih stabil.

### 4. Pembacaan Sensor pH (`sensor_ph.h`)

Nilai ADC dikonversi menjadi tegangan, kemudian menjadi nilai pH:

```text
Tegangan = ADC × 3,3 / 4095
pH       = (-7,95 × Tegangan) + 21,24
```

Hasil pengolahan meliputi nilai ADC, tegangan, dan nilai pH.

### 5. Pembacaan Sensor Kelembapan (`sensor_kelembapan.h`)

Membaca nilai analog sensor kelembapan sebanyak 20 sampel, kemudian menghitung rata-ratanya. Nilai ADC dikonversi menjadi persentase berdasarkan nilai kalibrasi:

- Nilai kering (`NILAI_KERING`): 4095.
- Nilai basah (`NILAI_BASAH`): 3270.

Sensor diasumsikan memberi ADC tinggi saat kering dan ADC rendah saat basah. Hasil dibatasi pada rentang 0–100%.

### 6. Evaluasi Kondisi Tanah (`evaluasi.h`)

Mengelompokkan hasil pengukuran menjadi status.

**Klasifikasi pH tanah:**

| Rentang pH | Status |
|---|---|
| pH < 4,5 | Sangat Asam |
| 4,5 – 6,0 | Asam |
| > 6,0 – 7,0 | Netral |
| > 7,0 – 7,5 | Agak Basa |
| pH > 7,5 | Basa |

**Klasifikasi kelembapan tanah:**

| Persentase | Status |
|---|---|
| Di bawah 30% | Kering |
| 30% hingga di bawah 70% | Normal |
| 70% atau lebih | Basah |

### 7. Peringatan (`peringatan.h`)

Mengirim event ke Blynk ketika kondisi tanah berada di luar batas aman. Batas peringatan diatur di `config.h` dan terpisah dari klasifikasi status di atas.

| Event Blynk | Kondisi | Batas default |
|---|---|---|
| `ph_asam` | pH terlalu asam | pH < 4,5 |
| `ph_basa` | pH terlalu basa | pH > 8,5 |
| `tanah_kering` | Tanah sangat kering | Kelembapan < 15% |
| `tanah_basah` | Tanah sangat basah | Kelembapan > 90% |

Aturan pengiriman:

- Kondisi harus muncul **3 pembacaan berturut-turut** (`PERINGATAN_KONFIRMASI`) sebelum peringatan dikirim, supaya lonjakan sesaat tidak memicu peringatan palsu.
- Jika kondisi bertahan, peringatan diulang setiap **30 menit** (`PERINGATAN_ULANG_MS`).
- Nilai pH di luar rentang 0–14 dianggap pembacaan tidak valid dan tidak memicu peringatan.
- Peringatan hanya dikirim saat perangkat terhubung ke Blynk.

### 8. Tampilan LCD (`lcd_display.h`)

Mengatur tampilan pada LCD I2C 16x2. Selama monitoring, LCD menampilkan dua baris yang diperbarui setiap siklus pembacaan:

```text
pH:6,5 Netral
Lmb:45% Normal
```

Status "Sangat Asam" disingkat menjadi "Sgt Asam" agar muat pada 16 karakter. Saat startup, LCD juga menampilkan status koneksi Wi-Fi dan Blynk.

### 9. Koneksi Blynk (`cloud.h`)

Mengatur komunikasi antara ESP32 dan Blynk. Data dikirim melalui empat virtual pin:

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

### 2. Konfigurasi Dashboard Blynk

Pada template Blynk, buat empat datastream sesuai tabel virtual pin di atas (V0 dan V1 bertipe angka, V2 dan V3 bertipe string). Untuk peringatan, buat empat event dengan kode persis seperti berikut: `ph_asam`, `ph_basa`, `tanah_kering`, dan `tanah_basah`.

### 3. Konfigurasi Wi-Fi dan Token

Salin file `secrets.h.example` dan ubah nama salinannya menjadi `secrets.h`. Isi dengan data berikut:

- `BLYNK_TEMPLATE_ID`
- `BLYNK_TEMPLATE_NAME`
- `BLYNK_AUTH_TOKEN`
- SSID Wi-Fi (`ssid`)
- Password Wi-Fi (`pass`)

File `secrets.h` berisi informasi rahasia, sehingga tidak boleh diunggah ke repository. Tambahkan `secrets.h` ke dalam `.gitignore`.

### 4. Upload Program

1. Pastikan seluruh file berada dalam satu folder sketch. Arduino IDE mengharuskan nama folder sama dengan nama file `.ino`, sehingga folder sketch harus bernama `sensor`.
2. Buka file `sensor.ino` menggunakan Arduino IDE.
3. Pilih board ESP32 yang digunakan.
4. Pilih port perangkat ESP32.
5. Hubungkan ESP32 ke komputer.
6. Upload program ke perangkat.

### 5. Menjalankan Monitoring

Setelah program berhasil diunggah, ESP32 akan melakukan proses berikut:

1. Menginisialisasi ADC, sensor, dan LCD.
2. Menghubungkan perangkat ke jaringan Wi-Fi.
3. Menghubungkan perangkat ke Blynk.
4. Membaca nilai pH dan kelembapan tanah setiap 1,5 detik.
5. Mengolah hasil pembacaan menjadi nilai pengukuran dan status kondisi tanah.
6. Menampilkan hasil pada LCD dan Serial Monitor.
7. Memeriksa kondisi ekstrem dan mengirim peringatan ke Blynk bila diperlukan.
8. Mengirimkan data ke dashboard Blynk apabila koneksi tersedia.

## Catatan Kalibrasi

Nilai kalibrasi dapat disesuaikan melalui file berikut:

- `sensor_ph.h` untuk kalibrasi pH (`PH_SLOPE` dan `PH_CALIBRATION`).
- `sensor_kelembapan.h` untuk nilai ADC kondisi kering dan basah (`NILAI_KERING` dan `NILAI_BASAH`).
- `config.h` untuk batas peringatan pH dan kelembapan.

Kalibrasi pH sebaiknya dilakukan dengan larutan buffer (misalnya pH 4 dan pH 7), sedangkan kalibrasi kelembapan dengan mengukur ADC saat sensor di udara kering dan saat sensor terendam air. Kalibrasi perlu dilakukan berdasarkan hasil pengujian agar pengukuran sesuai dengan kondisi sebenarnya.

## Penutup

Dengan adanya sistem monitoring ini, pengukuran pH dan kelembapan tanah dapat dilakukan secara elektronik menggunakan ESP32. Informasi hasil pengukuran dapat dilihat melalui LCD maupun dashboard Blynk, dan peringatan otomatis membantu pengguna segera mengetahui bila kondisi tanah perlu ditangani.