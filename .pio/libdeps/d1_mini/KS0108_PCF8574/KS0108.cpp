#include "KS0108.h"
#include "PCF8574_DataBus.h"

/*KS0108::KS0108 (PinName _RST,PinName _DI, PinName _RW, PinName _E, PinName _CS2, PinName _CS1, PinName DB0, PinName DB1, PinName DB2, PinName DB3, PinName DB4, PinName DB5, PinName DB6, PinName DB7)
: DB(DB0,DB1,DB2,DB3,DB4,DB5,DB6,DB7),RST (_RST),DI(_DI), RW(_RW), E(_E), CS2(_CS2), CS1(_CS1) {*/



KS0108::KS0108 (PinName _RST,PinName _DI, PinName _RW, PinName _E, PinName _CS2, PinName _CS1,PCF8574_DataBus &_DB)
    :
    RST(_RST),
    DI(_DI),
    RW(_RW),
    E(_E),
    CS2(_CS2),
    CS1(_CS1),
    DB(_DB)
{
    RST.mode(OpenDrain);
    DI.mode(OpenDrain);
    RW.mode(OpenDrain);
    E.mode(OpenDrain);
    CS2.mode(OpenDrain);
    CS1.mode(OpenDrain);

    //RST.output();
    DI.output();
    RW.output();
    E.output();
    CS2.output();
    CS1.output();
    DB.output();


    CS1.output();
    CS2.output();
    RST.write(0);
    wait_us(10);
    RST.write(1);                         //reset screen
    E.write(0);
    ClearScreen();                      //clear display
    WriteInstruction(LCD_ON, BOTH);     //turn on lcd
    Inverted = 0;
}



void  KS0108::WriteInstruction(unsigned int Command,unsigned int side)
{
    E.write(0);
    DI.write(0);
    RW.write(0);

    SelectSide(side);   //select controller

    wait(0.0000003);     //wait 300ns
    E.write(1);
    DB.output();
    DB.write(Command);
    wait(0.0000001);
    E.write(0);
}


void  KS0108::WriteData(unsigned int data,unsigned char side)
{
    E.write(0);
    DI.write(1);
    RW.write(0);

    SelectSide(side);

    wait(0.0000003); // 300ns
    E.write(1);
    DB.output();
    DB.write(data);
    wait(0.0000001);
    E.write(0);
}

void KS0108::WriteData(unsigned int data)
{
    unsigned int displayData, yOffset, chip;

    if(Coord.x >= SCREEN_WIDTH)
        return;
    chip = Coord.x/CHIP_WIDTH;
    wait(0.000000450); // 300ns

    if(Coord.x % CHIP_WIDTH == 0 && chip > 0) {
        GotoXY(Coord.x, Coord.y);
    }

    DI.write(1);                    // D/I = 1
    RW.write(0);                      // R/W = 0
    DB.output();                    // data port is output

    yOffset = Coord.y%8;

    if(yOffset != 0) {                 // first page

        displayData = ReadData();

        DI.write(1);                            // D/I = 1
        RW.write(0);                              // R/W = 0
        SelectSide(chip);
        DB.output();
        // data port is output
        displayData |= data << yOffset;
        if(Inverted)    displayData = ~displayData;
        DB.write(displayData);                     // write data
        wait(0.0000003);                         // 300ns
        E.write(1);
        wait(0.0000001);
        E.write(0);

        // second page
        GotoXY(Coord.x, Coord.y+8);

        displayData = ReadData();

        DI.write(1);                            // D/I = 1
        RW.write(0);                              // R/W = 0
        SelectSide(chip);

        DB.output();                // data port is output

        displayData |= data >> (8-yOffset);

        if(Inverted)
            displayData = ~displayData;
        DB.write(displayData);        // write data

        wait(0.0000003);             // 300ns
        E.write(1);
        wait(0.0000001);
        E.write(0);

        GotoXY(Coord.x+1, Coord.y-8);
    } else    {

        // just this code gets executed if the write is on a single page
        if(Inverted)
            data = ~data;
        wait(0.0000003);                 // 300nsEN_DELAY();
        DB.write(data);                    // write data
        wait(0.0000003);                 // 300ns
        E = 1;
        wait(0.0000001);
        E = 0;
        Coord.x++;
    }
}


