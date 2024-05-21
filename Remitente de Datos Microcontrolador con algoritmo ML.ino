#include <esp_now.h>
#include <WiFi.h>


// REEMPLAZA CON LA Dirección MAC de tu receptor
uint8_t broadcastAddress[] = {0xB0, 0xA7, 0x32, 0x2A, 0x85, 0x84};

// Variable para almacenar si el envío de datos fue exitoso
String success;

//Ejemplo de estructura para enviar datos
//Debe coincidir con la estructura del receptor
typedef struct struct_message {
    int control;
} struct_message;

struct_message comandos;

esp_now_peer_info_t peerInfo;

// Devolución de llamada cuando se envían datos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  status == ESP_NOW_SEND_SUCCESS ? "Entrega exitosa" : "Error de entrega";
  if (status ==0){
    success = "Entrega exitosa :)";
  }
  else{
    success = "Error de entrega :(";
  }
}


// --- Código para el sensor -------- //

const int LoPlus = 16;
const int LoMin = 17;


const int timeData = 5; // Este es el tiempo que se activará la función de extracción de características
double sensorMuscular = 0;

// ---- Extracción de características ---- //


double wlSensorMuscular = 0;
double wlaSensorMuscular = 0;


///////////////////////////////// Variables Red Neuronal /////////////////////////////////
double a0[1];
double W1[6][1] = {{-1.109},{0.727},{-1.192},{-0.805},{0.276},{0.864}};
double a1[6];
double W2[2][6] = {{0.999,-1.0,0.808,1.31,0.38,-0.519},{-0.593,1.138,-0.533,-1.247,0.043,1.03}};
double a2[2]; 
double b1[6]= {1.059,0.736,0.753,0.503,-0.25,0.875};
double b2[2]= {-1.292,1.292};
double aux = 0.0;
//////////////////////////////////////////////////////////////////



///////////////////////////////// Preprocesamiento Red Neuronal /////////////////////////////////
double mean[1]={2646.833};
double dstd[1]={2333.564};
///////////////////////////////////////////////////////////////////////////////////////////////////////




void setup() {

  Serial.begin(115200);
  
  pinMode(LoPlus, INPUT); // Configuración para la detección LO +
  pinMode(LoMin, INPUT); // Configuración para la detección LO - 

  initEmisor();

  delay(100);
}

void loop() {

  
    if((digitalRead(LoPlus) == 1)||(digitalRead(LoMin) == 1)){
    Serial.println('!');
    }
    else{
      ExtraccionCaracteristicas();
      
      a0[0] = dataNormalized(wlSensorMuscular,mean[0],dstd[0]);
      
    ///////////////////////////////// Estructura Red Neuronal /////////////////////////////////
    for(int i = 0 ; i<6; i++ ) {aux=0.0;for(int j = 0 ; j <1 ; j++ ) { aux=aux+W1[i][j]*a0[j];} a1[i]=relu(aux+b1[i]);}
    for(int i = 0 ; i<2; i++ ) {aux=0.0;for(int j = 0 ; j <6 ; j++ ) { aux=aux+W2[i][j]*a1[j];} a2[i]=sigmoid(aux+b2[i]);}
    //////////////////////////////////////////////////////////////////////////////////////////

    comandos.control = round(a2[0])+1; 
  
    // Enviar mensaje a través de ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &comandos, sizeof(comandos));
   

    Serial.print("Contraccción: ");
    Serial.println(round(a2[0]));
    }
    //Espere un poco para evitar que los datos en serie se saturen


    delay(5);
}


void initEmisor(){
   
   // Establecer el dispositivo como estación Wi-Fi
  WiFi.mode(WIFI_STA);

  // Iniciar ESP-AHORA
  if (esp_now_init() != ESP_OK) {
    return;
  }

  // Una vez que ESPNow se inicie con éxito, nos registraremos en 
  // Enviar CB para obtener el estado del paquete transmitido
  esp_now_register_send_cb(OnDataSent);
  
  // Registrar par
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Agregar par      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    return;
  }
}


void ExtraccionCaracteristicas(){
  

  wlSensorMuscular = 0.0;
  
  int sizeSample = timeData*10;
  
  for(int k =0; k < sizeSample; k++){

    sensorMuscular = analogRead(A0);
  
  delay(5);


    wlSensorMuscular = wlSensorMuscular + abs(sensorMuscular - wlaSensorMuscular);

    wlaSensorMuscular = sensorMuscular;

    
    delay(5);
    } 
    
  }

double relu(double n)
{
  if(n>=0) return n; else if (n<0) return 0;
}


double sigmoid(double n){
  
  return 1.0/(1.0 + exp(-n));
  
  }
  
double dataNormalized(double inputData,double mean,double desvStandar) 
{
  double valueNorm;
  valueNorm = (inputData-mean)/desvStandar;
  return valueNorm;
}
