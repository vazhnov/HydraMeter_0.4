#include "ST7565.h"
#include <nrf24.h>
#include <SPI.h>

// the LCD backlight is connected up to a pin so you can turn it on & off
#define BACKLIGHT_LED 22

// pin 9 - Serial data out (SID)
// pin 8 - Serial clock out (SCLK)
// pin 7 - Data/Command select (RS or A0)
// pin 6 - LCD reset (RST)
// pin 5 - LCD chip select (CS)
//15->I
//14->II
//13->L
//12->R
#define PIN_I 9 //9 - 15
#define PIN_II 8 //8 - 14
#define PIN_L 7 //7 - 13
#define PIN_R 6 //6 - 12

#define SPI1_SCK 10
#define SPI1_MOSI 11
#define SPI1_MISO 12
#define RADIO_CS 14
#define RADIO_DO0 15

ST7565 glcd(19, 18, 16, 20, 17);
nrf24 nrfRadio(RADIO_DO0,RADIO_CS);//ce,cs

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// a bitmap of a 16x16 quincunx
const static unsigned char __attribute__ ((progmem)) logo16_glcd_bmp[]={
0x1C, 0x22, 0x41, 0x41, 0xC1, 0x62, 0x3C, 0x10, 0x10, 0x3C, 0x62, 0xC1, 0x41, 0x41, 0x22, 0x1C, 
0x38, 0x44, 0x82, 0x82, 0x83, 0x46, 0x3C, 0x08, 0x08, 0x3C, 0x46, 0x83, 0x82, 0x82, 0x44, 0x38,};

#define STATE_STRING_LENGTH 16 //max length of the state string
char stateString[STATE_STRING_LENGTH+1];//+1 for guaranteed null term
int stateStreamIndex = 0;

#define MAX_DATA_STREAMS 16
#define MAX_DATA_STREAM_ID_LENGTH 4
#define MAX_DATA_STREAM_LABEL_LENGTH 20
#define MAX_DATA_STREAM_UNIT_LENGTH 3
#define MAX_DATA_STREAM_STR_LENGTH 16

int64_t dataStreamLastUpdate[MAX_DATA_STREAMS];
char dataStreamIDs[MAX_DATA_STREAMS][MAX_DATA_STREAM_ID_LENGTH];//TODO add 1 length and force null term
char dataStreamLabels[MAX_DATA_STREAMS][MAX_DATA_STREAM_LABEL_LENGTH];
char dataStreamUnits[MAX_DATA_STREAMS][MAX_DATA_STREAM_UNIT_LENGTH];
double dataStreamValues[MAX_DATA_STREAMS];
char dataStreamStringVal[MAX_DATA_STREAMS][MAX_DATA_STREAM_STR_LENGTH];
bool dataStreamIsInfo[MAX_DATA_STREAMS];
bool dataStreamIsStringVal[MAX_DATA_STREAMS];
bool doDispDataStream[MAX_DATA_STREAMS];

typedef union {
	uint8_t byteAr[4];
	float flt;
} floatToBytes_t;

int matchDataStreamID(char* in){
  for(int i=0;i<MAX_DATA_STREAMS;i++){
    //Serial.print("Does <");Serial.print(dataStreamIDs[i]);
    //Serial.print("> match <");Serial.println(in);
    for(int j=0;j<MAX_DATA_STREAM_ID_LENGTH;j++){
      if(dataStreamIDs[i][j] == in[j]){
        //Serial.print(".");
        if(in[j] == '\0'){
          //Serial.print("Returning ");Serial.println(i);
          //if(j==0){
          //  return -1;
          //}
          return i;
        }
      }else{
        j=1000;
        break;//break out of inner loop (j) - labels don't match
      }
    }
  }
  return -1;
}

