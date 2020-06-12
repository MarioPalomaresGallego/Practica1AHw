#include <Arduino.h>

// --------------------------------------
// INTERFACES
// --------------------------------------
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <Wire.h>

// --------------------------------------
// GLOBAL VARIABLES
// --------------------------------------
float speed = 55.0;
int speed_int;
int speed_dec;
unsigned long time1;
unsigned long time2;
int sc = 0;
long diff;

//Variables para saber que tarea hay que ejecutar
int req_gas = 0;
int req_brake = 0;
int req_mix = 0;

//Variables de estado de los actuadores
int state_gas = 0;
int state_slope = 1;
int state_brake = 0;

//Variable que recoge para todas las tareas el estado nuevo al que cambiar
int req_state =0;

//Variables para recibir y enviar respuestas
char request[10];
char answer[10];

// --------------------------------------
// Function: comm_server
// --------------------------------------

void comm_server(int howmany)
{
    int i;

    // while there is enough data for a request
    if (Wire.available() >= 9) {
        // read the request
        i=0;
        while ( (i<9) && (Wire.available() >= (9-i)) ) {
            // read one character
            request[i]=Wire.read();

            // if the new line is positioned wrong
            if ( (i!=8) && (request[i]=='\n') ) {
                // Send error and start all over again
                sprintf(answer,"MSG: ERR\n");
                memset(request,'\0', 9);
                i=0;
            } else {
              // read the next character
              i++;
            }
        }
        request[9]='\0';
    }
}

void execute(){
    // cast the request
    if (0 == strcmp("SPD: REQ\n",request)) {
		sprintf(answer,"SPD:%02d.%d\n",speed_int,speed_dec);
    }else if (0 == strcmp("GAS: SET\n",request)){
		req_gas = 1;
		req_state = 1;
		sprintf(answer,"GAS:  OK\n");
    }else if (0 == strcmp("GAS: CLR\n",request)){
		req_gas =1;
		req_state =0;
		sprintf(answer,"GAS:  OK\n");
    }else if (0 == strcmp("BRK: SET\n",request)){
		req_brake =1;
		req_state = 1;
		sprintf(answer,"BRK:  OK\n");
    }else if (0 == strcmp("BRK: CLR\n",request)){
		req_brake =1;
		req_state = 0;
		sprintf(answer,"BRK:  OK\n");
    }else if (0 == strcmp("MIX: SET\n",request)){
		req_mix =1;
		req_state =1;
		sprintf(answer,"MIX:  OK\n");
    }else if (0 == strcmp("MIX: CLR\n",request)){
		req_mix =1;
		req_state = 0;
		sprintf(answer,"MIX:  OK\n");
    }else if (0 == strcmp("SLP: REQ\n",request)){
		switch(state_slope){
			case 0:
			  sprintf(answer,"SLP:DOWN\n");
			  break;
			case 1:
			  sprintf(answer,"SLP:FLAT\n");
			  break;
			case 2:
			  sprintf(answer,"SLP:  UP\n");
		}
    }else{
        // error, send error message
        sprintf(answer,"MSG: ERR\n");
    }
    Wire.write(answer);
}


int task_gas(){
	if(req_gas){
		//Serial.println("GAS");
		if(req_state){
			digitalWrite(13,HIGH);
			state_gas =1;
		}else{
			digitalWrite(13,LOW);
			state_gas =0;
		}
		req_gas = 0;
	}
	return 0;
}

int task_brake(){
	if(req_brake){
		//Serial.println("BRK");
		if(req_state){
			digitalWrite(12,HIGH);
			state_brake =1;
		}else{
			digitalWrite(12,LOW);
			state_brake = 0;
		}
		req_brake = 0;
	}
	return 0;
}

int task_mixer(){
	if(req_mix){
		//Serial.println("MIX");
		if(req_state){
			digitalWrite(11,HIGH);
		}else{
			digitalWrite(11,LOW);
		}
		req_mix = 0;
	}
	return 0;
}

int task_speed(){
	float speed_led;
	float acceleration=0;
	//Obtenemos el valor de la aceleración en el momento de la tarea
	if(state_gas) acceleration += 0.5;
	if(state_brake) acceleration -=0.5;
	switch (state_slope){
		case 0:
		  acceleration +=0.25;
		  break;
		case 2:
		  acceleration -=0.25;
	}
	//Calculamos la nueva velocidad
	speed = speed + acceleration * 0.1;
	speed_int = (int) speed;
	speed_dec = ((int) (speed*10))%10;
	speed_led = map(speed,40,70,0,255);
	analogWrite(10,speed_led);
	return 0;
}

int task_slope(){
	int up = digitalRead(9);
	int down = digitalRead(8);
	if(up){
		state_slope = 2;
	}else if(down){
		state_slope = 0;
	}else{
		state_slope = 1;
	}
	return 0;
}

// --------------------------------------
// Function: setup
// --------------------------------------
void setup() {
    time1 = micros();
    //Serial.begin(9600);
    Wire.begin(6);
    Wire.onReceive(comm_server);
    Wire.onRequest(execute);
    pinMode(13,OUTPUT); //Acelerador
    pinMode(12,OUTPUT); //Freno
    pinMode(11,OUTPUT); //Mixer
    pinMode(10,OUTPUT); //Velocidad
    pinMode(9,INPUT);  //Pendiente Alta
    pinMode(8,INPUT);  //Pendiente Baja
}

// --------------------------------------
// Function: loop
// --------------------------------------
void loop() {
    switch(sc){
      case 0:
        task_speed();
        task_slope();
        task_gas();
        task_brake();
        task_mixer();
        break;
      case 1:
        task_speed();
        task_slope();
        task_gas();
        task_brake();
        task_mixer();
    }
    sc = (sc+1) % 2;
    time2 = micros();
    if(time1< time2) diff = time2-time1;
    else{
      diff = ULONG_MAX - time1 + time2;
    }
    diff = 100000 - diff;
    if(diff>0){
      delay(diff/1000);
      delayMicroseconds(diff - (diff/1000)*1000);
    }
    else return;
    time1 += 100000;
}