void KS0108::WriteDataColPag(unsigned int page, unsigned int col,  unsigned int data)
{

    SelectSide(NONE);
    col     = col%SCREEN_WIDTH;
    page    = page%8;

    if(col<(SCREEN_WIDTH/2)) {
        SelectSide(LEFT);
        WriteInstruction(LCD_SET_PAGE|page,LEFT);
        WriteInstruction(LCD_SET_ADD|col,LEFT);          //set page and column position
        WriteData(data,LEFT);                            //output data to D0-D7
    } else {

        SelectSide(RIGHT);
        col -= (SCREEN_WIDTH/2);
        WriteInstruction(LCD_SET_PAGE|page,RIGHT);
        WriteInstruction(LCD_SET_ADD|col,RIGHT);        //set page and column position
        WriteData(data,RIGHT);                          //output data to D0-D7
    }

    SelectSide(NONE);
}



unsigned int KS0108::ReadData()
{
    unsigned int data;
    DB.input();


    DI.write(1);
    RW.write(1);

    E.write(1);
    wait(0.00000045);

    data = DB.read();
    wait(0.0000001);
    E.write(0);
    DB.output();

    return data;
}

unsigned int KS0108::ReadStatus()
{
    unsigned int status;
    DB.input();

    DI.write(0);

    RW.write(1);
    E.write(1);
    wait(0.00000045);

    status = DB.read();
    E.write(0);
    wait(0.0000001);
    DB.output();

    return status;
}



void KS0108::SelectSide(unsigned char side)
{
    if(side==LEFT) {
        CS1.write(1);
        CS2.write(0);
    } else if(side==RIGHT) {
        CS1.write(0);
        CS2.write(1);
    } else if (side==BOTH) {
        CS1.write(1);
        CS2.write(1);
    } else if (side==NONE) {
        CS1.write(0);
        CS2.write(0);
    }
}


void KS0108::ClearScreen()
{
    for (int col=0; col<128; col++) {
        for (int page=0; page<8; page++) {
            WriteDataColPag(page,col,WHITE);
        }
    }
}


void KS0108::TurnOn()
{
    WriteInstruction(LCD_ON,BOTH);
}


void KS0108::TurnOff()
{
    WriteInstruction(LCD_OFF,BOTH);
}


/*******************************************************************************************/


void KS0108::SetPixel(unsigned int x,  unsigned int y,  unsigned int color)
{

    unsigned int position;

    SelectSide(NONE);
    WriteInstruction(LCD_SET_ADD,NONE);

    if(x>=64) {
        WriteInstruction(LCD_SET_PAGE|(y/8),RIGHT);
        WriteInstruction(LCD_SET_ADD|x,RIGHT);
        position = ReadData();                            //dummy read
        position = ReadData();                            //actual read
        WriteInstruction(LCD_SET_ADD|x,RIGHT);
        if(color==WHITE)
            WriteData(position&(~(1<<(y%8))),RIGHT);         // draw a white pixel
        else
            WriteData(position|(1<<(y%8)),RIGHT);
        wait_us(450);
    } else {
        WriteInstruction(LCD_SET_PAGE|(y/8),LEFT);
        WriteInstruction(LCD_SET_ADD|x,LEFT);
        position = ReadData();                            //dummy read
        position = ReadData();                            //actual read
        WriteInstruction(LCD_SET_ADD|x,LEFT);
        if(color==WHITE)
            WriteData(position&(~(1<<(y%8))),LEFT);
        else
            WriteData(position|(1<<(y%8)),LEFT);

        wait_us(450);

    }

}



