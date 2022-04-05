    //Displaying "HELLO" on LCD for Dragon12+ Trainer Board 
//with HCS12 Serial Monitor Program installed. This code is for CodeWarrior IDE
//Modified from Mazidi's book with contribution from Travis Chandler
//On Dragon12+ LCD data pins of D7-D4 are connected to Pk5-Pk2, En=Pk1,and RS=Pk0,

#include <time.h> 
#include <string.h>
#include <stdlib.h>
#include <hidef.h>      /* common defines and macros */


#include "mc9s12dg256.h"      /* derivative-specific definitions */

  
#define LCD_DATA PORTK
#define LCD_CTRL PORTK
#define RS 0x01
#define EN 0x02

int word_count;
char words[50][10];
char chosen_word[10];
char searched_word[10];
char current_letter = 'A';
char appereances[26];

void create_words(){
  strcpy(words[0],"ARIPA");
  strcpy(words[1],"JOC");
  strcpy(words[2],"BOLOBOC");
  strcpy(words[3],"FOTBAL");
  strcpy(words[4],"COJOC");
  strcpy(words[5],"CAINE");
  strcpy(words[6],"KEBAB");
  word_count = 7;
}
  
  

void COMWRT4(unsigned char);
void DATWRT4(unsigned char);
void MSDelay(unsigned int);
void Choose_word (){
  int random,random_seed;
  srand (254);     //// modificat seed-ul in functie de cat timp tinem apasat un buton
  random = rand() % word_count;
  strcpy(chosen_word,words[random]);
}

void parse_word (char a[]){
    int i,n;
    n = strlen(a);
    for ( i = 0; i < n ; i++ ){
      DATWRT4(a[i]);
      MSDelay(1); 
    }
}

void reset (char a[]){
  strcpy(a,"\0"); 
}

void init_searched_word (char a[]){
  
  int i,n;
  
  n = strlen(a);
  for( i = 0 ; i < n; i++){
    searched_word[i] = '_';
  }
}

int number_of_lives = 6;

void showlives(){
    int n = 10,i;
    PTP = 0xFE;
    MSDelay(100);
    switch(number_of_lives){
      case 6:
        PORTB = 0x7D;
        MSDelay(100);
        break;
      case 5:
        PORTB = 0x6D;
        MSDelay(100);
        break;
      case 4:
        PORTB = 0x66;
        MSDelay(100);
        break;
      case 3:
        PORTB = 0x4F;
        MSDelay(100);
        break;
      case 2:
        PORTB = 0x5B;
        MSDelay(100);
        break;
      case 1:
        PORTB = 0x06;
        MSDelay(100);
        break;
      case 0:
        PORTB = 0x3F;
        MSDelay(1000);
        COMWRT4(0x01);
        MSDelay(1);
        parse_word("YOU LOSE!");
        while(n){
            for(i = 0 ; i< 30 ; i++){
              PTP = 0xFE;
              PORTB = 0xFF;
              MSDelay(5);
              PORTB = 0xFF;
              PTP = 0xFD;
              PORTB = 0xFF;
              MSDelay(5);
              PTP = 0xFB;
              PORTB = 0xFF;
              MSDelay(5);
              PTP = 0xF7;
              PORTB = 0xFF;
              MSDelay(5);
            }
            n--;
        }
        exit(0);
        break;
      default: 
        PORTB = 0xFF;
        MSDelay(100);
        break;
    }
}


