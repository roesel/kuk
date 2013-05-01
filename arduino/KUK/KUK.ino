/*
Name: Circular ball accelerator controller
Author: Jan Nikl
Year: 2013
*/

#define INT_GATE_1      0
#define INT_GATE_2      1
#define PIN_GATE_1      2
#define PIN_GATE_2      3
#define PIN_COIL_1  		6
#define PIN_COIL_2  		7

#define TRACK_LENGTH    60000.0f
#define TRACK_GATES     4

#define TIMER_ITER      137

#define INFO_VERSION    "INFO:Circular ball accelerator controller v1.3"
#define INFO_HELP       "INFO:Options:A-accelerate/S-stop/V-velocity/D[int]D-delay/1-coil 1/2-coil 2/X-coils off/P[int]M[int]C-aprox/R-rotations"

#define MSG_SUCCESS     "OK"
#define MSG_ERROR       "ERROR"

#define COIL_1_ON()  (digitalWrite(PIN_COIL_1,HIGH))
#define COIL_1_OFF() (digitalWrite(PIN_COIL_1,LOW))
#define COIL_2_ON()  (digitalWrite(PIN_COIL_2,HIGH))
#define COIL_2_OFF() (digitalWrite(PIN_COIL_2,LOW))

volatile int g_iDetect = 0;//last gate
volatile unsigned long g_ulTime1 = 0;//start time for velocity measurment
volatile unsigned long g_ulTime2 = 0;//end time for velocity measurment
volatile unsigned g_uCount = 0, g_uTotal = 0;//number of detections (for velocity and total)
volatile boolean g_bAccel = false;//accelerate
volatile boolean g_bVelocity = true;//velocity measument
volatile unsigned long g_ulDelay = 0;//wait loop iterations for delay
volatile boolean g_bAprox = false;//aprox delay
volatile unsigned g_uM = 0, g_uC = 0;//delay aproximation constants
volatile unsigned g_uRecVelocity = 0;//reciprocal velocity

void cmd_accelerate();//start acceleration
void cmd_stop();//stop acceleration
void cmd_velocity();//get velocity averaged from last call
void cmd_delay();//set fixed delay
void cmd_aprox();//set aprox parameters and enable it
void cmd_rotations();//get total rotations

void gate1();//ISR for gate 1
void gate2();//ISR for gate 2

void detection(int gate);//detection handling

void setup()
{ 
  //set IO modes to pins
  pinMode(PIN_GATE_1,INPUT);
  pinMode(PIN_GATE_2,INPUT);
  pinMode(PIN_COIL_1,OUTPUT);
  pinMode(PIN_COIL_2,OUTPUT);
  
 //initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  //set serial read timeout
  Serial.setTimeout(100);
  
  //init pseudorandom generator by analog input on unconnected pin
  randomSeed(analogRead(0));
  
  //attach interrupts from detectors
  attachInterrupt(INT_GATE_1,gate1,RISING);
  attachInterrupt(INT_GATE_2,gate2,RISING);
  
  //printout version
  Serial.println(F(INFO_VERSION));
} 

void loop()
{ 
  int iCmd;
  
  //wait for commands
  if(Serial.available() > 0)
  {
    //read a keyletter
    iCmd = Serial.read();
    
    /*
    //flush the rest
    while(Serial.available() > 0)
      Serial.read();
    */
    
    switch(iCmd)
    {
      //accelerate
      case 'A':
        cmd_accelerate();
        break;
      //stop
      case 'S':
        cmd_stop();
        break;
      //velocity
      case 'V':
        cmd_velocity();
        break;
      //delay
      case 'D':
        cmd_delay();
        break;
      //aprox
      case 'P':
      	cmd_aprox();
      	break;
    	//rotations
    	case 'R':
    		cmd_rotations();
    		break;
      //turn on coil 1
      case '1':
        COIL_2_OFF();
        COIL_1_ON();
        Serial.println(MSG_SUCCESS);
        break;
      //turn on coil 2
      case '2':
        COIL_1_OFF();
        COIL_2_ON();
        Serial.println(MSG_SUCCESS);
        break;
      //turn off coils
      case 'X':
        COIL_1_OFF();
        COIL_2_OFF();
        Serial.println(MSG_SUCCESS);
        break;
      //print help
      default:
        Serial.println(F(INFO_HELP));
    }
  }
}

