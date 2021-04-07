#ifndef KS0108_H
#define KS0108_H

#define VERSION 2.0

#include "mbed.h"
#include "SystemFont5x7.h"
#include "PCF8574_DataBus.h"
#include "menbedDisplay.h"

/************************************************************************************/
// Commands
#define LCD_ON              0x3F
#define LCD_OFF             0x3E
#define LCD_SET_ADD         0x40    // Y adresse 0-63
#define LCD_SET_PAGE        0xB8 //X adresse 0-7
#define LCD_DISP_START      0xC0

//Controller directives
#define LEFT                0
#define RIGHT               1
#define BOTH                3
#define NONE                4

// Colors
#define BLACK               0xFF
#define WHITE               0x00

//Screen dimensions
#define SCREEN_HEIGHT    64
#define SCREEN_WIDTH    128
#define CHIP_WIDTH         64

/***********************************************************************************/
//helper functions 

#define absDiff(x,y) ((x>y) ?  (x-y) : (y-x))
#define swap(a,b) \
do\
{\
uint8_t t;\
    t=a;\
    a=b;\
    b=t;\
} while(0)    

/**************************************************************************************/

// Font Indices
#define FONT_LENGTH         0
#define FONT_FIXED_WIDTH    2
#define FONT_HEIGHT         3
#define FONT_FIRST_CHAR     4
#define FONT_CHAR_COUNT     5
#define FONT_WIDTH_TABLE    6


/*************************Callback function definietion for fonts *********************/
typedef unsigned int (*FontCallback)(unsigned int*);

/*************************Callback function    for reading font array*********************/
static unsigned int ReadData(unsigned int* ptr) {  
    return *ptr;
}

/*************************************************************************************/
#define MAX_IMG_SIZE 128*64

typedef struct {      
     unsigned int imgWidth;
    unsigned int imgHeight;         
    unsigned char imgarray[MAX_IMG_SIZE];
}Image;


typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int page;
} LCDCoord;


/****************************************************************************************/



class KS0108  : public MenbedDisplay {
public:

   /**
    *@brief Constructor, initializes the lcd on the respective pins.
    *@param control pins     RST,DI,RW,E,CS2,CS1
    *@param databus        DB0-DB7    data pins
    *@return none
    */

KS0108 (PinName _RST,PinName _DI, PinName _RW, PinName _E, PinName _CS2, PinName _CS1,PCF8574_DataBus &_DB);
   /**
    *@brief Write instruction to the specific controller.
    *@param Command     command to send to the controller
    *@param side         controller side can be LEFT or RIGHT
    *@return none
    *
    */
    void  WriteInstruction(unsigned int Command,unsigned int side);

    /**
     *@brief Write data byte to the controller.
     *@param data     data send to the controller chip
     *@param side     selected controller can be LEFT or RIGHT
     *@return none
     *
     */
    void  WriteData(unsigned int data ,unsigned char side);


    /**
     *@brief Write data byte to the controller (overloaded function).
     *
       *@param data     data send to the controller chip
     *@return none
     */
    void  WriteData(unsigned int data);

    /**
     *@brief Write data byte to the screen on specific page and column
     *@param page     page varies from 0-7 for each side
     *@param col     col varies from 0-64 for each side
     *@param data     info to be written on given coordinates
     *@return none
     *
     */
    void  WriteDataColPag(unsigned int page, unsigned int col,  unsigned int data);

    /**
     *@brief Read data from display
     *@param none
     *@return none
     *
     */
    unsigned int ReadData();

    /**
     *@brief Read status of display , and check if it's busy
     *@param none
     *@return status     status of display
     *
     */
    unsigned int ReadStatus();

    /**
     *@brief Select controller chip
     *
     *@param side     controller side can be LEFT or RIGHT
     *@return none
     *
     */
    void SelectSide(unsigned char side);

    
       /**
     *@brief Set cursor to specified coordinates
     *
     *@param  x     row
     *@param  y     column
     *@return none
     */
    void GotoXY(unsigned int x, unsigned int y); 


    /**
     *@brief Clears display
     *
     *@param none
     *@return none
     *
     */
    void ClearScreen();
    
    
    /**
     *@brief Turn on display
     *
     *@param none
     *@return none
     *
     */
    void TurnOn();
    
    
    /**
     *@brief Turn Off display
     *
     *@param none
     *@return none
     *
     */
    void TurnOff();

