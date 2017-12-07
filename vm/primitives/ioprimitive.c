#include "ioprimitive.h"

/* i/o primitives 
basically, files are all kept in a large array.
File operations then just give an index into this array 
IO primitives are 120-139
 we assume this is initialized to NULL */
static FILE *fp[MAXFILES]= {0};
#ifndef IMG_BUILD
object edit(char* buff, int size)
{
	object returnedObject;
	initEditor();
	editorInput(buff, size);
	int len;
	editorSetStatusMessage(
		"HELP: Ctrl-C = commit | Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
	while(edStatus() == ED_ON) {
		editorRefreshScreen();
		editorProcessKeypress(STDIN_FILENO);
	}
	//print content
	buff = editorRowsToString(&len);
	edFreeBuffer();
	returnedObject = newStString(buff);
	free(buff);
	return returnedObject;
}
#endif
/*120 - 129*/
object filePrimitive(int number, object* arguments)
{
    int i, j;
    char *p, buffer[1024];
    object returnedObject;
    returnedObject = nilobj;
	i = intValue(arguments[0])-1;
	if(i == 0 && !fp[i]) fp[i] = stdin;
	if(i == 1 && !fp[i]) fp[i] = stdout;
    switch (number) {
	    case 0:			/* file open */
		if(i < 2) return newInteger(i);
		p = charPtr(arguments[1]);
		fp[i] = fopen(p, charPtr(arguments[2]));
		if (fp[i] == NULL)
		    returnedObject = nilobj;
		else
		    returnedObject = newInteger(i);
		break;

	    case 1:			/* file close - recover slot */
		if (i >= 2 && fp[i])
		{
			ignore fclose(fp[i]);
			fp[i] = NULL;
		}
		break;

	    case 2:			/* stdout no return */
#ifndef IMG_BUILD
		printf("\n");
		fflush(stdout);
		returnedObject = edit(charPtr(arguments[0]), intValue(arguments[1]));
	
#endif
		break;
	
   	 	case 3:			/* read byte array */
		j =  intValue(arguments[1]);
		p = (char*) malloc(j);
		if(p)
		{
			j = fread(p,1, j,fp[i]);
			if(j>0)
			{
				returnedObject = newByteArray(j);
				memcpy(charPtr(returnedObject),p,j);
			}
			free(p);
			p = NULL;
		}
		break;

    	case 4:			/* write byte array */
		j = sizeField(arguments[1]);
		if (j < 0)
			j = (-j);
		returnedObject = newInteger(
			fwrite(charPtr(arguments[1]), 1, j, fp[i]));
		break;

	    case 5:			/* get line */
		if (!fp[i])
		    break;
		j = 0;
		buffer[j] = '\0';
		if(fp[i] == stdin)
		{
#ifndef IMG_BUILD
			tty_readline(buffer,sizeof(buffer),1);
			returnedObject = newStString(buffer);
#endif
		} else if (!fgets(buffer, 1024, fp[i]))
			returnedObject = nilobj;
		else
		{
		    j = strlen(buffer) - 1;
		    buffer[j] = '\0';
			    /* else we loop again */
			returnedObject = newStString(buffer);
		}
		break;
		
		case 6: // file path primitive
#ifndef IMG_BUILD
		j = intValue(arguments[0]);
		returnedObject = fp_primitive(j,&arguments[1]);
#endif;
		break;
			
   	 	case 7:			/* write an object image */
		break;

    	case 8:			/* print no return */
    	case 9:			/* print string */
		if (!fp[i])
	   	 	break;
#ifndef IMG_BUILD		
		if(fp[i] == stdout)
			//setvbuf(stdout, NULL, _IONBF, 0);
			printf(charPtr(arguments[1]));
		else
#endif
			ignore fputs(charPtr(arguments[1]), fp[i]);
			//setvbuf(stdout, NULL, _IOLBF, 0);
		//serial_write(charPtr(arguments[1]),strlen(charPtr(arguments[1])));
		if (number == 9) {
			ignore fputs("\n", fp[i]);
		}
		fflush(fp[i]);
		break;
	
		default:break;
	}
	return returnedObject;
}
/*UART primitive 130-133*/
#ifndef IMG_BUILD
extern FATFS FatFs;
extern DSTATUS fs_stt;
object fp_primitive(int number, object* arguments)
{
	object returnedObject,tmp;
	FILINFO fno;
	returnedObject = nilobj;
	FRESULT res;
	char buffer[1024];
	int size;
	DIR dir;
	FILE* old,*new;
	switch(number)
	{
		case 0: // check exist
		returnedObject = falseobj;
		if(f_stat(charPtr(arguments[0]), &fno) == FR_OK)
			returnedObject = trueobj;
		break;
		
		case 1: // is dir
		returnedObject = falseobj;
		if(f_stat(charPtr(arguments[0]), &fno) == FR_OK)
			if (fno.fattrib & AM_DIR)
				returnedObject = trueobj;
		break;
		
		case 2: // all child
		if(f_opendir(&dir, charPtr(arguments[0])) == FR_OK)
		{
			while(1)
			{
				res = f_readdir(&dir, &fno);
				if (res != FR_OK || fno.fname[0] == 0) break;
				strcpy(buffer,fno.fname);
				if(strcmp(buffer,".") != 0 && strcmp(buffer,"..") != 0)
				{
					tmp = newLink(nilobj,newStString(buffer));
					basicAtPut(tmp,3,returnedObject);
					returnedObject = tmp;
				}
				memset(buffer,'\0',sizeof(buffer));
			}
			f_closedir(&dir);
		}
		break;
		
		case 3: // mkdir
		returnedObject = falseobj;
		if(f_mkdir(charPtr(arguments[0])) == FR_OK)
			returnedObject = trueobj;
		break;
		
		case 4: //delete
		returnedObject = falseobj;
		if(f_unlink(charPtr(arguments[0])) == FR_OK)
			returnedObject = trueobj;
		break;
		
		case 5: // cpy
		old = fopen(charPtr(arguments[0]),"rb");
		if(!old)
			return falseobj;
		new = fopen(charPtr(arguments[1]),"wb");
		if(!new)
		{
			fclose(old);
			return falseobj;
		}
		while(!feof(old))
		{
			size = fread(buffer,1,sizeof(buffer),old);
			fwrite(buffer,1,size, new);
			memset(buffer,0,sizeof(buffer));
		}
		fclose(old);
		fclose(new);
		return trueobj;
		break;
		
		case 6: //move
		returnedObject = falseobj;
		if(f_rename(charPtr(arguments[0]), charPtr(arguments[1])) == FR_OK)
			returnedObject = trueobj;
		break;
		
		case 7://mount
		returnedObject = falseobj;
		fs_stt  = f_mount(&FatFs, "", 0);
		if(fs_stt == FR_OK)
			returnedObject = trueobj;
		break;
		
		case 8: // unmount
		returnedObject = falseobj;
		fs_stt  = f_mount(NULL, "", 0);
		if(fs_stt == FR_OK)
			returnedObject = trueobj;
		break;
		
		default:break;
	}
	return returnedObject;
}
#ifdef USE_X_MODEM
static int get_file_over_uart()
{
	int size, chunk;
	char buff[1024];
	FILE* fp = NULL;
	fp = fopen("log.txt","w");
	if(!fp) return 0;
	// receive file name
	memset(buff,'\0',sizeof(buff));
	xmodemReceive(buff,sizeof(buff));
	fputs("File :",fp);
	fwrite(buff, 1, strlen(buff), fp);
	fputs("\n",fp);
	if(strlen(buff) == 0) return 0;
	memset(buff,0,sizeof(buff));
	while((chunk = xmodemReceive(buff,sizeof(buff))) > 0)
	{	
		fwrite(buff, 1, chunk, fp);
		memset(buff,0,sizeof(buff));
	}
	fputs("End",fp);
	fclose(fp);
	return 1;
}
#endif
object gpioPrimitive(int number, object* arguments)
{
	object returnedObject = nilobj;
	int len;
	switch(number)
	{
		case 0: // read char 0 priv
		returnedObject = newInteger(serial_getc());
		break;
	
		case 1: // put char 1 priv
		serial_putc(intValue(arguments[0]));
		break;
		// read a byte array
		case 2: 
		len = intValue(arguments[0]);
		returnedObject = newByteArray(len);
		serial_read(charPtr(returnedObject),len);
		break;
		
		case 3:
		serial_write(charPtr(arguments[0]),intValue(arguments[1]));
		break;
#ifdef USE_X_MODEM
		case 4:
		returnedObject = falseobj;
		if(get_file_over_uart())
			returnedObject = trueobj;
		break;
#endif
		case 5: // gpio pinMode
		pinMode(intValue(arguments[0]), intValue(arguments[1]));
		returnedObject = trueobj;
		break;

		case 6: // gpio read
		returnedObject = newInteger(digitalRead(intValue(arguments[0])));
		break;

		case 7: // pio write
		digitalWrite(intValue(arguments[0]), arguments[1] == trueobj ? HIGH:LOW);
		returnedObject = trueobj;
		break;

	    default:
		sysError("unknown primitive", "ioPrimitive");
	}
	 return (returnedObject);
}
#endif

object ioPrimitive(int number, object * arguments)
{
    object returnedObject;
    returnedObject = nilobj;
	if(number <= 9)
		return filePrimitive(number, arguments);
#ifndef IMG_BUILD
	else
		return gpioPrimitive(number-10,arguments);
#endif
}

