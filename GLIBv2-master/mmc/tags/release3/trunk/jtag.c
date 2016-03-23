#include <avr/io.h>  
#include "avrlibtypes.h"
#include "avrlibdefs.h"

#include "jtag.h"
#include "project.h"


#define tdo() (bit_is_set(JTAG_INTF_PIN,PING1)? 1 : 0)


static void jreset();
static void jidle();
static void jsh_ir(u16 instr);
static void jsh_dr(u08 *p,int sz, u08 bitwise);
static void clocking(u08 num);
static void setTMS(u08 val);
static void setTDI(u08 val);
static void bypass_fpga();

static u08  endir = ENDIR_IDLE;
static u08 w_buf[FLASH_WRITE_BYTE_MAX];
static u08  proc_status;
static u32  image_length = 0 ;






void jtag_init()
{

    JTAG_INTF_DDR = 0x1D;
    JTAG_INTF_PORT = 0x10;

	jreset();
	jidle();

	proc_status = JTAG_INIT;



}
void flashwrite(uint8_t *buf,u08 len)
{
   u16 in_reg;
   u08 val[4], i;

   static u08 buf_len = 0;
   

		   CRITICAL_SECTION_START;
    if( proc_status < FLASH_WRITING )
	{
    in_reg = ISC_ENABLE;
    jsh_ir( in_reg );

	val[0] = 0x03;
    jsh_dr(val,1, 1);

	in_reg = XSC_DATA_BTC;
    jsh_ir( in_reg );

    val[0] = 0xff;
	val[1] = 0xff;
	val[2] = 0xff;
    val[3] = 0xe4;
    jsh_dr(val,4, 1);

	in_reg = ISC_PROGRAM;
    jsh_ir( in_reg );

    proc_status = FLASH_WRITING;
	}


	for( i = 0; i < len; i++ )
	{
		w_buf[buf_len++] = buf[i];
		if( buf_len == FLASH_WRITE_BYTE_MAX )
		{

           in_reg = ISC_DATA_SHIFT;
           jsh_ir( in_reg );
		   jsh_dr(w_buf,FLASH_WRITE_BYTE_MAX, 0);
		   if( proc_status < FLASH_ADDRESS_SHIFTED )
		   {
		      in_reg = ISC_ADDRESS_SHIFT;
              jsh_ir( in_reg );
			  val[0] = 0x00;
			  val[1] = 0x00;
			  val[2] = 0x00;
			  jsh_dr(val,3, 1);
			  proc_status = FLASH_ADDRESS_SHIFTED;
			  		   		
		   }
		   if( (image_length+i+1) == 0x00100020)
		   {
		      in_reg = ISC_ADDRESS_SHIFT;
              jsh_ir( in_reg );
			  val[0] = 0x10;
			  val[1] = 0x00;
			  val[2] = 0x00;
			  jsh_dr(val,3, 1);

		   }
		   in_reg = ISC_PROGRAM;
           jsh_ir( in_reg );
		   buf_len = 0;
          
		}
	}

  image_length += len;
CRITICAL_SECTION_END;
	
}

u08 flashfinish(uint8_t *uploaded_size)
{
   u16 in_reg;
   u08 val[3];
 
	

	in_reg = XSC_DATA_DONE;
    jsh_ir( in_reg );

	val[0] = 0xc0;
    jsh_dr(val,1,1);

    in_reg = ISC_PROGRAM;
    jsh_ir( in_reg );

    in_reg = XSC_OP_STATUS;

    jsh_ir( in_reg );
    val[0] = 0x00;
    jsh_dr(val,1, 1);

	if( val[0] == 0x36 )
	{
		proc_status = FLASH_PROGRAMMED;
	}
    in_reg = ISC_DISABLE;
    jsh_ir( in_reg );
	
	jtag_reset();

 

	if( (uploaded_size[0] == (image_length & 0xFF)) &&
	    (uploaded_size[1] == ((image_length>>8) & 0xFF)) && 
        (uploaded_size[2] == ((image_length>>16) & 0xFF)) &&
		(uploaded_size[3] == ((image_length>>24) & 0xFF)) )
	    return 0;
	else
		return 1;


}