void KS0108::FullRectangle(unsigned int Xaxis1, unsigned int Yaxis1, unsigned int Xaxis2 ,unsigned int Yaxis2,unsigned int color)
{

    for(unsigned int i=Xaxis1; i<=Xaxis2; i++) {
        for(unsigned int j=Yaxis1; j<=Yaxis2; j++) {
            SetPixel(i,j,color);

        }
    }
}


void KS0108::EmptyRectangle(unsigned int Xaxis1,unsigned int Yaxis1, unsigned int Xaxis2,unsigned int Yaxis2,unsigned int color)
{
    unsigned int CurrentValue;

    /* Draw the two horizontal lines */
    for (CurrentValue = 0; CurrentValue < Xaxis2 - Xaxis1+ 1; CurrentValue++) {
        SetPixel(Xaxis1 + CurrentValue, Yaxis1,color);
        SetPixel(Xaxis1 + CurrentValue, Yaxis2,color);
    }

    /* draw the two vertical lines */
    for (CurrentValue = 0; CurrentValue < Yaxis2 - Yaxis1 + 1; CurrentValue++) {
        SetPixel(Xaxis1, Yaxis1 + CurrentValue,color);
        SetPixel(Xaxis2, Yaxis1 + CurrentValue,color);
    }
}


void KS0108::RoundRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned int radius, unsigned int color)
{
    int tSwitch, x1 = 0, y1 = radius;
    tSwitch = 3 - 2 * radius;

    while (x1 <= y1) {
        SetPixel(x+radius - x1, y+radius - y1, color);
        SetPixel(x+radius - y1, y+radius - x1, color);

        SetPixel(x+width-radius + x1, y+radius - y1, color);
        SetPixel(x+width-radius + y1, y+radius - x1, color);

        SetPixel(x+width-radius + x1, y+height-radius + y1, color);
        SetPixel(x+width-radius + y1, y+height-radius + x1, color);

        SetPixel(x+radius - x1, y+height-radius + y1, color);
        SetPixel(x+radius - y1, y+height-radius + x1, color);

        if (tSwitch < 0) {
            tSwitch += (4 * x1 + 6);
        } else {
            tSwitch += (4 * (x1 - y1) + 10);
            y1--;
        }
        x1++;
    }

    HLineShort(x+radius,y, width-(2*radius), color);                // top
    HLineShort(x+radius,y+height, width-(2*radius),  color);        // bottom
    VLineShort(x,y+radius,height-(2*radius), color);                // left
    VLineShort(x+width, y+radius,height-(2*radius),  color);        // right
}


void KS0108::HLine(unsigned int Xaxis1, unsigned int Xaxis2 ,unsigned int Yaxis,unsigned int color)
{
    FullRectangle(Xaxis1,Yaxis,Xaxis2,Yaxis,color);

}


void KS0108::HLineShort(unsigned int Xaxis, unsigned int Yaxis,unsigned int width ,unsigned int color)
{
    FullRectangle(Xaxis,Yaxis,Xaxis+width,Yaxis,color);

}


void KS0108::VLine(unsigned int Xaxis, unsigned int Yaxis1 ,unsigned int Yaxis2,unsigned int color)
{
    FullRectangle(Xaxis,Yaxis1,Xaxis,Yaxis2,color);
}


void KS0108::VLineShort(unsigned int Xaxis,unsigned int Yaxis, unsigned int height ,unsigned int color)
{
    FullRectangle(Xaxis,Yaxis,Xaxis,Yaxis+height,color);

}


