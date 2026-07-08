# Medibox Bun

Aplikasi pengingat minum obat sederhana menggunakan Bun dan SQLite.

## Struktur Proyek

```text
src/
├── db/
│   ├── index.ts        # Inisialisasi Database
│   └── schema.ts       # Definisi Tabel/Schema
├── services/
│   └── reminder.ts     # Logika bisnis pengingat obat
├── types/
│   └── index.ts        # Definisi tipe TypeScript
├── index.ts            # Entry point utama
└── seed.ts             # Script untuk mengisi data awal
```

## Cara Menjalankan

### 1. Install Dependensi
Pastikan Anda sudah menginstal [Bun](https://bun.sh/).
```bash
bun install
```

### 2. Seed Data
Untuk mengisi database dengan data contoh:
```bash
bun src/seed.ts
```

### 3. Jalankan Aplikasi
```bash
bun src/index.ts
# atau cukup
bun run src/index.ts
```
