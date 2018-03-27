#line 1 "E:/Acer_D_Backup/DEVELOPMENT/PROJECTS/micros/openKeyboard03/openKeyboard.c"
#line 39 "E:/Acer_D_Backup/DEVELOPMENT/PROJECTS/micros/openKeyboard03/openKeyboard.c"
const char scanMap[50] = {
 8, 9, 10, 40, 20, 30, 47,
 41, 42, 43, 39, 44, 38, 45,
 31, 32, 33, 35, 34, 36, 37,
 21, 22, 23, 25, 24, 26, 27,
 11, 12, 13, 15, 14, 16, 17,
 1, 2, 3, 5, 4, 6, 7,
 28, 18, 19, 0, 0, 29, 46 };

const char serialMap[4][49] = {

{56, 57, 48, 13, 112, 44, 0,
 0, 0, 0, 46, 32, 109, 0,
 0, 122, 120, 118, 99, 98, 110,
 97, 115, 100, 103, 102, 104, 106,
113, 119, 101, 116, 114, 121, 117,
 49, 50, 51, 53, 52, 54, 55,
107, 105, 111, 0, 0, 108, 8},


{56, 57, 48, 13, 80, 44, 0,
 0, 0, 0, 46, 32, 77, 0,
 0, 90, 88, 86, 67, 66, 78,
 65, 83, 68, 71, 70, 72, 74,
 81, 87, 69, 84, 82, 89, 85,
 49, 50, 51, 53, 52, 54, 55,
 75, 73, 79, 0, 0, 76, 8},


{56, 57, 48, 13, 41, 58, 0,
 0, 0, 0, 63, 32, 62, 0,
 0, 96, 39, 45, 187, 124, 60,
126, 154, 123, 168, 125, 47, 39,
 33, 64, 128, 37, 35, 94, 38,
 49, 50, 51, 53, 52, 54, 55,
 91, 42, 40, 0, 0, 93, 8},


{56, 57, 48, 13, 61, 59, 0,
 0, 0, 0, 191, 32, 181, 0,
 0, 230, 156, 95, 231, 43, 241,
225, 223, 240, 165, 163, 92, 34,
161, 229, 232, 254, 36, 253, 249,
 49, 50, 51, 53, 52, 54, 55,
169, 236, 242, 0, 0, 248, 8}
};

 unsigned char data[7] = {0,0,0,0,0,0,0};
 unsigned char oldData[7] = {0,0,0,0,0,0,0};
 unsigned char justPressed[7] = {0,0,0,0,0,0,0};

 unsigned char newlyPressd = 0;

 unsigned char AMask = 0;
 unsigned char BMask = 0;

 unsigned char somethingChanged = 0;

 unsigned long int timer = 0xFFFF;
 unsigned long int lightFadeOffTime = 0x8000;



 unsigned char alts [] = {0, 1, 2, 2, 3, 3, 3, 3};
 unsigned char alt;

 void (*send)() = 0;
 void (*lightHandler)() = 0;

 unsigned char baudrate = 0;
 unsigned char sendMode = 0;
 unsigned char lightMode = 0;
 unsigned char lightStrenght = 0;

 unsigned char pwmRunning = 0;

 void enterPowerSave(){
 PORTA = 0;
 INTCON.RBIE = 1;
 asm{
 sleep;
 }
 }

 void exitPowerSave(){
 INTCON.RBIE = 0;
 }

 void setupUsart(unsigned char br){
 switch(br){
 case  1 : USART_Init(300);
 break;
 case  2 : USART_Init(1200);
 break;
 case  3 : USART_Init(2400);
 break;
 case  4 : USART_Init(4800);
 break;
 case  5 : USART_Init(9600);
 break;
 case  6 : USART_Init(19200);
 break;
 case  7 : USART_Init(38400);
 break;
 case  8 : USART_Init(57600);
 break;
 case  9 : USART_Init(115200);
 break;
 default: EEprom_write( 1 ,  4 );
 br =  4 ;
 USART_Init(4800);
 }
 baudrate = br;
 }

 void turnBacklightOn(){
 timer = lightFadeOffTime;
 if (!pwmRunning) {
 PWM1_Start();
 PWM2_Start();
 pwmRunning = 1;
 }
 PWM1_Change_Duty(lightStrenght);
 PWM2_Change_Duty(lightStrenght);
 }

 void nop(){

 }

 void lightFade(){
 unsigned char s;
 if (newlyPressd) {
 turnBacklightOn();
 return;
 }

 if (timer>0) timer --;
 else {
 PWM1_Stop;
 PWM2_Stop;
 PORTC = 255;
 pwmRunning = 0;
 enterPowerSave();
 }

 if (timer<(~lightStrenght)){
 PWM1_Change_Duty(~((unsigned char)timer));
 PWM2_Change_Duty(~((unsigned char)timer));
 }

 }

 void setupLightMode(unsigned char lm){

 if (pwmRunning){
 PWM1_Stop();
 PWM2_Stop();
 }

 switch (lm){
 case  0 : lightHandler = &nop;
 lightStrenght = 0xFF;
 lightMode =  0 ;
 break;

 case  1 : lightHandler = &nop;
 turnBacklightOn();
 lightMode =  1 ;
 break;

 case  2 : lightHandler = &lightFade;
 turnBacklightOn();
 lightStrenght = 0x80;
 lightMode =  2 ;
 break;

 default : EEprom_write( 3 ,  0 );
 lightMode =  0 ;
 lightHandler = &nop;
 lightStrenght = 0xFF;
 }

 PORTC = 255;


 if (lightMode !=  0 ) {
 PWM1_Init(40000);
 PWM1_Change_Duty(lightStrenght);
 PWM1_Start();

 PWM2_Init(40000);
 PWM2_Change_Duty(lightStrenght);
 PWM2_Start();

 pwmRunning = 1;
 }
 }

 void doScan(){
 unsigned char sc = 0;

 newlyPressd = 0;

 for (AMask=1; AMask<128; AMask<<=1){

 oldData[sc] = data[sc];

 PORTA = ~AMask;
 data[sc] = PORTB;
 data[sc] = data[sc] | PORTB;
 data[sc] = data[sc] | PORTB;
 data[sc] = ~data[sc];

 justPressed[sc] = data[sc]&(~oldData[sc]);

 newlyPressd = newlyPressd | justPressed[sc];

 sc++;
 }
 }


 void serialSend(){
 unsigned char idx = 0;
 unsigned char sc = 0;

 for (AMask=1; AMask<128; AMask<<=1){
 for (BMask=1; BMask<128; BMask<<=1){
 if (justPressed[sc] & BMask) USART_Write(serialMap[alt][idx]);
 idx++;
 }
 sc++;
 }
 alt = alts[data[2]&1|((data[1]&1)<<1)|((data[0]&64)>>4)];
 }

 void scanSend(){
 unsigned char idx = 0;
 unsigned char sc = 0;
 unsigned char bs = 0;

 for (AMask=1; AMask<128; AMask<<=1){
 somethingChanged = data[sc] ^ oldData[sc];
 bs = 0;
 for (BMask=1; BMask<128; BMask<<=1){
 if (someThingChanged & BMask)
 USART_Write(scanMap[idx] | (((data[sc]&BMask)>>bs)<<6));
 bs++;
 idx++;
 }
 sc++;
 }
 }

 void ps2Send(){

 }

 void setupSendMode(unsigned char sm){
 switch (sm){
 case  1  : send = &scanSend;
 sendMode =  1 ;
 break;

 case  0 : send = &serialSend;
 sendMode =  0 ;
 break;





 default : send = &serialSend;
 EEprom_write( 2 ,  0 );
 sendMode =  0 ;
 }
 }

 void config(){
 timer = lightFadeOffTime;

 if ((data[1]&4)&&(lightStrenght<0xff)) {
 lightStrenght++;
 PWM1_Change_Duty(lightStrenght);
 PWM2_Change_Duty(lightStrenght);
 if (lightMode== 0 ) lightMode =  2 ;
 }

 if ((data[1]&64)&&(lightStrenght>0)) {
 lightStrenght--;
 PWM1_Change_Duty(lightStrenght);
 PWM2_Change_Duty(lightStrenght);
 if (lightMode== 0 ) lightMode =  2 ;
 }


 if (justPressed[5]&1){



 setupLightMode( 2 );
 lightFadeOffTime = 2<<8;

 }

 if (justPressed[5]&2){
 if (data[1]&1)
 ;
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 4<<8;
 }
 }

 if (justPressed[5]&4){
 if (data[1]&1)
 ;
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 8<<8;
 }
 }

 if (justPressed[5]&16){
 if (data[1]&1)
 ;
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 16<<8;
 }
 }

 if (justPressed[5]&8){
 if (data[1]&1)
 setupUsart( 4 );
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 32<<8;
 }
 }

 if (justPressed[5]&32){
 if (data[1]&1)
 setupUsart( 5 );
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 64<<8;
 }
 }

 if (justPressed[5]&64){
 if (data[1]&1)
 ;
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 128<<8;
 }
 }

 if (justPressed[0]&1){
 if (data[1]&1)
 ;
 else {
 setupLightMode( 2 );
 lightFadeOffTime = 255<<8;
 }
 }

 if (justPressed[0]&2){
 if (data[1]&1)
 ;
 else
 setupLightMode( 1 );
 }

 if (justPressed[0]&4){
 if (data[1]&1)
 ;
 else
 setupLightMode( 0 );
 }

 if (justPressed[3]&1){
 setupSendMode( 1 );
 }

 if (justPressed[3]&2){
 setupSendMode( 0 );
 }

 Delay_5ms();

 }

 void saveConfig(){
 EEprom_write( 1 , baudrate);
 EEprom_write( 2 , sendMode);
 EEprom_write( 3 , lightMode);
 EEprom_write( 4 , lightStrenght);
 EEprom_write( 5 , lightFadeOffTime>>8);
 }

 void interrupt() {
 if (INTCON.RBIF) {
 exitPowerSave();
 INTCON.RBIF = 0;
 }




 }

void main() {

 OSCCON = 0x67;

 ANSEL = 0;
 ANSELH = 0;

 PORTA = 0;
 TRISA = 0;

 PORTB = 0;
 TRISB = 255;

 PORTC = 0;
 TRISC = 0;

 OPTION_REG = 0;




 INTCON.RBIE = 0;
 INTCON.RBIF = 0;

 IOCB = 127;

 PIE1 = 32;

 INTCON.GIE = 1;


 Delay_ms(20);
 sendMode = EEprom_read( 2 );
 setupSendMode(sendMode);



 Delay_ms(20);
 baudrate = EEprom_read( 1 );
 setupUsart(baudrate);


 Delay_ms(20);
 lightFadeOffTime = EEprom_read( 5 ) << 8;
 lightStrenght = EEprom_read( 4 );

 Delay_ms(20);
 lightMode = EEprom_read( 3 );
 setUpLightMode(lightMode);

 do {

 doScan();

 if (data[1]&2) {
 config();
 }else{
 if (oldData[1]&2) saveConfig();
 send();
 }
 lightHandler();
 Delay_80us();
 } while (1);

}