void easyEngFltPrint(char* strng,double val){
  double absv = abs(val);
  if(val >= 0){
    strng[0] = ' ';
    strng++;
  }
  if(absv < 0.99995e-5){
    sprintf(strng,"%1.4fu",val*1e6);
  }else if(absv < 0.99995e-4){
    sprintf(strng,"%1.3fu",val*1e6);
  }else if(absv < 0.99995e-3){
    sprintf(strng,"%1.2fu",val*1e6);
  }else if(absv < 0.99995e-2){
    sprintf(strng,"%1.4fm",val*1e3);
  }else if(absv < 0.99995e-1){
    sprintf(strng,"%1.3fm",val*1e3);
  }else if(absv < 0.5){
    sprintf(strng,"%3.2fm",val*1e3);
  }else if(absv < 0.99995e1){
    sprintf(strng,"%1.4f",val);
  }else if(absv < 0.99995e2){
    sprintf(strng,"%1.3f",val);
  }else if(absv < 0.99995e3){
    sprintf(strng,"%1.2f",val);
  }else if(absv < 0.99995e4){
    sprintf(strng,"%1.4fK",val/1e3);
  }else if(absv < 0.99995e5){
    sprintf(strng,"%1.3fK",val/1e3);
  }else if(absv < 0.99995e6){
    sprintf(strng,"%1.2fK",val/1e3);
  }else if(absv < 0.99995e7){
    sprintf(strng,"%1.4fM",val/1e6);
  }else if(absv < 0.99995e8){
    sprintf(strng,"%1.3fM",val/1e6);
  }else if(absv < 0.99995e9){
    sprintf(strng,"%1.2fM",val/1e6);
  }else if(absv < 0.99995e10){
    sprintf(strng,"%1.4fG",val/1e9);
  }else if(absv < 0.99995e11){
    sprintf(strng,"%1.3fG",val/1e9);
  }else if(absv < 0.99995e10){
    sprintf(strng,"%1.2fG",val/1e9);
  }
}

//engineering float print with separate prefix (pstrng = 'M' or 'K' or whatever)
//also puts leading space in >0
void easyEngFltPrintSepPx(char* dstrng,char* pstrng,double val){
  double absv = abs(val);
  if(val >= 0){
    dstrng[0] = ' ';
    dstrng++;
  }
  if(absv < 0.99995e-5){
    sprintf(dstrng,"%1.4f",val*1e6);
    pstrng[0] = 'u';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e-4){
    sprintf(dstrng,"%1.3f",val*1e6);
    pstrng[0] = 'u';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e-3){
    sprintf(dstrng,"%1.2f",val*1e6);
    pstrng[0] = 'u';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e-2){
    sprintf(dstrng,"%1.4f",val*1e3);
    pstrng[0] = 'm';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e-1){
    sprintf(dstrng,"%1.3f",val*1e3);
    pstrng[0] = 'm';
    pstrng[1] = '\0';
  }else if(absv < 0.5){
    sprintf(dstrng,"%1.2f",val*1e3);
    pstrng[0] = 'm';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e1){
    sprintf(dstrng,"%1.4f",val);
    pstrng[0] = '\0';
  }else if(absv < 0.99995e2){
    sprintf(dstrng,"%1.3f",val);
    pstrng[0] = '\0';
  }else if(absv < 0.99995e3){
    sprintf(dstrng,"%1.2f",val);
    pstrng[0] = '\0';
  }else if(absv < 0.99995e4){
    sprintf(dstrng,"%1.4f",val/1e3);
    pstrng[0] = 'K';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e5){
    sprintf(dstrng,"%1.3f",val/1e3);
    pstrng[0] = 'K';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e6){
    sprintf(dstrng,"%1.2f",val/1e3);
    pstrng[0] = 'K';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e7){
    sprintf(dstrng,"%1.4f",val/1e6);
    pstrng[0] = 'M';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e8){
    sprintf(dstrng,"%1.3f",val/1e6);
    pstrng[0] = 'M';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e9){
    sprintf(dstrng,"%1.2f",val/1e6);
    pstrng[0] = 'M';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e10){
    sprintf(dstrng,"%1.4f",val/1e9);
    pstrng[0] = 'G';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e11){
    sprintf(dstrng,"%1.3f",val/1e9);
    pstrng[0] = 'G';
    pstrng[1] = '\0';
  }else if(absv < 0.99995e12){
    sprintf(dstrng,"%1.2f",val/1e9);
    pstrng[0] = 'G';
    pstrng[1] = '\0';
  }
}