void KS0108::DegreeLine(unsigned int x, int y,unsigned int degree,unsigned int inner_radius,unsigned int outer_radius, unsigned int color)
{
    int fx,fy,tx,ty;
    fx = x + dfloor(inner_radius * sin(degree * 3.14 / 180));
    fy = y - dfloor(inner_radius * cos(degree * 3.14 / 180));
    tx = x + dfloor(outer_radius * sin(degree * 3.14 / 180));
    ty = y - dfloor(outer_radius * cos(degree * 3.14 / 180));
    SlantyLine(fx,fy,tx,ty,color);
}


double KS0108::dfloor( double value )
{

    if (value < 0.0)
        return ceil( value );
    else
        return floor( value );

}


void KS0108::SlantyLine(unsigned int lX1, unsigned int lY1, unsigned int lX2,unsigned int lY2,unsigned int color)
{
    long lError, lDeltaX, lDeltaY, lYStep, bSteep;

    // A steep line has a bigger ordinate.

    if(((lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2)) > ((lX2 > lX1) ? (lX2 - lX1) : (lX1 - lX2))) {
        bSteep = 1;
    } else {
        bSteep = 0;
    }

    // If line is steep, swap the X and Y coordinates.
    if(bSteep) {
        lError = lX1;
        lX1 = lY1;
        lY1 = lError;
        lError = lX2;
        lX2 = lY2;
        lY2 = lError;
    }


    // If the starting X coordinate is larger than the ending X coordinate,
    // swap coordinates.
    if(lX1 > lX2) {
        lError = lX1;
        lX1 = lX2;
        lX2 = lError;
        lError = lY1;
        lY1 = lY2;
        lY2 = lError;
    }

    // Compute the difference between the start and end coordinates.
    lDeltaX = lX2 - lX1;
    lDeltaY = (lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2);

    lError = -lDeltaX / 2;          // Initialize the error term to negative half the X delta.

    if(lY1 < lY2) {                  // Determine the direction to step in the Y axis when required.
        lYStep = 1;
    } else {
        lYStep = -1;
    }

    for(; lX1 <= lX2; lX1++) {   // Loop through all the points along the X axis of the line.

        // See if this is a steep line.

        if(bSteep) {

            // Plot this point of the line, swapping the X and Y coordinates.

            SetPixel(lY1, lX1,color);
        } else {         // Plot this point of the line, using the coordinates as is.
            SetPixel(lX1, lY1,color);
        }

        // Increment the error term by the Y delta.

        lError += lDeltaY;

        if(lError > 0) {               // See if the error term is now greater than zero.

            lY1 += lYStep;            // Take a step in the Y axis.

            lError -= lDeltaX;         // Decrement the error term by the X delta.
        }
    }
}



void KS0108::Line(unsigned int x1, unsigned int  y1, unsigned int  x2, unsigned int  y2, unsigned int color)
{
    unsigned int  deltax, deltay, x,y, steep;
    int lerror, ystep;

    steep = absDiff(y1,y2) > absDiff(x1,x2);   //check slope

    if ( steep ) {
        swap(x1, y1);
        swap(x2, y2);
    }

    if (x1 > x2) {
        swap(x1, x2);
        swap(y1, y2);
    }

    deltax = x2 - x1;
    deltay = absDiff(y2,y1);
    lerror = deltax / 2;
    y = y1;
    if(y1 < y2) ystep = 1;
    else ystep = -1;

    for(x = x1; x <= x2; x++) {
        if (steep) SetPixel(y,x, color);
        else SetPixel(x,y, color);
        lerror -= deltay;
        if (lerror < 0) {
            y = y + ystep;
            lerror += deltax;
        }
    }
}


void KS0108::EmptyCircle(unsigned int CenterX, unsigned int CenterY, unsigned int Radius,unsigned int color)
{
    unsigned int y=0, x=0, d = 0;
    int part;
    d = CenterY - CenterX;
    y = Radius;
    part = 3 - 2 * Radius;

    while (x <= y) {
        SetPixel(CenterX + x, CenterY + y,color);
        SetPixel(CenterX + x, CenterY - y,color);
        SetPixel(CenterX - x, CenterY + y,color);
        SetPixel(CenterX - x, CenterY - y,color);
        SetPixel(CenterY + y - d, CenterY + x,color);
        SetPixel(CenterY + y - d, CenterY - x,color);
        SetPixel(CenterY - y - d, CenterY + x,color);
        SetPixel(CenterY - y - d, CenterY - x,color);

        if (part < 0) part += (4 * x + 6);
        else {
            part += (4 * (x - y) + 10);
            y--;
        }
        x++;
    }

}


