# 1 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
# 1 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.
// To get started please visit https://microsoft.github.io/azure-iot-developer-kit/docs/projects/devkit-translator/?utm_source=ArduinoExtension&utm_medium=ReleaseNote&utm_campaign=VSCode
# 5 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 6 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 7 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 8 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 9 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 10 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2

# 12 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 13 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 2
# 24 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
enum STATUS
{
  Idle,
  Recording,
  Recorded,
  WavReady,
  Uploaded,
  SelectLanguage
};

static int wavFileSize;
static char *waveFile = 
# 35 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 3 4
                       __null
# 35 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
                           ;
static char azureFunctionUri[128];

// The timeout for retrieving the result
static uint64_t result_timeout_ms;

// Audio instance
static AudioClass &Audio = AudioClass::getInstance();

// Language
static int currentLanguage = 1; // Default is Chinese
static const char *allLanguages[9] = {"Arabic", "Chinese", "French", "German", "Italian", "Japanese", "Portuguese", "Russian", "Spanish"};
static const char *allLocalNames[9] = {"ar-EG", "zh-CN", "fr-FR", "de-DE", "it-IT", "ja-JP", "pt-BR", "ru-RU", "es-ES"};

// Indicate the processing status
static STATUS status = Idle;

// Indicate whether WiFi is ready
static bool hasWifi = false;

// Indicate whether IoT Hub is ready
static bool hasIoTHub = false;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities
static void InitWiFi()
{
  Screen.print(2, "Connecting...");

  if (WiFi.begin() == WL_CONNECTED)
  {
    IPAddress ip = WiFi.localIP();
    Screen.print(1, ip.get_address());
    hasWifi = true;
    Screen.print(2, "Running... \r\n");
  }
  else
  {
    hasWifi = false;
    Screen.print(1, "No Wi-Fi\r\n ");
  }
}

static void EnterIdleState(bool clean = true)
{
  status = Idle;
  if (clean)
  {
    Screen.clean();
  }
  Screen.print(0, "Hold B to talk");
}

static int HttpTriggerTranslator(const char *content, int length)
{
  if (content == 
# 90 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 3 4
                __null 
# 90 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
                     || length <= 0 || length > (64 * 1024))
  {
    Serial.println("Content not valid");
    return -1;
  }

  sprintf(azureFunctionUri, "%s&&source=%s", (char *)"https://devicefunc.azurewebsites.net/api/devkit_translator?code=JyMArAJpGdLke8JlTkkKyocv9F5fgAmKyQnFEerbnbdGw9D91azaxA==", allLocalNames[currentLanguage]);
  HTTPClient client = HTTPClient(HTTP_POST, azureFunctionUri);
  client.set_header("source", allLocalNames[currentLanguage]);
  const Http_Response *response = client.send(content, length);

  if (response != 
# 101 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 3 4
                 __null 
# 101 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
                      && response->status_code == 200)
  {
    return 0;
  }
  else
  {
    return -1;
  }
}