void parseRadio(uint8_t* buf){
  char streamID[3];
  int dataStreamIndex = -1;
  boolean dataStreamIsString = false;
  boolean failed = false;
  streamID[2] = '\0';
  streamID[0] = buf[0];
  streamID[1] = buf[1];
  Serial.print("RadMatch:");
  Serial.println(streamID);
  int index = matchDataStreamID(streamID);
  if(index>=0 && index <MAX_DATA_STREAMS){
    dataStreamIndex = index;
    if(dataStreamIsStringVal[index]){
      dataStreamIsString = true;
    }else{
      dataStreamIsString = false;
    }
  }else{
    failed = true;
  }
  if(failed){
    Serial.println("No match");
    return;
  }
  if(dataStreamIndex == stateStreamIndex){
    int strind = (buf[2]-'0')*3; //TODO need to tell when we're done, so we can end string
    if(strind + 4 >= STATE_STRING_LENGTH){
      return;//out of bounds, do nothing
    }
    stateString[strind] = buf[3];
    stateString[strind+1] = buf[4];
    stateString[strind+2] = buf[5];
    stateString[strind+3] = '\0';
  }else if(dataStreamIsString){
    //char streamStr[5];
    //streamStr[4] = '\0';
    //for(int i=0;i<4;i++){
    //  streamStr[i] = buf[i+2];
    //}
    //strncpy(&dataStreamStringVal[dataStreamIndex][0],streamStr,MAX_DATA_STREAM_STR_LENGTH);
    dataStreamStringVal[dataStreamIndex][0] = buf[2];
    dataStreamStringVal[dataStreamIndex][1] = buf[3];
    dataStreamStringVal[dataStreamIndex][2] = buf[4];
    dataStreamStringVal[dataStreamIndex][3] = buf[5];
    Serial.printf("Wrote str:%s\r\n",&dataStreamStringVal[dataStreamIndex][0]);
  }else{
    floatToBytes_t fltConv;
    fltConv.byteAr[0] = buf[2];
    fltConv.byteAr[1] = buf[3];
    fltConv.byteAr[2] = buf[4];
    fltConv.byteAr[3] = buf[5];
    dataStreamValues[dataStreamIndex] = (double)fltConv.flt;
    Serial.printf("Wrote val:%f\r\n",fltConv.flt);
  }
  dataStreamLastUpdate[dataStreamIndex] = millis();
}

