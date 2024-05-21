#include <Servo.h>

#include <esp_now.h>
#include <WiFi.h>

// REEMPLAZA CON LA Dirección MAC de tu receptor
uint8_t broadcastAddress[] = {0xC0, 0x49, 0xEF, 0xE5, 0xB1,0xB0};

// Definir variables para almacenar lecturas entrantes
int comando;

//Ejemplo de estructura para enviar datos
//Debe coincidir con la estructura del receptor
typedef struct struct_message {
    int control;
} struct_message;

struct_message comandos;

esp_now_peer_info_t peerInfo;

// Devolución de llamada cuando se reciben datos
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&comandos, incomingData, sizeof(comandos));
  comando = comandos.control;
}





// -------------- Posición de los servos para que la mano esté abierta ----------------------------//

//  180,180,180,0,0  ABIERTO
//  0,0,0,180,180  CERRADO


int posPulgar = 180;
int posIndice = 180;
int posMedio = 180;

int posAnular = 0;
int posMenique = 0;

// -------------- Variables de almacenamiento para la Media Móvil Exponencial, o valor suavizado de movimiento del servo --------- //

double pulgarSuavizado = 180;
double indiceSuavizado = 180;
double medioSuavizado = 180;
double anularSuavizado = 0;
double meniqueSuavizado = 0;

double valorAlfa = 0.2; // Mientras sea más pequeno este valor, el suavizado será mayor; o sea, se moverá más lento. 

// --------------- Definir los pines a los que va a ir cada servo con cada dedo ------------------ //

static const int servoPinP = 27; // Esta sentencia es para determintar el pin en el ESP32 que le mandará la señal PWM al servo
static const int servoPinI = 26;
static const int servoPinA = 25;
static const int servoPinMed = 33;
static const int servoPinMen = 32;

// --------------- Creación de los objetos servo para cada dedo ------------------- //

Servo servoPulgar;
Servo servoIndice;
Servo servoAnular;
Servo servoMedio;
Servo servoMenique;

// ------------------------ Comunicación Serial ------------------ //

String inputString = "";         
bool stringComplete = false;
const char separator = ','; 
const int dataLength = 1;
double datos[dataLength]; 



void setup() {

    initReceptor();
  
    // -------- Vincular el objeto servo de cada dedo con su respectivo pin ---------- //
    servoPulgar.attach(servoPinP);
    servoIndice.attach(servoPinI);
    servoAnular.attach(servoPinA);
    servoMedio.attach(servoPinMed);
    servoMenique.attach(servoPinMen);
    
  
}

void loop() {



  if (comando == 2){
    
     abierto();
    
    }


   else if (comando == 1){
    
    cerrado();
    
    }



    
    pulgarSuavizado = valorAlfa*posPulgar + (1-valorAlfa)*pulgarSuavizado;
    indiceSuavizado = valorAlfa*posIndice + (1-valorAlfa)*indiceSuavizado;
    medioSuavizado = valorAlfa*posMedio + (1-valorAlfa)*medioSuavizado;
    anularSuavizado = valorAlfa*posAnular + (1-valorAlfa)*anularSuavizado;
    meniqueSuavizado = valorAlfa*posMenique + (1-valorAlfa)*meniqueSuavizado;
  
  
    servoPulgar.write(int(pulgarSuavizado));
    servoIndice.write(int(indiceSuavizado));
    servoAnular.write(int(medioSuavizado));
    servoMedio.write(int(anularSuavizado));
    servoMenique.write(int(meniqueSuavizado));

    delay(10);


}



void abierto(){
  
     posPulgar = 180;
     posIndice = 180;
     posMedio = 180;
     posAnular = 0;
     posMenique = 0;
  
  }

void cerrado(){
  
     posPulgar = 0;
     posIndice = 0;
     posMedio = 0;
     posAnular = 180;
     posMenique = 180;
  
  }

//-------------- Funciones para la recepción de datos  ---------------------//



void initReceptor()
{
    // Establecer el dispositivo como estación Wi-Fi
  WiFi.mode(WIFI_STA);

  // Iniciar ESP-AHORA
  if (esp_now_init() != ESP_OK) {
    return;
  }
 // Registrar par
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Agregar par      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }
  
  // Regístrese para una función de devolución de llamada que se llamará cuando se reciban los datos
  esp_now_register_recv_cb(OnDataRecv);
}
