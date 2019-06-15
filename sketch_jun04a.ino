#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int gasPin = A0; 

#define         MQ_PIN                       (0)     
#define         RL_VALUE                     (5)     
#define         RO_CLEAN_AIR_FACTOR          (9.83)  
                                                     

#define         CALIBARAION_SAMPLE_TIMES     (50)   
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   
                                                    
#define         READ_SAMPLE_INTERVAL         (50)    
#define         READ_SAMPLE_TIMES            (5)    

#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)
 
float           LPGCurve[3]  =  {2.3,0.21,-0.47};  
float           COCurve[3]   =  {2.3,0.72,-0.34};   
float           SmokeCurve[3] = {2.3,0.53,-0.44};                                                     
float           Ro           =  10;  

struct package
  {
    float sicaklik = 0;
    float nem = 0;
    int h_kalite = 0;
    int lpg = 0;
    int co = 0;
    int smoke = 0;
  };
typedef struct package Package;
Package data;
void setup() {
  Serial.begin(9600);
  dht.begin();
  Ro = MQCalibration(MQ_PIN);
}

void loop() {
  // ISI - SICAKLIK SENSÖRÜ
  data.sicaklik = dht.readTemperature();
  data.nem = dht.readHumidity();
  data.h_kalite = analogRead(gasPin);
  // HAVA KALİTE SENSÖRÜ
  data.h_kalite = analogRead(gasPin);
  data.lpg = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_LPG);
  data.co = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_CO);
  data.smoke = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE);
  delay(5000);
  Serial.print(data.sicaklik);
  Serial.print(";");
  Serial.print(data.nem);
  Serial.print(";");
  Serial.print(data.h_kalite);
  Serial.print(";");
  Serial.print(data.lpg);
  Serial.print(";");
  Serial.print(data.co);
  Serial.print(";");
  Serial.print(data.smoke);
  Serial.println(";");
  delay(60000);
}
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   
  val = val/RO_CLEAN_AIR_FACTOR;                                                                          
  return val;
}
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}
 
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_LPG ) {
     return MQGetPercentage(rs_ro_ratio,LPGCurve);
  } else if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }    
 
  return 0;
}
 
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