    /*******************************Graphic functions************************************************/

    /**
    *@brief Set pixel to specific location on the screen.
    *@param x coordinate varies from 0-128
    *@param y col varies from 0-64
    *@param color color of pixel, can be BLACK or WHITE
    *@return none
    *
    */
    void SetPixel( unsigned int x,  unsigned int y,  unsigned int color);


    /**
     *@brief Draws a line from x1,y1 to x2,y1
     *@param Xaxis1   x coordinate of one side
     *@param Xaxis2   x coordinate of one side
     *@param Yaxis   y coordinate both points
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
     void HLine(unsigned int Xaxis1, unsigned int Xaxis2 ,unsigned int Yaxis,unsigned int color);

    /**
     *@brief Draw a horizontal line
     *@param Xaxis1
     *@param Xaxis2
     *@param width
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void HLineShort(unsigned int Xaxis, unsigned int Yaxis,unsigned int width ,unsigned int color);

    /**
     *@brief Draws a vertical line
     *@param Xaxis
     *@param Yaxis1
     *@param Yaxis2
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void VLine(unsigned int Xaxis, unsigned int Yaxis1 ,unsigned int Yaxis2,unsigned int color);

    /**
     *@brief Draw a vertical line of a given width starting from X, Y 
     *@param Xaxis
     *@param Yaxis
     *@param height    Height of line
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void VLineShort(unsigned int Xaxis,unsigned int Yaxis, unsigned int height ,unsigned int color);


    /**
     *@brief Draws a line from x1,y1 to x2,y2.
     *@param x1   x coordinate of one side
     *@param y1   y coordinate of one side
     *@param x2   x coordinate of other side
     *@param y2   y coordinate of other side
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void Line(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2, unsigned int color);


    /**
     *@brief Draws a slanty line from x1,y1  to x2,y2
     *@param lX1   x coordinate of one side
     *@param lY1   y coordinate of one side
     *@param lX2   x coordinate of other side
     *@param lY2   y coordinate of other side
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void SlantyLine(unsigned int lX1, unsigned int lY1, unsigned int lX2,unsigned int lY2,unsigned int color);

    /**
     *@brief Draws a line from x,y at given degree from inner_radius to outer_radius.
     *@param x
     *@param y
     *@param inner_radius
     *@param outer_radius
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void DegreeLine(unsigned int x, int y,unsigned int degree,unsigned int inner_radius,unsigned int outer_radius, unsigned int color);

    /**
     *@brief Draw a filled reactangle
     *
     *@param Xaxis1
     *@param Yaxis1
     *@param Xaxis2
     *@param Yaxis2
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void FullRectangle(unsigned int Xaxis1, unsigned int Yaxis1, unsigned int Xaxis2 ,unsigned int Yaxis2,unsigned int color);

    /**
     *@brief Draw an empty rectangle
     *@param Xaxis1
     *@param Yaxis1
     *@param Xaxis2
     *@param Yaxis2
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void EmptyRectangle(unsigned int Xaxis1,unsigned int Yaxis1, unsigned int Xaxis2,unsigned int Yaxis2,unsigned int color);


    /**
     *@brief Draw a rectangle with round corners
     *@param Xaxis1 x-coordinate of the top left point
     *@param Yaxis1 y-coordinate of the top left point
     *@param width  rectangle width
     *@param height rectangle height
     *@param radius radius of the edges
     *@param color can be BLACK or WHITE
     *@return none
     *
     */
    void RoundRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int radius, unsigned int color);

      
    /**
     *Draws a triangle .
     *@param 
     *@param 
     *@param 
     *@param 
     *@return none
     *
     */
    
    void Triangle ( int topx, int topy, int rightx, int righty);
    

    /**
     *Draws a right angle triangle .
     *@param 
     *@param 
     *@param 
     *@param 
     *@return none
     *
     */
    void RightTriangle ( int topx, int topy, int rightx, int righty);


    /**
     *Draws an empty circle centered a x,y with radius R and specific color.
     *@param CenterX   center x coordinate 
     *@param CenterY   center y coordinate
     *@param Radius    circle radius
     *@param color     Color can be BLACK or WHITE
     *@return none
     *
     */
    void EmptyCircle(unsigned int CenterX, unsigned int CenterY, unsigned int Radius,unsigned int color);

    /**
     * Circle fill Code is merely a modification of the midpoint
     * circle algorithem which is an adaption of Bresenham's line algorithm
     * http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
     * http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
     * Adapted from arduino lib
     *
     *@param  CenterX center x coordinate
     *@param CenterY  center y coordinate
     *@param Radius   circle radius
     *@param color    Color can be BLACK or WHITE
     */
    void FullCircle(unsigned int CenterX, unsigned int CenterY, unsigned int Radius,unsigned int color);

    /**
     *Draws an ellipse.
     *@param CX   x coordinate of one side
     *@param CY   y coordinate of one side
     *@param XRadius   x coordinate of other side
     *@param YRadius   y coordinate of other side
     *@param color can be BLACK or WHITE
     *@return none
     *
     * Ported the algorithm found at http://homepage.smc.edu/kennedy_john/belipse.pdf
     *
     */
    void PlotEllipse(long CX, long  CY, long XRadius,long YRadius, int color);
    void Plot4EllipsePoints(long CX,long  CY, long X, long Y, int color);

    
    /**
     *@brief Round a double
     *@param double
     *@return value
     *
     */
    double dfloor( double value );


    /*****************************Bitmaps *****************************************************************/
    
    /**
     *@brief Draws an image on screen.
     *@param PictureData  128x64 image array
     *@return none
     *
     *
     */
    void FullScreenBMP (unsigned char *ImageData);

    /**
     *@brief Draw a 1-bit bitmap
      *
     *@param  image struct containing img size and array
     *@param x x-coordinate
     *@param y y-coordinate
     *@param color can be BLACK or WHITE
     *@return none
     */
    void DrawBitmap(const unsigned int * bitmap, unsigned int x, unsigned int y, unsigned int color);

    /**
     *@brief Static function , mplemented to read an array
     *@param  ptr     data array
     *@return none
     */
    unsigned int ReadArrayData(const unsigned int* ptr); 

   
    /*************************************Font functions **************************************/

   /**
    *@brief Print a character on specified coordinates
    *
    *@param  page     row
    *@param  col     column
    *@param  c     integer value
    *@return none 
    */
    void Putchar (int page, int col,unsigned char c);

   /**
    *@brief Print a string on specified coordinates
    *
    *@param  str     char array
    *@param  x     row
    *@param  y     column 
    *@return none 
    */
    void PutString(unsigned int x, unsigned int y,char* str);

   /**
    *@brief Print a float on specified coordinates
    *
    *@param  val     float value
    *@param  x     row
    *@param  y     column
    *@return none 
    */
    void PrintFloat(float val, unsigned int x,unsigned int y);

   /**
    *@brief Print an integer on specified coordinates
    *
    *@param  val     integer value
    *@param  x     row
    *@param  y     column
    *@return none 
    */ 
    void PrintInteger(int val,unsigned int x,unsigned int y);


   /**
    *@brief Select a specific font
    *
    *@param  font          font array
    *@param  color         font color , can be BLACK or WHITE
    *@param  callback    function pointer to load font
    *@return none 
    */
    void SelectFont(unsigned int* font,unsigned int color, FontCallback callback);

    
    /**
     *@brief Print a character
     *
     *@param  c     char
     *@return none
     */
    int PrintChar(char c);

    
    /**
     *@brief Print a character string 
     *
     *@param  str     char string
     *@return none
     */
    void PrintString(char* str);
    
    
    /**
     *@brief Print a number 
     *
     *@param  n     number
     *@return none
     */
    void PrintNumber(long n);      
    
      /**
     *@brief print *char
     *
     *@param  
     *@return 1
     */
    virtual bool writeLine (const char *line, uint8_t row);
    virtual void showUpArrow (bool show);
    virtual void showDownArrow (bool show);
    virtual uint8_t getLines (void);
    virtual uint8_t getLineLength (void);                          
    
private:
    //BusInOut DB;
    PCF8574_DataBus &DB;
    DigitalInOut RST; // hardware reset
    PinName SDA;
    PinName SCL;
    
    DigitalInOut DI;
    DigitalInOut RW;
    DigitalInOut E;
    DigitalInOut CS2;
    DigitalInOut CS1;
    
    
    bool    Inverted;
    
    LCDCoord                Coord;
    FontCallback            FontRead ;
    unsigned int            FontColor;
    unsigned int*            Font;
    unsigned int color;

   
};


#endif