void flasherase()
{
   u16 in_reg;
   u08 val[3];


	//jreset();
    in_reg = ISC_ENABLE;
    jsh_ir( in_reg );

	val[0] = 0x03;
    jsh_dr(val,1, 1);

    endir = ENDIR_PAUSE;

    in_reg = ISC_ERASE;
	jsh_ir( in_reg );

	val[0] = 0x00;
	val[1] = 0x00;
	val[2] = 0x03;

    jsh_dr(val,3, 1);

    endir = ENDIR_IDLE;

	
}

u08 flashprepare()
{
   u16 in_reg;
   u08 val[3];
   u08 runs;

    image_length = 0;

    in_reg = XSC_OP_STATUS;
   
    jsh_ir( in_reg );
    val[0] = 0x00;
	val[1] = 0x00;
	val[2] = 0x00;

    jsh_dr(val,1, 1);


	if( val[0] == 0x36 )
	{
	   proc_status = FLASH_ERASED;
	
       in_reg = ISC_DISABLE;

       jsh_ir( in_reg );
       runs = 0x0050; 
	   do{
          }while( --runs );


        jreset();
		return 1;

	 }
	 else
	 	return 0;	


}

void jtag_reset()
{
	jreset();
	
	JTAG_INTF_PORT = 0x00;	
    proc_status = 0;

}

static void jsh_ir( u16 inreg )
{
    int i;


    jidle();
    setTMS(1); clocking(2); setTMS(0); clocking(2);
    for(i=0;i<16;i++){ 
		 clocking(1); 
		 setTDI( inreg & (1<<i)); 
		 }
	bypass_fpga();
    if( endir == ENDIR_PAUSE )
	{
	  setTMS(1); 
	  clocking(1); 
	  setTMS(0); 
	  clocking(1);

	}
	else
	{
      setTMS(1); 
	  clocking(2); 
	  setTMS(0); 
	  clocking(1);
	}
    

}

static void jsh_dr(u08 *p,int sz, u08 bitwise){        /* shift of data register */
    int i, bit; 
    u08 tmp;

	if( endir == ENDIR_PAUSE )
	{
		setTMS(1); clocking(3); setTMS(0); clocking(2);
	}
	else
	{
        jidle(); 
        setTMS(1); clocking(1); setTMS(0); clocking(2);
	}
	if( bitwise )
	{
    for(i=sz;i>0;i--){
  		for( bit=0; bit<8; bit++ )
 		{ 
        tmp=tdo(); 
        clocking(1); 
        setTDI(p[i-1] & (1<<bit)); 
        if(tmp)
		  p[i-1] |= (1<<bit);
		else
		  p[i-1] &= ~(1<<bit);
        }
	 }
	 }
	 else
	 {
    for(i=0;i<sz;i++){
  		for( bit=0; bit<8; bit++ )
 		{ 
        tmp=tdo(); 
        clocking(1); 
        setTDI(p[i] & (1<<bit)); 
        if(tmp)
		  p[i] |= (1<<bit);
		else
		  p[i] &= ~(1<<bit);
        }
	 }
	 }
 
    clocking(1); 
    setTDI(0); 
	
    setTMS(1); clocking(2); setTMS(0); clocking(1);
}

static void bypass_fpga()
{
    int i;

    for(i=0;i<10;i++){ 
 		 clocking(1); 
		 setTDI(1);
		 }
     
}

static void jreset()
{
    setTMS(1);
    clocking(6);
}

static void jidle()
{
    setTMS(0);
    clocking(1);
}

static void clocking(u08 num)
{
    u08 tick;


    
    for( tick = 0; tick < num; tick++)
    {
        sbi(JTAG_INTF_PORT, JTAG_INTF_TCK);
        cbi(JTAG_INTF_PORT, JTAG_INTF_TCK);
    }

}

static void setTMS(u08 val)
{

    if(val)
     sbi(JTAG_INTF_PORT, JTAG_INTF_TMS);
    else
     cbi(JTAG_INTF_PORT, JTAG_INTF_TMS);
}

static void setTDI(u08 val)
{
    if(val)
     sbi(JTAG_INTF_PORT, JTAG_INTF_TDI);
    else
     cbi(JTAG_INTF_PORT, JTAG_INTF_TDI);

}