void parseSerial(char in){ //TODO cleanup reuse of dataStreamIsString checks/calls
  static int64_t upper = 0;
  static double lower = 0;
  static int digitNum = 0;
  static boolean isBeforeDec = false;
  static boolean isBeforeNumber = false;
  static boolean isDone = false;
  static char streamID[MAX_DATA_STREAM_ID_LENGTH];
  static char streamStr[MAX_DATA_STREAM_STR_LENGTH+1];
  static int idI = 0;
  static boolean failed = false;
  static int dataStreamIndex = 0;
  static boolean dataStreamIndexSet = false;
  static boolean dataStreamIsString = false;
  static boolean isNegative = false;
  //Serial.print("<<");Serial.print(in);Serial.println(">>");
  if(in >= '0' && in <= '9'){
    if(!failed){
      if(isBeforeNumber){
        streamID[idI] = in;
        idI++;
        if(idI > MAX_DATA_STREAM_ID_LENGTH){
          idI = 0;
          failed = true;
        }
      }else if(dataStreamIsString){
        streamStr[digitNum] = in;
        digitNum++;
        if(digitNum >= MAX_DATA_STREAM_STR_LENGTH){
          failed = true;
          digitNum = 0;
        }
      }else{
        if(isBeforeDec){
          upper *= 10;
          upper += in-'0';
        }else{
          digitNum --;
          lower += (in-'0') * pow(10,digitNum);
        }
      }
    }
  }else if((in >= 'A' && in<'Z') || (in >= 'a' && in <= 'z')){
    if(!failed){
      if(isBeforeNumber){
          //Serial.print('>');Serial.print(in);
          streamID[idI] = in;
          idI++;
          if(idI > MAX_DATA_STREAM_ID_LENGTH){
            idI = 0;
            failed = true;
          }
      }else{
        if(dataStreamIsString){
          streamStr[digitNum] = in;
          digitNum++;
          if(digitNum >= MAX_DATA_STREAM_STR_LENGTH){
            failed = true;
            digitNum = 0;
          }
        }
      }
    }
  }else{
    switch(in){
    case '\r':
    case '\n':{
      if(!failed && dataStreamIndex >= 0){
        if(dataStreamIsString){
          streamStr[digitNum] = '\0';
          digitNum++;
          strncpy(&dataStreamStringVal[dataStreamIndex][0],streamStr,MAX_DATA_STREAM_STR_LENGTH);
        }else{
          dataStreamValues[dataStreamIndex] = ((double)upper + lower) * (isNegative?-1:1);
        }
        dataStreamLastUpdate[dataStreamIndex] = millis();
        Serial.print("Got value ");
        Serial.print(streamID);
        Serial.print(" : ");
        if(dataStreamIsString){
          Serial.println(dataStreamStringVal[dataStreamIndex]);
        }else{
          Serial.println(dataStreamValues[dataStreamIndex]);
        }
      }else{
        Serial.println("Failed, done");
      }
      idI = 0;
      isBeforeDec = true;
      isBeforeNumber=true;
      isDone = false;
      digitNum = 0;
      upper = 0;
      lower = 0;
      failed = false;
      idI = 0;
      dataStreamIndex = -1;
      dataStreamIndexSet = false;
      dataStreamIsString = false;
      isNegative = false;
      streamStr[MAX_DATA_STREAM_STR_LENGTH] = '\0';
    }
    break;
    case '.':
    if(dataStreamIsString){
      streamStr[digitNum] = in;
      digitNum++;
      if(digitNum >= MAX_DATA_STREAM_STR_LENGTH){
        failed = true;
        digitNum = 0;
      }
    }else{
      isBeforeDec = false;
    }
    //Serial.println("Decimal");
    break;
    case ',':
      if(dataStreamIsString){
        streamStr[digitNum] = in;
        digitNum++;
        if(digitNum >= MAX_DATA_STREAM_STR_LENGTH){
          failed = true;
          digitNum = 0;
        }
      }
    break;
    case '=':{
      isBeforeNumber = false;
      streamID[idI] = '\0';
      Serial.print("Matching<");
      Serial.print(idI);Serial.print('>');
      Serial.println(streamID);
      int index = matchDataStreamID(streamID);
      if(index>=0 && index <MAX_DATA_STREAMS){
        //Serial.print("Found data stream:");
        dataStreamIndex =index;
        if(dataStreamIsStringVal[index]){
          dataStreamIsString = true;
        }else{
          dataStreamIsString = false;
        }
        //Serial.println(index);
        dataStreamIndexSet = true;
      }else{
        //Serial.println("didn't find data stream :(");
        failed = true;
      }
    }
    break;
    case '\0':
    break;
    case '-':
      if(dataStreamIsString){
        streamStr[digitNum] = in;
        digitNum++;
        if(digitNum >= MAX_DATA_STREAM_STR_LENGTH){
          failed = true;
          digitNum = 0;
        }
      }else{
        isNegative = true;
      }
    break;
    //these are all explicitly allowed in string values, chars aren't by default
    //notable EXCEPTIONS, are '=' (used to separate ID from value), and control chars (\t, \r, \n,). '_' is reserved for unused
    case ':':
    case '{':
    case '}':
    case '<':
    case '>':
    case '\'':
    case '\"':
    case ')':
    case '(':
    case '#':
    case '@':
    case '!':
    case '$':
    case '%':
    case '^':
    case '&':
    case '*':
    case '[':
    case ']':
    case ';':
    case '?':
    case '+':
    case ' ':
      if(dataStreamIsString){
        streamStr[digitNum] = in;
        digitNum++;
        if(digitNum >= MAX_DATA_STREAM_STR_LENGTH){
          failed = true;
          digitNum = 0;
        }
      }
    break;
    default:
    break;
    }
  }
}