void testbutton()
{
    int i,n,count = 0;
    n = strlen(searched_word);
    DDRH = 0x00;
    showlives();
    
    /*
    if((PTH || 0xFD) == 0) showlives();
    if((PTH || 0xFB) == 0) DATWRT4('2');
    if((PTH || 0xF7) == 0) DATWRT4('3');
    */
    if (PTH != 0x0E){       //// adaugat debouncing - 
      if (PTH == 0x0C) {
        if(current_letter!= 'Z'){
          current_letter++;
        } else{
          current_letter = 'A';
        }
        COMWRT4(0x01);  //Clear display
        MSDelay(1);
        parse_word(searched_word);
        DATWRT4(' ');
        DATWRT4(current_letter);
      }
      if (PTH == 0x0A) {
         count = 0;
         if (appereances[current_letter - 'A'] == 1){
            number_of_lives--;
            showlives();
         } 
         else {
             for(i = 0 ; i < n ; i++) {
                if (current_letter == chosen_word[i]){
                  searched_word[i] = current_letter;
                  appereances[current_letter - 'A'] = 1;
                  count++;
                }
            }
              if( strcmp ( searched_word,chosen_word) == 0){
                COMWRT4(0x01);
                MSDelay(1);
                parse_word(chosen_word);
                MSDelay(2000);
                COMWRT4(0x01);
                MSDelay(1);
                parse_word("YOU WON!");
                MSDelay(2000);
                exit(0);
              }
          
              if( count == 0){
                number_of_lives--;
                showlives();
              }
         }
         
          COMWRT4(0x01);
          MSDelay(1);
          parse_word(searched_word);
          DATWRT4(' ');
          DATWRT4(current_letter);
        }
      if (PTH == 0x06) {
        if(current_letter!= 'A'){
          current_letter--;
        } else{
          current_letter = 'Z';
        }
        COMWRT4(0x01);  //Clear display
        MSDelay(1);
        parse_word(searched_word);
        DATWRT4(' ');
        DATWRT4(current_letter);
      }
      
    }
    
    MSDelay(10);
}

void main(void) 
{
        int i;
        DDRB = 0xFF;    //PORTB as output since LEDs are connected to it
        DDRJ = 0xFF;    //PTJ as output to control Dragon12+ LEDs
        DDRP = 0xFF;
        DDRA = 0xFF;
        PTJ=0x0; 

        DDRK = 0xFF;   
        COMWRT4(0x33);   //reset sequence provided by data sheet
        MSDelay(1);
        COMWRT4(0x32);   //reset sequence provided by data sheet
        MSDelay(1);
        COMWRT4(0x28);   //Function set to four bit data length
                                         //2 line, 5 x 7 dot format
        MSDelay(1);
        COMWRT4(0x06);  //entry mode set, increment, no shift
        MSDelay(1);
        COMWRT4(0x0E);  //Display set, disp on, cursor on, blink off
        MSDelay(1);
        COMWRT4(0x01);  //Clear display
        MSDelay(1);
        COMWRT4(0x80);  //set start posistion, home position
        MSDelay(1);
        
        create_words();
        Choose_word();
        init_searched_word(chosen_word);
          
        parse_word(searched_word);
        DATWRT4(' ');
        DATWRT4(current_letter);
        showlives();
        for(;;)testbutton();
        
	
}
void COMWRT4(unsigned char command)
  {
        unsigned char x;
        
        x = (command & 0xF0) >> 2;         //shift high nibble to center of byte for Pk5-Pk2
      LCD_DATA =LCD_DATA & ~0x3C;          //clear bits Pk5-Pk2
        LCD_DATA = LCD_DATA | x;          //sends high nibble to PORTK
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~RS;         //set RS to command (RS=0)
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //Drop enable to capture command
        MSDelay(15);                       //wait
        x = (command & 0x0F)<< 2;          // shift low nibble to center of byte for Pk5-Pk2
        LCD_DATA =LCD_DATA & ~0x3C;         //clear bits Pk5-Pk2
        LCD_DATA =LCD_DATA | x;             //send low nibble to PORTK
        LCD_CTRL = LCD_CTRL | EN;          //rais enable
        MSDelay(5);
        LCD_CTRL = LCD_CTRL & ~EN;         //drop enable to capture command
        MSDelay(15);
  }

         void DATWRT4(unsigned char data)
  {
  unsigned char x;
       
        
        
        x = (data & 0xF0) >> 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | RS;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(5);
       
        x = (data & 0x0F)<< 2;
        LCD_DATA =LCD_DATA & ~0x3C;                     
        LCD_DATA = LCD_DATA | x;
        LCD_CTRL = LCD_CTRL | EN;
        MSDelay(1);
        LCD_CTRL = LCD_CTRL & ~EN;
        MSDelay(15);
  }


 void MSDelay(unsigned int itime)
  {
    unsigned int i; unsigned int j;
    for(i=0;i<itime;i++)
      for(j=0;j<4000;j++);
 }