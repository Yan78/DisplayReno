#ifndef ELM_TIMEOUT
	#warning "ELM_TIMEOUT must be defined."
	#define ELM_TIMEOUT 2000
#endif

#ifndef ELM_BAUD_RATE
	#warning "ELM_BAUD_RATE must be defined"
	#define ELM_BAUD_RATE 38400
#endif

#ifndef ELM_PORT
	#warning "ELM_PORT is not defined, using default value"
	#define ELM_PORT mySerial
#endif

#define ELM_SUCCESS 0

#define ELM_NO_RESPONSE 1

#define ELM_BUFFER_OVERFLOW 2

#define ELM_GARBAGE 3

#define ELM_UNABLE_TO_CONNECT 4

#define ELM_NO_DATA 5

byte Elm327_begin(){	
    ELM_PORT.begin(ELM_BAUD_RATE);
    char data[20];
    Elm327_runCommand("AT E0",data,20);
    delay(100);
    return Elm327_runCommand("AT SP 0",data,20);
}

byte Elm327_runCommand(const char *cmd, char *data, unsigned int dataLength)
{	
//	byte cmdLength;
	
	// Flush any leftover data from the last command.
	
	// Send the specified command to the controller.
	Elm327_flush();
	ELM_PORT.print(cmd);
	ELM_PORT.print('\r');
    
	unsigned long timeOut;
        int counter;
	bool found;
	
	// Start reading the data right away and don't stop 
	// until either the requested number of bytes has 
	// been read or the timeout is reached, or the >
	// has been returned.
	//
	counter=0;
	timeOut=millis()+ELM_TIMEOUT;
	found=false;
	while (!found && counter<( dataLength ) && millis()<timeOut)
    {
        if ( ELM_PORT.available() ){
			data[counter]=ELM_PORT.read();
			if (  data[counter] == '>' ){
				found=true;
				data[counter]='\0';
			}else{
				++counter;
			}
        }
    }
	// If there is still data pending to be read, raise OVERFLOW error.
	if (!found  && counter>=dataLength)
	{
		// Send a character, this should cancel any operation on the elm device
		// so that it doesnt spuriously inject a response during the next 
		// command
		ELM_PORT.print("XXXXXXXXX\r\r\r");
		delay(300);
		return ELM_BUFFER_OVERFLOW;
	}
    
	// If not found, and there is still buffer space, then raise no response error.
    if (!found && counter<dataLength){
		// Send a character, this should cancel any operation on the elm device
		// so that it doesnt spuriously inject a response during the next 
		// command
		ELM_PORT.print("XXXXXXXXX\r\r\r");
		delay(300);
		return ELM_NO_RESPONSE;
    }

	char *match;
	match=strstr(data,"UNABLE TO CONNECT");
	if (match != NULL){
		return ELM_UNABLE_TO_CONNECT;
	}
	match=strstr(data,"NO DATA");
	if (match != NULL){
		return ELM_NO_DATA;
	}
	if (strncmp(data,"SEARCHING...",12)==0)
	{
		// Remove searching...
		byte i=12;
		while (data[i]!='\0'){
			data[i-12]=data[i];
			i++;
		}
		data[i]='\0';
	}

	// Otherwise return success.
	return ELM_SUCCESS;
}

void Elm327_flush(){
	while (ELM_PORT.read() >= 0)
		   ; // do nothing
}

byte Elm327_getVoltage(float &voltage){
	char data[20];
	byte status;
	char cmd[]="ATRV";
	status=Elm327_runCommand(cmd,data,20);
	if (status==ELM_SUCCESS){
		voltage=atof(data);
	}
	return status;
}

byte Elm327_coolantTemperature(int &temp){
	byte status;
	byte values[1];
	status=Elm327_getBytes("01","41","05",values,1);
	if (status != ELM_SUCCESS){
		return status;
	}
	temp=values[0]-40;
	return ELM_SUCCESS;
}

byte Elm327_intakeAirTemperature(int &temperature){
	byte status;
	byte values[1];
	status=Elm327_getBytes("01","41","0F",values,1);
	if (status != ELM_SUCCESS){
		return status;
	}
	temperature=values[0]-40;
	return ELM_SUCCESS;
}

byte Elm327_intakeManifoldAbsolutePressure(byte &pressure){
	byte status;
	byte values[1];
	status=Elm327_getBytes("01","41","0B",values,1);
	if (status != ELM_SUCCESS){
		return status;
	}
	pressure=values[0];
	return ELM_SUCCESS;
}

byte Elm327_getBytes( const char *mode, const char *chkMode, const char *pid, byte *values, unsigned int numValues){
	char data[64];
	byte status;
	char hexVal[]="0x00";
	char cmd[5];
	cmd[0]=mode[0];
	cmd[1]=mode[1];
	cmd[2]=' ';
	cmd[3]=pid[0];
	cmd[4]=pid[1];
	cmd[5]='\0';

	status=Elm327_runCommand(cmd,data,64);
	if ( status != ELM_SUCCESS )
	{
		return status;
	};
	
	// Check the mode returned was the one we sent
	if ( data[0]!=chkMode[0] 
	  or data[1]!=chkMode[1]
	  or data[3]!=pid[0]
	  or data[4]!=pid[1] ){
		return ELM_GARBAGE;
	}
	
	// For each byte expected, package it up
	int i=0;
	for (int i=0; i<numValues; i++){
		hexVal[2]=data[6+(3*i)];
		hexVal[3]=data[7+(3*i)];
		values[i]=strtol(hexVal,NULL,16);
	}
	return ELM_SUCCESS;
}