void setup()   {            
  delay(50);    
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(PIN_I,INPUT_PULLUP);
  pinMode(PIN_II,INPUT_PULLUP);
  pinMode(PIN_L,INPUT_PULLUP);
  pinMode(PIN_R,INPUT_PULLUP);

  // turn on backlight
  pinMode(BACKLIGHT_LED, OUTPUT);
  digitalWrite(BACKLIGHT_LED, HIGH);
  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  pinMode(PIN_L,INPUT_PULLUP);
  pinMode(PIN_R,INPUT_PULLUP);

  SPI1.setSCK(SPI1_SCK);
	SPI1.setRX(SPI1_MISO);
	SPI1.setTX(SPI1_MOSI);
	SPI1.begin(false);
  delay(10);
  nrfRadio.begin(RF24_PRX,6);
	delay(20);
  // initialize and set the contrast to 0x18
  glcd.begin(0x1E);

  glcd.clear();
  glcd.drawbitmap(56, 0, logo16_glcd_bmp, 16, 16, BLACK);
  glcd.display();
  glcd.drawBigString_24(0,2,"HYDRA",LCDWIDTH);
  glcd.drawBigString_16(0,5,"Meter",LCDWIDTH);
  glcd.display();       // show the lines
  delay(2000);

  for(int i=0;i<MAX_DATA_STREAMS;i++){
    strcpy(dataStreamIDs[i],"_");
    strcpy(dataStreamLabels[i],"None");
    strcpy(dataStreamUnits[i],"_");
    dataStreamIsInfo[i] = false;
    dataStreamStringVal[i][0]='\0';
    dataStreamValues[i]=0;
    doDispDataStream[i] = true;
  }

  int ind = 0;
  strcpy(dataStreamIDs[ind],"V");
  strcpy(dataStreamLabels[ind],"DC Volt");
  strcpy(dataStreamUnits[ind],"V");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"I");
  strcpy(dataStreamLabels[ind],"Current");
  strcpy(dataStreamUnits[ind],"A");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"R");
  strcpy(dataStreamLabels[ind],"Resist");
  strcpy(dataStreamUnits[ind],"R");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"IT");
  strcpy(dataStreamLabels[ind],"Test I");
  strcpy(dataStreamUnits[ind],"A");
  dataStreamIsInfo[ind] = true;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"P");
  strcpy(dataStreamLabels[ind],"Power");
  strcpy(dataStreamUnits[ind],"W");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"RR");
  strcpy(dataStreamLabels[ind],"RRange");
  strcpy(dataStreamUnits[ind],"");
  dataStreamIsInfo[ind] = true;
  dataStreamIsStringVal[ind] = true;
  ind++;

  strcpy(dataStreamIDs[ind],"VR");
  strcpy(dataStreamLabels[ind],"VRange");
  strcpy(dataStreamUnits[ind],"");
  dataStreamIsInfo[ind] = true;
  dataStreamIsStringVal[ind] = true;
  ind++;

  strcpy(dataStreamIDs[ind],"VA");
  strcpy(dataStreamLabels[ind],"AC Volt");
  strcpy(dataStreamUnits[ind],"V");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"IR");
  strcpy(dataStreamLabels[ind],"IRange");
  strcpy(dataStreamUnits[ind],"");
  dataStreamIsInfo[ind] = true;
  dataStreamIsStringVal[ind] = true;
  ind++;
  
  strcpy(dataStreamIDs[ind],"M0");
  strcpy(dataStreamLabels[ind],"XT60 V");
  strcpy(dataStreamUnits[ind],"V");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"M1");
  strcpy(dataStreamLabels[ind],"XT60 I");
  strcpy(dataStreamUnits[ind],"A");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"MP");
  strcpy(dataStreamLabels[ind],"XT60 Pwr");
  strcpy(dataStreamUnits[ind],"W");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"MR");
  strcpy(dataStreamLabels[ind],"XT60 Res");
  strcpy(dataStreamUnits[ind],"R");
  dataStreamIsInfo[ind] = false;
  dataStreamIsStringVal[ind] = false;
  ind++;

  strcpy(dataStreamIDs[ind],"ST");//Main state string
  strcpy(dataStreamLabels[ind],"");
  strcpy(dataStreamUnits[ind],"");
  dataStreamIsInfo[ind] = true;
  dataStreamIsStringVal[ind] = true;
  doDispDataStream[ind] = true;//false;
  stateStreamIndex = ind;
  ind++;

  for(int i=0;i<MAX_DATA_STREAMS;i++){
    dataStreamLastUpdate[i] = 0;
  }
}