void KS0108::FullCircle(unsigned int CenterX, unsigned int CenterY, unsigned int Radius,unsigned int color)
{

    int f = 1 - Radius;
    int ddF_x = 1;
    int ddF_y = 2 * Radius;             //changed sign of -2
    unsigned int x = 0;
    unsigned int y = Radius;

    //Fill in the center between the two halves

    Line(CenterX, CenterY-Radius, CenterX, CenterY+Radius, color);

    while(x < y) {
        if(f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        /*
         * Now draw vertical lines between the points on the circle rather than
         * draw the points of the circle. This draws lines between the
         * perimeter points on the upper and lower quadrants of the 2 halves of the circle.
         */

        Line(CenterX+x, CenterY+y, CenterX+x, CenterY-y, color);
        Line(CenterX-x, CenterY+y, CenterX-x, CenterY-y, color);
        Line(CenterX+y, CenterY+x, y+CenterX, CenterY-x, color);
        Line(CenterX-y, CenterY+x, CenterX-y, CenterY-x, color);
    }
}



void KS0108::PlotEllipse(long CX, long  CY, long XRadius,long YRadius, int color)
{


    long X, Y;
    long XChange, YChange;
    long EllipseError;
    long TwoASquare,TwoBSquare;
    long StoppingX, StoppingY;
    TwoASquare = 2*XRadius*XRadius;
    TwoBSquare = 2*YRadius*YRadius;
    X = XRadius;
    Y = 0;
    XChange = YRadius*YRadius*(1-2*XRadius);
    YChange = XRadius*XRadius;
    EllipseError = 0;
    StoppingX = TwoBSquare*XRadius;
    StoppingY = 0;

    while ( StoppingX >=StoppingY ) {               //first set of points,y'>-1
        Plot4EllipsePoints(CX,CY,X,Y,color);
        Y++;
        StoppingY=StoppingY+ TwoASquare;
        EllipseError = EllipseError+ YChange;
        YChange=YChange+TwoASquare;
        if ((2*EllipseError + XChange) > 0 ) {
            X--;
            StoppingX=StoppingX- TwoBSquare;
            EllipseError=EllipseError+ XChange;
            XChange=XChange+TwoBSquare;
        }
    }

    Y = YRadius;
    X = 0;
    YChange = XRadius*XRadius*(1-2*YRadius);
    XChange = YRadius*YRadius;
    EllipseError = 0;
    StoppingY = TwoASquare*YRadius;
    StoppingX = 0;

    while ( StoppingY >=StoppingX ) {               //{2nd set of points, y'< -1}
        Plot4EllipsePoints(CX,CY,X,Y,color);
        X++;
        StoppingX=StoppingX + TwoBSquare;
        EllipseError=EllipseError+ XChange;
        XChange=XChange+TwoBSquare;
        if ((2*EllipseError + YChange) > 0 ) {
            Y--;
            StoppingY=StoppingY- TwoASquare;
            EllipseError=EllipseError+ YChange;
            YChange=YChange+TwoASquare;
        }
    }
}



void KS0108::Plot4EllipsePoints(long CX,long  CY, long X, long Y, int color)
{
    SetPixel(CX+X, CY+Y, color); //{point in quadrant 1}
    SetPixel(CX-X, CY+Y, color); //{point in quadrant 2}
    SetPixel(CX-X, CY-Y, color); //{point in quadrant 3}
    SetPixel(CX+X, CY-Y, color); //{point in quadrant 4}
}


void KS0108::RightTriangle ( int topx, int topy, int rightx, int righty)
{

    //draw rectangle one line at a time
    Line( topx,topy, rightx,righty,BLACK );        //draw hypotenuse
    Line ( topx,righty,topx,topy,BLACK);         //draw perpendicular
    Line (topx,righty, rightx,righty,BLACK);      // draw base

}

void KS0108::Triangle ( int topx, int topy, int rightx, int righty)
{
    int base =0;
    base = 2* rightx-topx;
    //draw rectangle one line at a time
    Line( topx,topy, rightx,righty,BLACK );                //draw hypotenuse
    Line ( topx,righty,topx,topy,BLACK);                     //draw perpendicular
    Line(topx-base/2,righty, rightx,righty,BLACK);         // draw base
    Line(topx-base/2, righty, topx,topy,BLACK);            // draw hypotenuse

}




/***********************************************************************************/


void KS0108::FullScreenBMP (unsigned char *PictureData)
{
    unsigned int Page=0;
    unsigned int Column=0;

    // Draw left side of the picture
    SelectSide(LEFT);
    for (Page = 0; Page < 8; Page++) {                    /* loop on the 8 pages */
        WriteInstruction(LCD_SET_PAGE | Page,LEFT); /* Set the page */
        for (Column = 0; Column < 64; Column++)
            WriteData(PictureData[(128*Page)+Column],LEFT);
    }

    // Draw right side of the picture
    SelectSide(RIGHT);
    for (Page = 0; Page < 8; Page++) {                    /* loop on the 8 pages */

        WriteInstruction(LCD_SET_PAGE| Page,RIGHT); /* Set the page */
        for (Column = 64; Column < 128; Column++)
            WriteData(PictureData[(128*Page)+Column],RIGHT);
    }
}

unsigned int KS0108::ReadArrayData(const unsigned int* ptr)
{
    return (*ptr);
}

void KS0108::DrawBitmap(const unsigned int * bitmap, unsigned int x, unsigned int y, unsigned int color)
{
    unsigned int width, height;
    unsigned int i, j;

    width = ReadArrayData(bitmap++);
    height = ReadArrayData(bitmap++);
    for(j = 0; j < height / 8; j++) {
        GotoXY(x, y + (j*8) );
        for(i = 0; i < width; i++) {
            unsigned int displayData = ReadArrayData(bitmap++);
            if(color == BLACK)
                WriteData(displayData);
            else
                WriteData(~displayData);
        }
    }
}
/******************************************************************************************/


void KS0108::GotoXY(unsigned int x, unsigned int y)
{
    unsigned int chip, cmd;

    if( (x > SCREEN_WIDTH-1) || (y > SCREEN_HEIGHT-1) )    // exit if coordinates are not legal
        return;
    Coord.x = x;                                    // save new coordinates
    Coord.y = y;

    if(y/8 != Coord.page) {
        Coord.page = y/8;
        cmd = LCD_SET_PAGE | Coord.page;            // set y address on all chips
        for(chip=0; chip < 2; chip++) {
            WriteInstruction(cmd, chip);
        }
    }
    chip = Coord.x/64; //select chip
    x = x % 64; // valeur de X quelque soit le CS affich&#65533;
    cmd = LCD_SET_ADD | x;
    WriteInstruction(cmd, chip);                    // set x address on active chip

}


/*****************************************************************************************/



void KS0108::Putchar (int page, int col,unsigned char c)
{
    if (c>31 && c<127) {
        for(int i=0; i<5; i++) {
            WriteDataColPag(page,col+i,System5x7[((c-32)*5+i)+6]);
        }
    }
}



void KS0108::PutString(unsigned int x, unsigned int y,char* str)
{

    while(*str != 0) {
        Putchar(x,y,*str);
        str++;
        y+=System5x7[2];
    }

}

void KS0108::PrintFloat(float val, unsigned int x,unsigned int y)
{
    char buf[20] = {};  // prints up to 20 digits
    sprintf(buf,"%f",val);
    PutString(x,y,buf);

}


void KS0108::PrintInteger(int val,unsigned int x,unsigned int y)
{
    char buf[20] = {};  // prints up to 20 digits
    sprintf(buf,"%d",val);
    PutString(x,y,buf);
}

void KS0108::SelectFont(unsigned int* font,unsigned int color, FontCallback callback)
{
    Font = font;
    FontRead = callback;
    FontColor = color;
}


int KS0108::PrintChar(char c)
{
    unsigned int width = 0;
    unsigned int height = FontRead(Font+FONT_HEIGHT);
    unsigned int bytes = (height+7)/8;

    unsigned int firstChar = FontRead(Font+FONT_FIRST_CHAR);
    unsigned int charCount = FontRead(Font+FONT_CHAR_COUNT);

    unsigned int index = 0;
    unsigned int x=Coord.x , y=Coord.y;

    if(c < firstChar || c >= (firstChar+charCount)) {
        return 1;
    }
    c-= firstChar;

    if( FontRead(Font+FONT_LENGTH) == 0 && FontRead(Font+FONT_LENGTH+1) == 0) {
        // zero length is flag indicating fixed width font (array does not contain width data entries)
        width = FontRead(Font+FONT_FIXED_WIDTH);
        index = c*bytes*width+FONT_WIDTH_TABLE;
    } else {
        // variable width font, read width data, to get the index
        for(unsigned int i=0; i<c; i++) {
            index += FontRead(Font+FONT_WIDTH_TABLE+i);
        }
        index = index*bytes+charCount+FONT_WIDTH_TABLE;
        width = FontRead(Font+FONT_WIDTH_TABLE+c);
    }

    // last but not least, draw the character
    for(unsigned int i=0; i<bytes; i++) {
        unsigned int page = i*width;
        for(unsigned int j=0; j<width; j++) {
            unsigned int data = FontRead(Font+index+page+j);

            if(height > 8 && height < (i+1)*8) {
                data >>= (i+1)*8-height;
            }

            WriteData(data);

        }
        // 1px gap between chars
        WriteData(0x00);
        GotoXY(x,Coord.y+8);

    }
    GotoXY(x+width+1, y);


    return 0;
}

void KS0108::PrintString(char* str)
{
    int x = Coord.x;
    while(*str != 0) {
        if(*str == '\n') {
            GotoXY(x, Coord.y+ FontRead(Font+FONT_HEIGHT));
        } else {
            PrintChar(*str);
        }
        str++;
    }
}

void KS0108::PrintNumber(long n)
{
    char buf[10];  // prints up to 10 digits
    char i=0;
    if(n==0)
        PrintChar('0');
    else {
        if(n < 0) {
            PrintChar('-');
            n = -n;
        }
        while(n>0 && i <= 10) {
            buf[i++] = n % 10;  // n % base
            n /= 10;   // n/= base
        }
        for(; i >0; i--)
            PrintChar((char) (buf[i-1] < 10 ? '0' + buf[i-1] : 'A' + buf[i-1] - 10));
    }
}

bool KS0108::writeLine (const char *line, uint8_t row)
{
//"SystemFont5x7.h"
// |y   _x
    int x = Coord.x;

    Font = System5x7;

    FontColor = BLACK;

    // LCD.SelectFont(System5x7,BLACK,ReadData);
    GotoXY(x+10, Coord.y+7);
    PrintString(const_cast<char *> (line) );
    return 1;
}
void KS0108::showUpArrow (bool show)
{

}
void KS0108::showDownArrow (bool show)
{

}
uint8_t KS0108::getLines (void)
{
return 0;
}

uint8_t KS0108::getLineLength (void) 
{
return 0;
}