///////////////////////////////////////
/////////////INTERRUPTS////////////////
///////////////////////////////////////

//detection on gate 1
void gate1()
{
  detection(1);
}

//detection on gate 2
void gate2()
{
  detection(2);
}

//detection
void detection(int iGate)
{
	unsigned uAprox = 0;
	
  //check order of detections
  if(iGate == g_iDetect % 2 + 1)
  {
    if(g_bVelocity)//velocity is measured? 
    {
      if(g_uCount==0)//no detections
        g_ulTime1 = millis();
    	
    	//aprox aprop. delay
    	if(g_bAprox && g_uCount >= 1)//&& g_bAccel
    	{
    		uAprox = ((g_uM * (unsigned long)(millis() - g_ulTime2))/1000 + g_uC);
    		
    		//+debug
    		//Serial.println(millis() - g_ulTime2);
    		//Serial.println(uAprox);
    		//-debug
    		
    		//convert from ms to wait loop iterations
    		g_ulDelay = (((unsigned long)uAprox)*1000)/TIMER_ITER;
    	}
    	
      g_ulTime2 = millis();
      //increase time - time is not increased in interrupts!
			if(g_bAccel)
      	g_ulTime2 += uAprox;
      g_uCount++;
      g_uTotal++;
      
      //calc reciprocal velocity
      g_uRecVelocity = ((TRACK_GATES * (g_ulTime2 - g_ulTime1))/(g_uCount));
    }
  }
  
  g_iDetect = iGate;
  
  if(g_bAccel)
  {
  	//wait loop - one iter. takes 137 us
    for(unsigned long u=g_ulDelay;u>0;u--)
      random(65535);
    
    //switch solenoids
    switch(iGate)
    {
      case 1:
        COIL_1_OFF();
        COIL_2_ON();
        break;
      case 2:
        COIL_2_OFF();
        COIL_1_ON();
        break;
    }
  }
}

///////////////////////////////////////
//////////////COMMANDS/////////////////
///////////////////////////////////////

void cmd_accelerate()
{
  g_bAccel = true;
  
  COIL_2_OFF();
  COIL_1_ON();
  
  Serial.println(MSG_SUCCESS);
}

void cmd_stop()
{
  g_bAccel = false;
  
  COIL_1_OFF();
  COIL_2_OFF();
  
  Serial.println(MSG_SUCCESS);
}

void cmd_velocity()
{
  unsigned long ulTime;
  //unsigned uCount;
  float fVelocity;
  
  if(g_uCount<=1)
  {
    Serial.print(MSG_ERROR);
    Serial.println(": not enough detections");
    return;
  }
  
  //track length is confusingly used for RPM calc currently!
  fVelocity = TRACK_LENGTH / (float)(g_uRecVelocity);
  
  Serial.println(fVelocity,4);
}

void cmd_delay()
{
  int iDelay;
  
  iDelay = Serial.parseInt();
  
  if(iDelay < 0)
  	goto ERROR_PAR;
  
  if(Serial.read()!='D')
		goto ERROR_PAR;
  
  g_bAprox = false;
  //convert to wait loop iterations
  g_ulDelay = (((unsigned long)iDelay)*1000)/TIMER_ITER;
  //Serial.println(g_ulDelay);
  
  Serial.println(MSG_SUCCESS);
  return;
  
ERROR_PAR:
	Serial.print(MSG_ERROR);
  Serial.println(": bad parameter");
  return;
}

void cmd_aprox()
{
	int iM,iC;
		
	iM = Serial.parseInt();
	if(iM < 0)
		goto ERROR_PAR;
	
	if(Serial.read()!='M')
		goto ERROR_PAR;
	
	iC = Serial.parseInt();
	if(iC < 0)
		goto ERROR_PAR;
	
	if(Serial.read()!='C')
		goto ERROR_PAR;
	
	g_bAprox = false;
	g_uM = iM;
	g_uC = iC;
	g_bAprox = true;
	
	Serial.println(MSG_SUCCESS);
	
	return;
	
ERROR_PAR:
	Serial.print(MSG_ERROR);
  Serial.println(": bad parameter");
  return;
}

void cmd_rotations()
{
	Serial.println(((float)g_uTotal)/TRACK_GATES);
}