void loop(){
  static uint32_t lastHandShake=0;
  if(millis()-lastHandShake > 250){
    lastHandShake = millis();
    Serial1.println('H');
  }
  while(Serial1.available()){
    char ch = Serial1.read();
    parseSerial(ch);
  }
  if(nrfRadio.rxAvailable()) {
    uint8_t buf[8];
    nrfRadio.readPayload(buf,6);
    parseRadio(buf);
  }
  static uint32_t last = 0;
  if(millis()-last > 300){
    last = millis();
    Serial.println("postser");
    int numActiveStreams = 0;
    static int prevStreamNo = 0;
    int activeStreams[MAX_DATA_STREAMS];
    int numInfoStreams = 0;
    int infoStreams[MAX_DATA_STREAMS];
    Serial.print("StateStr:");
    for(int i=0;i<32;i++){
      if(stateString[i] != '\0'){
        Serial.print(stateString[i]);
      }else{
        break;
      }
    }
    Serial.println();
    for(int i=0;i<MAX_DATA_STREAMS;i++){
      if( ((millis()-dataStreamLastUpdate[i]) < (int64_t)350) &&  doDispDataStream[i]){
        if(dataStreamIsInfo[i]){
          infoStreams[numInfoStreams] = i;
          numInfoStreams++;
        }else{
          activeStreams[numActiveStreams]=i;
          numActiveStreams++;
        }
      }
    }
    Serial.println("streams counted");
    if(numActiveStreams != prevStreamNo){
      glcd.clear();
    }
    if(numActiveStreams == 0){
      Serial.print("no active streams");
      glcd.drawBigString_24(0,0,"No Streams",LCDWIDTH);
      glcd.drawBigString_24(0,3,"             ",LCDWIDTH);
      glcd.drawBigString_16(0,5,"HYDRA Meter",LCDWIDTH);
      glcd.drawString(0,7,"            ",LCDWIDTH);
    }else if(numActiveStreams == 1){
      Serial.println("1 active stream");
      int idx = activeStreams[0];
      char dstrng[16];
      char pstrng[16];
      Serial.print("idx:");Serial.println(idx);
      Serial.println("easyprint");
      easyEngFltPrintSepPx(dstrng,pstrng,dataStreamValues[idx]);
      Serial.println("draw big");
      Serial.print(dstrng);Serial.println(pstrng);
      int xe = 0;
      if(dataStreamIDs[idx][0] == 'R' && dataStreamIDs[idx][1] == '\0'){
        if(dataStreamValues[idx] < 0 || dataStreamValues[idx] >= 9999999.0){
          easyEngFltPrintSepPx(dstrng,pstrng,10000000);
          xe = glcd.drawBigString_48(0,2,dstrng,LCDWIDTH-1);
        }else{
          xe = glcd.drawBigString_48(0,2,dstrng,LCDWIDTH-1);
        }
      }else{
        xe = glcd.drawBigString_48(0,2,dstrng,LCDWIDTH-1);
      }
      if(pstrng[0] != '\0'){
        glcd.drawBigString_24(xe,4,pstrng,LCDWIDTH-1);
        //xe = xe + glcd.drawBigString(xe+2,4,dataStreamUnits[idx],LCDWIDTH-1);
      }
      Serial.print("Print units:");
      xe+=2;
      Serial.println(dataStreamUnits[idx]);
      glcd.drawBigString_16(xe,2,dataStreamUnits[idx],LCDWIDTH);
      Serial.println("P label");
      Serial.println(dataStreamLabels[idx]);
      glcd.drawBigString_8(0,0,dataStreamLabels[idx],LCDWIDTH);
      if(numInfoStreams > 0){
        //easyEngFltPrint(dstrng,dataStreamStringVal[infoStreams[0]]);
        xe = glcd.drawBigString_8(0,1,dataStreamLabels[infoStreams[0]],LCDWIDTH);
        Serial.printf("Infostr0:%s xe=%d",dataStreamLabels[infoStreams[0]],xe);
        if(dataStreamIsStringVal[infoStreams[0]]){
          xe += glcd.drawBigString_8(xe+2,1,dataStreamStringVal[infoStreams[0]],LCDWIDTH);
          Serial.printf(" = %s\n",dataStreamStringVal[infoStreams[0]]);
        }else{
          easyEngFltPrint(dstrng,dataStreamValues[infoStreams[0]]);
          xe += glcd.drawBigString_8(xe+2,1,dstrng,LCDWIDTH);
          Serial.printf(" = %s\n",dstrng);
        }
        xe += glcd.drawBigString_8(xe+2,1,dataStreamUnits[infoStreams[0]],LCDWIDTH);
      }
      Serial.println("Finished 1 stream");
    }else if(numActiveStreams == 2){
      int ind = 0;
      Serial.print("2 active streams");
      for(int i=0;i<numActiveStreams;i++){
        char strng[16];
        int idx = activeStreams[i];
        easyEngFltPrint(strng,dataStreamValues[idx]);
        //sprintf(strng,"%1.2f",dataStreamValues[idx]);
        int xe = glcd.drawBigString_24(0,2+i*3,strng,LCDWIDTH);
        xe+=2;
        glcd.drawBigString_24(xe,2+i*3,dataStreamUnits[idx],LCDWIDTH-1);
        Serial.print("Lab:");
        Serial.print(idx);
        Serial.print(">");
        Serial.println(&dataStreamLabels[idx][0]);
        xe =  5 + glcd.drawBigString_8(0,i,dataStreamLabels[idx],LCDWIDTH);
        
        if(numInfoStreams > i){
          xe += glcd.drawBigString_8(xe+2,i,dataStreamLabels[infoStreams[i]],LCDWIDTH);
          if(dataStreamIsStringVal[infoStreams[i]]){
            xe += glcd.drawBigString_8(xe+2,i,dataStreamStringVal[infoStreams[i]],LCDWIDTH);
          }else{
            char dstrng[32];
            easyEngFltPrint(dstrng,dataStreamValues[infoStreams[i]]);
            xe += glcd.drawBigString_8(xe+2,i,dstrng,LCDWIDTH);
          }
          xe += glcd.drawBigString_8(xe+2,i,dataStreamUnits[infoStreams[i]],LCDWIDTH);
        }
        ind++;
      }
    }else if(numActiveStreams == 3){
      Serial.print("3 active streams");
      for(int i=0;i<numActiveStreams;i++){
        char strng[16];
        int idx = activeStreams[i];
        easyEngFltPrint(strng,dataStreamValues[idx]);
        //sprintf(strng,"%1.2f",dataStreamValues[idx]);
        int xe = glcd.drawBigString_16(0,2+i*2,strng,LCDWIDTH);
        xe+=2;
        xe += glcd.drawBigString_8(xe,3+i*2,dataStreamUnits[idx],LCDWIDTH-1);
        Serial.print("Lab:");
        Serial.print(idx);
        Serial.print(">");
        Serial.println(&dataStreamLabels[idx][0]);
        xe+=2;
        xe += glcd.drawBigString_8(xe,3+i*2,dataStreamLabels[idx],LCDWIDTH);
        
        if(numInfoStreams > i){
          xe = glcd.drawBigString_8(0,i,dataStreamLabels[infoStreams[i]],LCDWIDTH);
          if(dataStreamIsStringVal[infoStreams[i]]){
            xe += glcd.drawBigString_8(xe+2,i,dataStreamStringVal[infoStreams[i]],LCDWIDTH);
          }else{
            char dstrng[32];
            easyEngFltPrint(dstrng,dataStreamValues[infoStreams[i]]);
            xe += glcd.drawBigString_8(xe+2,i,dstrng,LCDWIDTH);
          }
          xe += glcd.drawBigString_8(xe+2,i,dataStreamUnits[infoStreams[i]],LCDWIDTH);
        }
      }
    }else{
      Serial.println(">3 active streams");
      int mult = 3;
      if(numActiveStreams > 6){
        mult = 2;
        if(numActiveStreams > 8){
          Serial.println("Too many streams! showing 8");
        }
      }
      for(int i=0;i<numActiveStreams;i++){
        char strng[16];
        int idx = activeStreams[i];
        sprintf(strng,"%s:",dataStreamLabels[idx]);
        int zeroX = (i%2==0) ? 0 : 63;
        glcd.drawString(zeroX,mult*(i/2),strng,LCDWIDTH);
        easyEngFltPrint(strng,dataStreamValues[idx]);
        int xe = zeroX + glcd.drawBigString_8(zeroX, mult*(i/2)+1 ,strng,LCDWIDTH);
        xe+=2;
        xe += glcd.drawString(xe,mult*(i/2) + 1,dataStreamUnits[idx],LCDWIDTH);
      }
    }
    //glcd.drawBigString_48(0,1,&str2[2],LCDWIDTH);
    Serial.println("End of loop actions");
    glcd.st7565_set_brightness(0x17);
    delay(5);
    glcd.display();
    glcd.st7565_set_brightness(0x20);
    delay(10);
    glcd.st7565_set_brightness(0x1B);
    Serial.println("Disp updated");
  }/*else{
    char strng[8];
    for(int i=0;i<8;i++){
      strng[i] = (char) ((((millis()/2000) % 8)*8)+i+64);
    }
    int xe = glcd.drawBigString(0,4,strng,LCDWIDTH);

    glcd.st7565_set_brightness(0x17);
    delay(5);
    glcd.display();
    glcd.st7565_set_brightness(0x20);
    delay(10);
    glcd.st7565_set_brightness(0x1B);
    //xe+=2;
    //glcd.drawBigString(xe,2+i*3,dataStreamUnits[idx],LCDWIDTH-1);
  }*/

  if(digitalRead(PIN_I) == LOW){
    digitalWrite(BACKLIGHT_LED,HIGH);
  }else{
    digitalWrite(BACKLIGHT_LED,LOW);
  }
}
//15->I
//14->II
//13->L
//12->R
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  randomSeed(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(128);
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      glcd.drawbitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
    }
    glcd.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      glcd.drawbitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, 0);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > 64) {
	icons[f][XPOS] = random(128);
	icons[f][YPOS] = 0;
	icons[f][DELTAY] = random(5) + 1;
      }
    }
  }
}


void testdrawchar(void) {
  for (uint8_t i=0; i < 168; i++) {
    glcd.drawchar((i % 21) * 6, i/21, i);
  }    
}

void testdrawcircle(void) {
  for (uint8_t i=0; i<64; i+=2) {
    glcd.drawcircle(63, 31, i, BLACK);
  }
}


void testdrawrect(void) {
  for (uint8_t i=0; i<64; i+=2) {
    glcd.drawrect(i, i, 128-i, 64-i, BLACK);
  }
}

void testfillrect(void) {
  for (uint8_t i=0; i<64; i++) {
      // alternate colors for moire effect
    glcd.fillrect(i, i, 128-i, 64-i, i%2);
  }
}

void testdrawline() {
  for (uint8_t i=0; i<128; i+=4) {
    glcd.drawline(0, 0, i, 63, BLACK);
  }
  for (uint8_t i=0; i<64; i+=4) {
    glcd.drawline(0, 0, 127, i, BLACK);
  }

  glcd.display();
  delay(1000);

  for (uint8_t i=0; i<128; i+=4) {
    glcd.drawline(i, 63, 0, 0, WHITE);
  }
  for (uint8_t i=0; i<64; i+=4) {
    glcd.drawline(127, i, 0, 0, WHITE);
  }
}