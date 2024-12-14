# Web Server HTTP Multi-Client dengan Bahasa C
## Deskripsi Program
Sebuah Web Server sederhana yang dibuat menggunakan Bahasa C. Server ini dapat menangani permintaan GET dan POST dari banyak klien berbasis web secara bersamaan dengan menggunakan teknik forking.
## Fitur
- Mendukung koneksi multi-client
- Melayani permintaan GET untuk file statis
- Mendukung permintaan POST untuk pengiriman data
## Struktur Proyek
![image](https://github.com/user-attachments/assets/b87f8bc0-fb41-4960-8cf9-c80fcf0c7e31)
### 1. `server.c`: File utama yang berisi implementasi server. Dalam file ini terdapat procedure dan function untuk melayani client berdasarkan method dan route yang diberikan.
Daftar procedure dan function:
- `main()`: Function utama untuk membuat socket bagi server, lalu mengikat socket ke address dan port tertentu, dan mendengarkan koneksi masuk dari client. Setiap kali ada permintaan masuk dari client, proses utama akan membuat proses anak baru untuk menangani permintaan tersebut.
- `getFileURL()`: Procedure untuk mendapatkan path ke file yang diminta oleh client berdasarkan route yang diberikan.
- `getMimeType()`: Procedure untuk mendapatkan mime type atau content type berdasarkan jenis file yang sudah didapatkan dari procedure `getFileURL()`. Procedure ini memungkinkan server untuk melayani permintaan berupa text, js, bahkan gambar dan GIF.
- `handleSignal()`: Procedure ini berfungsi untuk menghentikan server secara benar dengan menutup socket dan membebaskan memori sebelum menghentikan program. Hal ini bertujuan untuk mencegah terjadinya memory leak.
- `getTimeString()`: Procedure untuk mendapatkan waktu saat ini dalam format string.
- `handleClient()`: Procedure untuk melayani permintaan dari client berdasarkan method dan route yang diberikan.
### 2. `htdocs/`: Direktori yang menyimpan file statis untuk client.
### 3. `index.html`: File client berbasis web. Permintaan akan dikirim menggunakan `Fetch` API.
## Persyaratan Sistem
- Sistem Operasi Linux (misalnya, Ubuntu).
- Web Browser yang kompatibel dengan ES6 JavaScript.
- Port untuk menjalankan server. Pastikan port tidak digunakan oleh proses lain. Secara default, port yang digunakan adalah `2728`
- Code editor untuk menulis kode program (misalnya, NeoVim).
- GCC atau compiler lainnya.
## Panduan Instalasi
1. Clone repository
2. Jalankan perintah `gcc -o server server.c`
3. Jalankan server dengan `./server`
4. Masukkan URL `localhost:2728` ke dalam web browser
