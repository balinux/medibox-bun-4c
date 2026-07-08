# Medibox Bun

Sistem **Smart Medicine Box** berbasis ESP32 + Bun/TypeScript. Terdiri dari:
- **Backend (Bun + Hono)** — REST API yang menyajikan jadwal obat dari database SQLite
- **Firmware (ESP32)** — Mengontrol 2 kotak obat (servo + LED), menerima input tombol manual, dan polling API untuk buka kotak otomatis sesuai jadwal

---

## Hardware Requirements

| Komponen | Jumlah |
|---|---|
| ESP32 (DOIT ESP32 DevKit V1 atau sejenis) | 1 |
| Servo SG90 (atau servo lain) | 2 |
| LED 5mm | 2 |
| Resistor 220Ω (untuk LED) | 2 |
| Push button / tactile switch | 1 |
| Buzzer aktif 5V | 1 |
| LCD I2C 16x2 | 1 |
| Breadboard + kabel jumper | secukupnya |

### Pin Mapping ESP32

| Pin | Terhubung ke |
|---|---|
| GPIO 14 | Buzzer |
| GPIO 13 | Servo 1 (signal) |
| GPIO 12 | Servo 2 (signal) |
| GPIO 26 | LED 1 (anoda → resistor → GPIO) |
| GPIO 27 | LED 2 (anoda → resistor → GPIO) |
| GPIO 4 | Push button (ke GND, pakai INPUT_PULLUP) |
| 3.3V / 5V | VCC komponen sesuai kebutuhan |
| GND | GND semua komponen |

I2C LCD menggunakan **SDA (GPIO21)** dan **SCL (GPIO22)** ESP32, alamat `0x27`.

---

## Struktur Proyek

```
medibox-bun-4c/
├── medibox.ino              # Firmware ESP32 (Arduino IDE / PlatformIO)
├── package.json             # Dependensi Node.js / Bun
├── tsconfig.json
├── medicine.db              # Database SQLite (auto-generated)
└── src/
    ├── index.ts             # Entry point server Hono
    ├── seed.ts              # Seeder data awal
    ├── db/
    │   ├── index.ts         # Inisialisasi koneksi SQLite
    │   └── schema.ts        # Definisi tabel schedules
    ├── services/
    │   └── reminder.ts      # Logika filter jadwal obat 1 jam ke depan
    └── types/
        └── index.ts         # Type Schedule
```

---

## Cara Kerja Sistem

### 1. Backend API (`GET /api/schedule`)
Mengembalikan array jadwal obat yang harus diminum dalam **1 jam ke depan**. Contoh response:

```json
[
  {
    "id": 1,
    "medicine_name": "Paracetamol",
    "schedule_time": "13:00",
    "box_number": "1"
  },
  {
    "id": 2,
    "medicine_name": "Vitamin C",
    "schedule_time": "13:15",
    "box_number": "2"
  }
]
```

### 2. ESP32 — Dua Mode Operasi

#### Mode Manual (Tombol)
- **1 klik** → toggle **Box 1** (servo 0°↔90°, LED1 on/off)
- **2 klik** → toggle **Box 2** (servo 0°↔90°, LED2 on/off)
- Deteksi multi-click dengan jeda 400ms antar ketukan
- Buzzer berbunyi tiap tombol ditekan sebagai konfirmasi

#### Mode Otomatis (API Polling)
- ESP32 melakukan HTTP GET ke `http://<server-ip>:3000/api/schedule` **setiap 60 detik**
- Parse response JSON dengan library `ArduinoJson`
- Untuk setiap item dengan `box_number: "1"` → buka Box 1 (jika belum terbuka)
- Untuk setiap item dengan `box_number: "2"` → buka Box 2 (jika belum terbuka)
- LCD menampilkan nama obat selama 2 detik
- **Tidak ada auto-close** — box ditutup manual via tombol

Kedua mode berjalan **hybrid** (tidak saling menggangu).

---

## Setup & Menjalankan

### Backend (Bun)

```bash
# 1. Install dependensi
bun install

# 2. Seed database dengan data contoh
bun run src/seed.ts

# 3. Jalankan server (auto-reload saat ada perubahan file)
bun run start
```

Server berjalan di `http://localhost:3000`.

### Firmware ESP32

1. Buka `medibox.ino` di **Arduino IDE** atau **PlatformIO**
2. Install library yang diperlukan melalui Library Manager:
   - `LiquidCrystal_I2C` by Frank de Brabander
   - `ESP32Servo` by Kevin Harrington
   - `ArduinoJson` by Benoit Blanchon
3. Sesuaikan konfigurasi WiFi & server pada baris 13-15:
   ```cpp
   const char* ssid       = "your_ssid";
   const char* password   = "your_password";
   const char* serverUrl  = "http://192.168.1.100:3000/api/schedule";
   ```
4. Pilih board **ESP32 Dev Module** dan port yang sesuai
5. Upload ke ESP32

> **Catatan:** Pastikan ESP32 dan server berada di jaringan yang sama.

---

## API Reference

### `GET /api/schedule`

Mengembalikan daftar obat yang jadwalnya dalam 1 jam ke depan.

**Response `200 OK`:**
```json
[
  {
    "id": 1,
    "medicine_name": "Paracetamol",
    "schedule_time": "13:00",
    "box_number": "1"
  }
]
```

**Response jika tidak ada jadwal:**
```json
[]
```

---

## Database

### Tabel `schedules`

| Kolom | Tipe | Keterangan |
|---|---|---|
| `id` | INTEGER (PK) | Auto increment |
| `medicine_name` | TEXT | Nama obat |
| `schedule_time` | TEXT | Format `HH:mm` |
| `box_number` | TEXT | Nomor kotak (`"1"` atau `"2"`) |

### Seed Data

Jalankan `bun run src/seed.ts` untuk mengisi data contoh:

| Obat | Waktu | Box |
|---|---|---|
| Paracetamol | 13:00 | 1 |
| Vitamin C | 13:15 | 2 |
| Amoxicillin | 13:45 | 1 |
| Omeprazole | 15:00 | 2 |

---

## Contoh Skenario

1. **Jam 12:55** — ESP32 polling API → dapat data Paracetamol (13:00, box 1) → **Box 1 terbuka otomatis**, LCD: "BOX 1 OPENED / Paracetamol"
2. **Jam 13:10** — ESP32 polling API → dapat data Vitamin C (13:15, box 2) → **Box 2 terbuka otomatis**, LCD: "BOX 2 OPENED / Vitamin C"
3. **Jam 13:15** — User minum obat lalu tekan tombol **1x** → Box 1 tertutup
4. **Jam 13:20** — User tekan tombol **2x** → Box 2 tertutup
