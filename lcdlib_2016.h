//  LCD Library Header File                 
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


/*  Function prototypes      */

void lcd_delay ( unsigned int size ); 

char lcd_start ( void ) ;

char lcd_clear ( void ) ;

char lcd_print_ch ( char ch ) ;

char lcd_print ( const char * message ) ;

char lcd_cursor ( char x, char y ) ;

char lcd_cursor_on ( void ) ;

char lcd_cursor_off ( void ) ;

char lcd_scroll_on ( void ) ;

char lcd_scroll_off ( void ) ;

void lcd_display_value ( unsigned char value );
void lcd_display_int_value ( unsigned int value ) ;

void lcd_command ( unsigned char in );

                       