static void ShowLanguages()
{
  char temp[20];

  int idx = (currentLanguage + 9 - 1) % 9;
  sprintf(temp, "  %s", allLanguages[idx]);
  Screen.print(1, temp);

  sprintf(temp, "> %s", allLanguages[(idx + 1) % 9]);
  Screen.print(2, temp);

  sprintf(temp, "  %s", allLanguages[(idx + 2) % 9]);
  Screen.print(3, temp);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Callback functions
static void ResultMessageCallback(const char *text, int length)
{
  Serial.printf("Enter message callback, text: %s\r\n", text);
  if (status != Uploaded)
  {
    return;
  }

  EnterIdleState();
  if (text == 
# 137 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 3 4
             __null
# 137 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
                 )
  {
    Screen.print(1, "Sorry, I can't \r\nhear you.");
    return;
  }

  char temp[33];
  int end = ((length)<(sizeof(temp) - 1)?(length):(sizeof(temp) - 1));
  memcpy(temp, text, end);
  temp[end] = '\0';
  Screen.print(1, "Translation: ");
  Screen.print(2, temp, true);
  LogTrace("DevKitTranslatorSucceed", "ver=2.0");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actions
static void DoIdle()
{
  if (digitalRead(USER_BUTTON_A) == 0x0)
  {
    // Enter Select Language mode
    status = SelectLanguage;
    Screen.clean();
    Screen.print(0, "Press B Scroll");
    ShowLanguages();
  }
  else if (digitalRead(USER_BUTTON_B) == 0x0)
  {
    // Enter the Recording mode
    Screen.clean();
    Screen.print(0, "Recording...");
    Screen.print(1, "Release to send\r\nMax duraion: \r\n1.5 sec");
    memset(waveFile, 0, ((32000 * 1.5) - 16000 + 44 + 1));
    Audio.format(8000, 16);
    Audio.startRecord(waveFile, ((32000 * 1.5) - 16000 + 44 + 1), 1.5);
    status = Recording;
  }

  DevKitMQTTClient_Check();
}

static void DoRecording()
{
  if (digitalRead(USER_BUTTON_B) == 0x1)
  {
    Audio.stop();
    status = Recorded;
  }
}

static void DoRecorded()
{
  Audio.getWav(&wavFileSize);
  if (wavFileSize > 0)
  {
    wavFileSize = Audio.convertToMono(waveFile, wavFileSize, 16);
    if (wavFileSize <= 0)
    {
      Serial.println("ConvertToMono failed! ");
      EnterIdleState();
    }
    else
    {
      status = WavReady;
      Screen.clean();
      Screen.print(0, "Processing...");
      Screen.print(1, "Uploading...");
    }
  }
  else
  {
    Serial.println("No Data Recorded! ");
    EnterIdleState();
  }
}

static void DoWavReady()
{
  if (wavFileSize <= 0)
  {
    EnterIdleState();
    return;
  }

  int ret = HttpTriggerTranslator(waveFile, wavFileSize);
  Serial.printf("Azure Function return value: %d\r\n", ret);
  if (ret == 0)
  {
    status = Uploaded;
    Screen.print(1, "Receiving...");
    // Start retrieving result timeout clock
    result_timeout_ms = SystemTickCounterRead();
  }
  else
  {
    Serial.println("Error happened when translating: Azure Function failed");
    EnterIdleState();
    Screen.print(1, "Sorry, I can't \r\nhear you.");
  }
}

static void DoUploader()
{
  DevKitMQTTClient_ReceiveEvent();

  if ((int)(SystemTickCounterRead() - result_timeout_ms) >= 30000)
  {
    // Timeout
    EnterIdleState();
    Screen.print(1, "Client Timeout");
  }
}

static void DoSelectLanguage()
{
  if (digitalRead(USER_BUTTON_B) == 0x0)
  {
    currentLanguage = (currentLanguage + 1) % 9;
    ShowLanguages();
  }
  else if (digitalRead(USER_BUTTON_A) == 0x0)
  {
    EnterIdleState();
  }
  DevKitMQTTClient_Check();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino sketch
void setup()
{
  Screen.init();
  Screen.print(0, "DevKitTranslator");

  if (strlen("https://devicefunc.azurewebsites.net/api/devkit_translator?code=JyMArAJpGdLke8JlTkkKyocv9F5fgAmKyQnFEerbnbdGw9D91azaxA==") == 0)
  {
    Screen.print(2, "No Azure Func");
    return;
  }

  Screen.print(2, "Initializing...");
  pinMode(USER_BUTTON_A, 0x1);
  pinMode(USER_BUTTON_B, 0x1);

  Screen.print(3, " > Serial");
  Serial.begin(115200);

  // Initialize the WiFi module
  Screen.print(3, " > WiFi");
  hasWifi = false;
  InitWiFi();
  if (!hasWifi)
  {
    return;
  }
  LogTrace("DevKitTranslatorSetup", "ver=2.0");

  // IoT hub
  Screen.print(3, " > IoT Hub");
  if (!DevKitMQTTClient_Init())
  {
    Screen.clean();
    Screen.print(0, "DevKitTranslator");
    Screen.print(2, "No IoT Hub");
    hasIoTHub = false;
    return;
  }
  hasIoTHub = true;
  DevKitMQTTClient_SetMessageCallback(ResultMessageCallback);

  // Audio
  Screen.print(3, " > Audio");
  waveFile = (char *)malloc(((32000 * 1.5) - 16000 + 44 + 1));
  if (waveFile == 
# 311 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino" 3 4
                 __null
# 311 "c:\\Users\\SibeeshVenu\\SourceCode\\IoT Dashboard\\ml.IoTDashboard.Device\\Device\\DevKitTranslator.ino"
                     )
  {
    Screen.print(3, "Audio init fails");
    Serial.println("No enough Memory!");
    return;
  }

  Screen.print(1, "Talk:   Hold  B\r\n \r\nSetting:Press A");
}

void loop()
{
  if (hasWifi && hasIoTHub)
  {
    switch (status)
    {
    case Idle:
      DoIdle();
      break;

    case Recording:
      DoRecording();
      break;

    case Recorded:
      DoRecorded();
      break;

    case WavReady:
      DoWavReady();
      break;

    case Uploaded:
      DoUploader();
      break;

    case SelectLanguage:
      DoSelectLanguage();
      break;
    }
  }
  delay(100);
}
