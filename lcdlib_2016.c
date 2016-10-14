// LCD Library Header File                 
//  Thomas Gartlan, DkIT, 2016              

//  2 line LCD Display                      
//  Standard Hitachi 44780 controller       

//  Pin assigments:             
//  Operates in Nibble mode so data bus is 
//  4 bits
//  PORT:0  ----->  LCD bit 4           
//  PORT:1  ----->  LCD bit 5           
//  PORT:2  ----->  LCD bit 6           
//  PORT:3  ----->  LCD bit 7           
//  PORT:4  ----->  LCD RS              
//  PORT:5  ----->  LCD E               

//Created for the XC8 compiler from Microchip

#include <xc.h>
#define _XTAL_FREQ 19660800
#include "../LCD_library/lcdlib_2016.h"


#define RS_Data 0x01
#define RS_Instr 0x00
#define EBIT 0x05

/*  defined values for delays            */
/*  tested up to 20Mhz PIC               */

/*  standard write delay                 */

/*  standard write delay                 */

//#define PUTCH_DELAY 250
#define PUTCH_DELAY __delay_us(50);

/*  clear and cursor home take longer    */
/*  special delay for them               */

//#define CLEAR_DELAY 5000


#define CLEAR_DELAY __delay_ms(2);

/*  Power up delay to let the LCD settle */

//#define POWER_UP_DELAY 10000
#define POWER_UP_DELAY __delay_ms(35);

/*  Bit delay to let the ports settle    */
#define BIT_DELAY __delay_us(2);
//#define BIT_DELAY 2


// Using generic names for ports.
// Actual port used should be tied to the generic names here
#define lcd_port LATDbits
#define lcd_port_dir TRISD


//#define E_bit_low lcd_port = lcd_port & 0xDF;  // clear E bit (bit 5)
//#define E_bit_high lcd_port = lcd_port | 0x20; // set E bit
#define E_bit_low lcd_port.LD5 = 0;
#define E_bit_high lcd_port.LD5 = 1;



/*  sends a byte out to the LCD    */
/*  the byte is given by 'in'. The   */
/*  mask is used to allow the      */
/*  state of RS to be set as well  */

void lcd_raw_send ( unsigned char in, 
                    unsigned char RS )
{
    
    lcd_port.LD0 = in >> 4;
    lcd_port.LD1 = in >> 5;
    lcd_port.LD2 = in >> 6;
    lcd_port.LD3 = in >> 7;
    lcd_port.LD4 = RS;   //mask must be 1 or 0
    
    BIT_DELAY
	
   //  Clock the bit out     
   // By raising and lowering E 
 
    E_bit_high  
    
    BIT_DELAY
    
    E_bit_low
    
    PUTCH_DELAY

    //now send the low nibble    
    lcd_port.LD0 = in;
    lcd_port.LD1 = in >> 1;
    lcd_port.LD2 = in >> 2;
    lcd_port.LD3 = in >> 3;
    
    BIT_DELAY
    
    E_bit_high
    
    BIT_DELAY
    
    E_bit_low
    
    PUTCH_DELAY
}    

//  Puts out a character at the cursor 
char lcd_print_ch ( unsigned char in )
{
    /* use raw send with RS set    */
    lcd_raw_send ( in, RS_Data ) ;
    return 1 ;
}    
 
//  Sends a command to the LCD    
void lcd_command ( unsigned char in )
{
    lcd_raw_send ( in, RS_Instr ) ;
}
 
//  clear the display             
// and home the cursor            

char lcd_clear ( void )
{
	// clear command
    lcd_command ( 0x01 ) ;
    CLEAR_DELAY
	
    // return home comand
    lcd_command ( 0x02 ) ;
    CLEAR_DELAY
    return 1 ;
}

//  position the cursor          
char lcd_cursor ( unsigned char x,
                  unsigned char y )
{
    
    // note in each case msb is 1 so command is to set
    // the DDRAM address.
    // The lower 7 bits is then the address
    // 000_0000(0x00) to 000_1111(0x0f) for top line
    // 100_0000(0x40) to 100_1111(0x4f) for top line
    if ( y==0 ) 
    {
        /* position for line 0     */
        y=0x80 ;
    }
    else 
    {
        /* position for line 1     */
        y=0xc0 ;
    }
    
    lcd_command ( y+x ) ;
    return 1 ;
}
    
const unsigned char lcd_init [5] = 
{
/*  LCD initialise                 */    
    0x33,  //removing this so I don't think its needed T.G(is needed!!!)
/*  Set for 4 bit operation        */    
    0x32,	// removing this so I don't think its needed T.G(is needed!!!!)
    
//  'Function Set' command
//  4-bit mode,2 line display, 5*8 dots    
/*  Set for 2 line LCD             */
    0x2c,
    
/*  Select move after write        */
//  See LCD 'Entry Mode Set' command     
    0x06,
/*  disp. on cursor off blink off  */ 
//  See LCD 'Display Control' command     
    0x0c
} ;     
    
    
char lcd_start (void) 
{
    unsigned char i, t ;
    
    /* set bits of PORT for output    */
    /* change for different hardware   */
    /* clear bottom five bits for LCD  */
    /* don't change any other bits     */
    lcd_port_dir = (lcd_port_dir & 0xc0) ;
    
    // Give the LCD time to settle     
    // From power up                   
    
    POWER_UP_DELAY        
    for ( i=0 ; i < 5 ; i++ )   
        lcd_command ( lcd_init [i] ) ;
    }
    lcd_clear () ;
    return 1 ;
}

/*  lcd_print accepts a pointer    */
/*  parameter which points at the  */
/*  string to be printed           */

char lcd_print ( const unsigned char * mess ) 
{
    unsigned char i = 0 ;

    /*  treat the pointer as an    */
    /*  array and send bytes until */
    /*  we find an element with 0  */
    /*  in it                      */
    
    while ( mess [i] != 0 ) 
    {
        lcd_print_ch ( mess[i] ) ;
        i++ ;
    }
    return 1 ;
}    

char lcd_cursor_on ( void ) 
{
    lcd_command ( 0x0f ) ;
    return 1 ;
}

char lcd_cursor_off ( void ) 
{
    lcd_command ( 0x0c ) ;
    return 1 ;
}

void lcd_display_value ( unsigned char value ) 
{
    unsigned char hunds, tens, units ;
    
    /*  first get the digits  */
    
    units = value % 10 ;
    value = value / 10 ;
    tens = value % 10 ;
    value = value / 10 ;
    hunds = value % 10 ;
    
    /*  now display them      */
    /* note how the ascii for the value is got */
    /* by using the value as an offset to ascii for '0' */
    
    lcd_print_ch ( '0' + hunds ) ;
    lcd_print_ch ( '0' + tens ) ;
    lcd_print_ch ( '0' + units ) ;
}

void lcd_display_int_value ( unsigned int value ) 
{
    unsigned char tenthous,thous,hunds, tens, units ;
    
    /*  first get the digits  */
    
    units = value % 10 ;
    value = value / 10 ;
    tens = value % 10 ;
    value = value / 10 ;
    hunds = value % 10 ;
    value = value / 10 ;
    thous = value % 10;
    value = value / 10 ;
    tenthous = value % 10;
    
    /*  now display them      */
    /* note how the ascii for the value is got */
    /* by using the value as an offset to ascii for '0' */
    lcd_print_ch ( '0' + tenthous ) ;
    lcd_print_ch ( '0' + thous ) ;
    lcd_print_ch ( '0' + hunds ) ;
    lcd_print_ch ( '0' + tens ) ;
    lcd_print_ch ( '0' + units ) ;
}




