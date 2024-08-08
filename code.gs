function doGet(e) { 
  Logger.log(JSON.stringify(e));      // Mencatat parameter permintaan yang diterima dalam log untuk debugging.
  var result = 'Ok';                  // Inisialisasi variabel result dengan nilai 'Ok'.

  // Memeriksa URL apakah terdapat Parameter
  if (!e.parameter) {                 
    result = 'No Parameters';         // Jika tidak ada parameter, atur result menjadi 'No Parameters'.


  // Jika ada parameter yang diterima.
  }else {                                              
    // ID dan Nama Sheet pada SpreadSheets 
    var sheet_id = '...';       // ID Spreadsheet Google Sheets.
    var sheet_name = "...";     // Nama Sheet dalam Google Sheets.

    // Buka SpreadSheets berdasarkan ID 
    var sheet_open = SpreadsheetApp.openById(sheet_id);
    // Mendapatkan Sheets berdasarkan nama di atas
    var sheet_target = sheet_open.getSheetByName(sheet_name);

    // Membuat baris baru seletah baris paling akhir untuk data baru 
    var newRow = sheet_target.getLastRow() + 1; // Menentukan baris baru untuk memasukkan data.
    

    // Array untuk menyimpan data pada tabel kiri
    var rowDataLog = [];                        

    // variable untuk menyimpan data terbaru
    var Data_for_I3; // Variabel untuk menyimpan data tanggal terbaru.
    var Data_for_J3; // Variabel untuk menyimpan data waktu terbaru.
    var Data_for_K3; // Variabel untuk menyimpan status pembacaan sensor terbaru.
    var Data_for_L3; // Variabel untuk menyimpan suhu terbaru.
    var Data_for_M3; // Variabel untuk menyimpan kelembaban terbaru.
    var Data_for_N3; // Variabel untuk menyimpan status Switch 1 terbaru.
    var Data_for_O3; // Variabel untuk menyimpan status Switch 2 terbaru.

    // Tanggal saat ini
    var Curr_Date = Utilities.formatDate(new Date(), "Asia/Jakarta", 'dd/MM/yyyy'); // Mendapatkan tanggal saat ini dalam format dd/MM/yyyy.
    rowDataLog[0] = Curr_Date;    // Menyimpan tanggal dalam kolom A (untuk log data).
    Data_for_I3   = Curr_Date;    // Menyimpan tanggal dalam kolom I3 (untuk data terbaru).

    // Waktu saat ini
    var Curr_Time = Utilities.formatDate(new Date(), "Asia/Jakarta", 'HH:mm:ss'); // Mendapatkan waktu saat ini dalam format HH:mm:ss.
    rowDataLog[1] = Curr_Time;  // Menyimpan waktu dalam kolom B (untuk log data).
    Data_for_J3 = Curr_Time;    // Menyimpan waktu dalam kolom J3 (untuk data terbaru).

    // Variable untuk menyimpan status
    var sts_val = ''; // Variabel untuk menyimpan status.

    // Looping untuk mengambil data dari parameter URL
    for (var param in e.parameter) {                  // Looping melalui semua parameter yang diterima.
      Logger.log('In for loop, param=' + param);      // Mencatat nama parameter dalam log.
      var value = stripQuotes(e.parameter[param]);    // Menghapus tanda kutip dari nilai parameter.
      Logger.log(param + ':' + e.parameter[param]);   // Mencatat nama dan nilai parameter dalam log.
      
      switch (param) { // Memeriksa nama parameter.
        case 'sts':
          sts_val = value; // Menyimpan nilai status.
          break;

        case 'srs':
          rowDataLog[2] = value;      // Menyimpan status pembacaan sensor dalam kolom C.
          Data_for_K3 = value;        // Menyimpan status pembacaan sensor dalam kolom K3.
          break;

        case 'temp':
          rowDataLog[3] = value;  // Menyimpan suhu dalam kolom D.
          Data_for_L3 = value;    // Menyimpan suhu dalam kolom L3.
          break;

        default:
          result += ", parameter tidak valid!"; // Menambahkan pesan hasil jika parameter tidak didukung.
      }
    }
    
    // Kondisi untuk menulis data yang diterima dari ESP32 ke Google Sheets berdasarkan variable "sts_val"
    if (sts_val == 'write') {
      // Data disimpan pada tabel log data
      Logger.log(JSON.stringify(rowDataLog));                                       // Mencatat data log dalam format JSON.
      var newRangeDataLog = sheet_target.getRange(newRow, 1, 1, rowDataLog.length); // Mendapatkan range untuk baris baru.
      newRangeDataLog.setValues([rowDataLog]);                                      // Menulis data log ke Google Sheets.
      
      // Data disimpan ke tabel data terbaru
      var RangeDataLatest = sheet_target.getRange('I3:O3');                         // Mendapatkan range untuk kolom I3 hingga O3.
      RangeDataLatest.setValues([[Data_for_I3, Data_for_J3, Data_for_K3, Data_for_L3, Data_for_M3, Data_for_N3, Data_for_O3]]); 
    
      // Mengirim pesan ke ESP32 bahwa pesan sudah diterima
      result += ", Berhasil dikirim";
      return ContentService.createTextOutput(result); 
    }
    
    // Kondisi untuk mengirim data ke ESP32 ketika ESP32 membaca data dari Google Sheets berdasarkan variable "sts_val"
    if (sts_val == 'read') {
      var all_Data = sheet_target.getRange('K3:O3').getValues();  // Mendapatkan data dari kolom K3 hingga O3.
      return ContentService.createTextOutput(all_Data);           // Mengembalikan data ke ESP32.
    }
  }
}

function stripQuotes(value) {
  return value.replace(/^["']|['"]$/g, ""); // Menghapus tanda kutip dari nilai.
}

