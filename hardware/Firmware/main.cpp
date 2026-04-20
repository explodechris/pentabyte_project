#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Audio.h>
#include <driver/i2s.h>


// --- SCREEN CONFIG (VSPI Bus) --------------------------------
// Uses Default VSPI Pins: MOSI=23, CLK=18
#define S_WIDTH  128
#define S_HEIGHT 128
#define S_CS     5    // Chip Select
#define S_DC     21   // Data/Command
#define S_RST    22   // Reset

Adafruit_SSD1351 tft = Adafruit_SSD1351(S_WIDTH, S_HEIGHT, &SPI, S_CS, S_DC, S_RST);  // Screen (Standard SPI)


// --- SD CARD CONFIG (HSPI Bus) --------------------------------
// We are forcing the SD card onto the secondary SPI bus
#define SD_MISO  19  
#define SD_MOSI  13
#define SD_SCK   14
#define SD_CS    15

SPIClass sdSPI(HSPI); // Separate SPI instance for SD


// --- MIC CONFIG ------------------------------------------------
#define I2S_MIC_SCK  16
#define I2S_MIC_WS   17
#define I2S_MIC_SD   34
#define I2S_MIC_PORT I2S_NUM_1
#define BUTTON_PIN 4


// --- SPEAKER CONFIG (I2S) --------------------------------------
#define I2S_DOUT 25
#define I2S_BCLK 26
#define I2S_LRC  27

Audio audio;

// --- AUDIO CONFIG ----------------------------------------------
const int sampleRate = 16000;
File audioFile;

void setupI2SMic() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_MIC_SCK,
    .ws_io_num = I2S_MIC_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SD
  };

  i2s_driver_install(I2S_MIC_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_MIC_PORT, &pin_config);
}

void writeWavHeader(File &file) {
  byte header[44] = {
    'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ',
    16, 0, 0, 0, 1, 0, 1, 0, 
    (byte)(sampleRate & 0xFF), (byte)((sampleRate >> 8) & 0xFF), 0, 0,
    (byte)((sampleRate * 2) & 0xFF), (byte)(((sampleRate * 2) >> 8) & 0xFF), 0, 0,
    2, 0, 16, 0, 'd', 'a', 't', 'a', 0, 0, 0, 0
  };
  file.write(header, 44);
}
void finalizeWavHeader(File &file) {
  unsigned long dataSize = file.size() - 44;
  unsigned long riffSize = dataSize + 36;
  file.seek(4);
  file.write((byte)(riffSize & 0xFF)); file.write((byte)((riffSize >> 8) & 0xFF));
  file.write((byte)((riffSize >> 16) & 0xFF)); file.write((byte)((riffSize >> 24) & 0xFF));
  file.seek(40);
  file.write((byte)(dataSize & 0xFF)); file.write((byte)((dataSize >> 8) & 0xFF));
  file.write((byte)((dataSize >> 16) & 0xFF)); file.write((byte)((dataSize >> 24) & 0xFF));
  file.close();
}


// --- UI FUNCTIONS ----------------------------------------------
void default_screen(){
  tft.fillScreen(0x0000);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(1);
  tft.setCursor(20, 50);
  tft.println("HOLD BUTTON");
  tft.setCursor(20, 65);
  tft.println("TO SPEAK");
}
void recording_screen(){
  tft.fillScreen(0xF800);
  tft.setCursor(15, 60);
  tft.setTextColor(0xFFFF);
  tft.setTextSize(2);
  tft.println("RECORDING");
}
void saved_screen(){
  tft.fillScreen(0x07E0);
  tft.setCursor(20, 60);
  tft.setTextColor(0x0000);
  tft.setTextSize(1);
  tft.println("SAVED!");
}


void setup() {
  Serial.begin(115200);
  
  // --- Screen ------------------------------
  Serial.println("Starting screen...");
  tft.begin();
  tft.fillScreen(0x001F); // Blue (Start)
  delay(500);

  // --- SD Card -----------------------------
  Serial.println("Mounting SD Card...");
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Mount SD
  if (!SD.begin(SD_CS, sdSPI)) {
    // --- FAILURE ---
    Serial.println("SD Card Mount FAILED");
    tft.fillScreen(0xF800); // Red
    tft.setCursor(10, 50);
    tft.setTextSize(2);
    tft.println("SD FAIL!");
    while (1); // Stop here
  }
  
  // --- Speaker ------------------------------
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(10); // 0...21 (10 is comfortable)
  if(audio.connecttoFS(SD, "/test.mp3")) {
      tft.println("Playing MP3...");
      Serial.println("Playing /test.mp3");
  } else {
      tft.println("File Not Found!");
      Serial.println("File Not Found");
  }

  // --- Mic ----------------------------------
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(15);
  setupI2SMic();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // --- READY --------------------------------
  Serial.println("Setup complete!");

  default_screen();
}

bool isRecording = false;

void loop() {
  audio.loop();

  if (digitalRead(BUTTON_PIN) == LOW) {
    audio.stopSong();
    delay(50);
    
    SD.remove("/echo.wav");
    audioFile = SD.open("/echo.wav", FILE_WRITE);
    writeWavHeader(audioFile);
    
    recording_screen();
    
    size_t bytes_read;
    int32_t raw_samples[256]; 
    int16_t wav_samples[256]; 
    
    while (digitalRead(BUTTON_PIN) == LOW) {
      i2s_read(I2S_MIC_PORT, &raw_samples, sizeof(raw_samples), &bytes_read, portMAX_DELAY);
      Serial.printf("Bytes Read: %d | Raw Sample[0]: %d\n", bytes_read, raw_samples[0]);
      int samples_read = bytes_read / 4;
      
      for (int i = 0; i < samples_read; i++) {
        int32_t boosted = raw_samples[i] >> 14; 
        Serial.printf("Raw: %d | Boosted: %d\n", raw_samples[i], boosted);
        if (boosted > 32767) boosted = 32767;
        if (boosted < -32768) boosted = -32768;

        wav_samples[i] = (int16_t)boosted;
        Serial.println(wav_samples[i]);
      }
      
      audioFile.write((byte*)wav_samples, samples_read * 2);
    }
    
    finalizeWavHeader(audioFile);
    
    tft.fillScreen(0x07E0); tft.setCursor(10, 60); tft.println("PLAYBACK");
    audio.connecttoFS(SD, "/echo.wav");
  }
}