#ifndef PERINGATAN_H
#define PERINGATAN_H

#include <Arduino.h>
#include <BlynkSimpleEsp32.h>

#include "config.h"

struct PelacakPeringatan {
    String kode;
    int hitungan;
    unsigned long terakhirKirim;
    bool sudahKirim;
};

PelacakPeringatan pelacakPH = {"", 0, 0, false};
PelacakPeringatan pelacakLembap = {"", 0, 0, false};

// Mengirim event ke Blynk bila kondisi bertahan beberapa pembacaan,
// lalu mengulang sesuai PERINGATAN_ULANG_MS.
void prosesPeringatan(PelacakPeringatan &pelacak, String kode, String pesan) {

    if (kode != pelacak.kode) {
        pelacak.kode = kode;
        pelacak.hitungan = 0;
        pelacak.sudahKirim = false;
    }

    if (kode == "") {
        return;
    }

    if (pelacak.hitungan < PERINGATAN_KONFIRMASI) {
        pelacak.hitungan++;
    }

    if (pelacak.hitungan < PERINGATAN_KONFIRMASI) {
        return;
    }

    unsigned long sekarang = millis();

    bool jatuhTempo =
        !pelacak.sudahKirim ||
        (sekarang - pelacak.terakhirKirim >= PERINGATAN_ULANG_MS);

    if (!jatuhTempo || !Blynk.connected()) {
        return;
    }

    Blynk.logEvent(kode.c_str(), pesan.c_str());

    pelacak.terakhirKirim = sekarang;
    pelacak.sudahKirim = true;

    Serial.print("PERINGATAN dikirim: ");
    Serial.println(kode);
}

// Nilai pH di luar 0-14 dianggap pembacaan tidak valid dan tidak memicu peringatan.
void peringatanPH(float ph, String status) {

    String kode = "";
    String pesan = "";

    if (!isnan(ph) && ph >= 0.0 && ph <= 14.0) {

        if (ph < PH_PERINGATAN_ASAM) {
            kode = "ph_asam";
            pesan = "pH " + String(ph, 2) + " terlalu asam (" + status + ")";
        }
        else if (ph > PH_PERINGATAN_BASA) {
            kode = "ph_basa";
            pesan = "pH " + String(ph, 2) + " terlalu basa (" + status + ")";
        }
    }

    prosesPeringatan(pelacakPH, kode, pesan);
}

void peringatanKelembapan(float persen, String status) {

    String kode = "";
    String pesan = "";

    if (persen < LEMBAP_PERINGATAN_KERING) {
        kode = "tanah_kering";
        pesan = "Kelembapan " + String(persen, 1) + "% kering banget (" + status + ")";
    }
    else if (persen > LEMBAP_PERINGATAN_BASAH) {
        kode = "tanah_basah";
        pesan = "Kelembapan " + String(persen, 1) + "% basah banget (" + status + ")";
    }

    prosesPeringatan(pelacakLembap, kode, pesan);
}

#endif