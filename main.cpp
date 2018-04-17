//#include<windows.h>
#include <stdlib.h>
#include <GL/glut.h>
#include<string.h>

#include<stdio.h>
#define maxHt 1024
#define maxWd 1024
#define maxVer 10000

int flag=0;
float pos=0,y=0;
int showclouds=0;
float increase=0;
int increase2=0;
int increaseclouds=0;

typedef struct edgebucket{
    int ymax;
    float xofymin;
    float slopeinverse;
}EdgeBucket;

typedef struct edgetabletup{
    int countEdgeBucket;
    EdgeBucket buckets[maxVer];
}EdgeTableTuple;

EdgeTableTuple EdgeTable[maxHt], ActiveEdgeTuple;

void initEdgeTable(){
    int i;
    for (i=0; i<maxHt; i++){
        EdgeTable[i].countEdgeBucket = 0;
    }
    ActiveEdgeTuple.countEdgeBucket = 0;
}

void midpoint(int X1,int Y1,int X2,int Y2)
{
  int dx=(X2-X1);
  int dy=(Y2-Y1);
  int d=dy-(dx/2);
  int x;
  int y;
  x=X1;
  y=Y1;
  glBegin(GL_POINTS);
  glVertex2d(x,y);
  glPointSize(10);
  while(x<X2){

        x++;

        if(d<0)
            d=d+dy;
        else{
            d+=(dy-dx);
            y++;
        }
            glVertex2d(x,y);
    }
  glEnd();
}

void insertionSort(EdgeTableTuple *ett){
    int i,j;
    EdgeBucket temp;

    for (i = 1; i < ett->countEdgeBucket; i++){
        temp.ymax = ett->buckets[i].ymax;
        temp.xofymin = ett->buckets[i].xofymin;
        temp.slopeinverse = ett->buckets[i].slopeinverse;
        j = i - 1;

    while ((temp.xofymin < ett->buckets[j].xofymin) && (j >= 0)){
        ett->buckets[j + 1].ymax = ett->buckets[j].ymax;
        ett->buckets[j + 1].xofymin = ett->buckets[j].xofymin;
        ett->buckets[j + 1].slopeinverse = ett->buckets[j].slopeinverse;
        j = j - 1;
    }
    ett->buckets[j + 1].ymax = temp.ymax;
    ett->buckets[j + 1].xofymin = temp.xofymin;
    ett->buckets[j + 1].slopeinverse = temp.slopeinverse;
    }
}


void storeEdgeInTuple (EdgeTableTuple *receiver,int ym,int xm,float slopInv){
    (receiver->buckets[(receiver)->countEdgeBucket]).ymax = ym;
    (receiver->buckets[(receiver)->countEdgeBucket]).xofymin = (float)xm;
    (receiver->buckets[(receiver)->countEdgeBucket]).slopeinverse = slopInv;
    insertionSort(receiver);
    (receiver->countEdgeBucket)++;
}


void storeEdgeInTable (int x1,int y1, int x2, int y2){
    float m,minv;
    int ymaxTS,xwithyminTS, scanline; //ts stands for to store

    if (x2==x1){
        minv=0.000000;
    }
    else{
    m = ((float)(y2-y1))/((float)(x2-x1));
    if (y2==y1)
        return;

    minv = (float)1.0/m;
    }

    if (y1>y2){
        scanline=y2;
        ymaxTS=y1;
        xwithyminTS=x2;
    }
    else{
        scanline=y1;
        ymaxTS=y2;
        xwithyminTS=x1;
    }
    storeEdgeInTuple(&EdgeTable[scanline],ymaxTS,xwithyminTS,minv);
}


void removeEdgeByYmax(EdgeTableTuple *Tup,int yy){
    int i,c;
    for (i=0; i< Tup->countEdgeBucket; i++){
        if (Tup->buckets[i].ymax == yy){
            for ( c = i ; c < Tup->countEdgeBucket -1 ; c++ ){
                Tup->buckets[c].ymax =Tup->buckets[c+1].ymax;
                Tup->buckets[c].xofymin =Tup->buckets[c+1].xofymin;
                Tup->buckets[c].slopeinverse = Tup->buckets[c+1].slopeinverse;
                }
                Tup->countEdgeBucket--;
            i--;
        }
    }
}


void updatexbyslopeinv(EdgeTableTuple *Tup){
    int i;
    for (i=0; i<Tup->countEdgeBucket; i++){
        (Tup->buckets[i]).xofymin =(Tup->buckets[i]).xofymin + (Tup->buckets[i]).slopeinverse;
    }
}


void ScanlineFill(float r,float g,float b)
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
    for (i=0; i<maxHt; i++){
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++){
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                     ymax,EdgeTable[i].buckets[j].xofymin,
                    EdgeTable[i].buckets[j].slopeinverse);
        }
        removeEdgeByYmax(&ActiveEdgeTuple, i);
        insertionSort(&ActiveEdgeTuple);
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = -1;
        x2 = -1;
        ymax1 = -1;
        ymax2 = -1;
        while (j<ActiveEdgeTuple.countEdgeBucket){
            if (coordCount%2==0){
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2)
                {
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else{
                        coordCount++;
                    }
                }
                else{
                        coordCount++;
                }
            }
            else{
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;
                FillFlag = 0;
                if (x1==x2){
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else{
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else{
                        coordCount++;
                        FillFlag = 1;
                }
            if(FillFlag){
                glColor3f(r,g,b);
                midpoint(x1,i,x2,i);
                }
        }
        j++;
    }
    updatexbyslopeinv(&ActiveEdgeTuple);
}
}


void ScanlineFill(float r,float g,float b,float extra)
{
    int i, j, x1, ymax1, x2, ymax2, FillFlag = 0, coordCount;
    for (i=0; i<maxHt; i++){
        for (j=0; j<EdgeTable[i].countEdgeBucket; j++){
            storeEdgeInTuple(&ActiveEdgeTuple,EdgeTable[i].buckets[j].
                     ymax,EdgeTable[i].buckets[j].xofymin,
                    EdgeTable[i].buckets[j].slopeinverse);
        }
        removeEdgeByYmax(&ActiveEdgeTuple, i);
        insertionSort(&ActiveEdgeTuple);
        j = 0;
        FillFlag = 0;
        coordCount = 0;
        x1 = -1;
        x2 = -1;
        ymax1 = -1;
        ymax2 = -1;
        while (j<ActiveEdgeTuple.countEdgeBucket){
            if (coordCount%2==0){
                x1 = (int)(ActiveEdgeTuple.buckets[j].xofymin);
                ymax1 = ActiveEdgeTuple.buckets[j].ymax;
                if (x1==x2)
                {
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x2 = x1;
                        ymax2 = ymax1;
                    }
                    else{
                        coordCount++;
                    }
                }
                else{
                        coordCount++;
                }
            }
            else{
                x2 = (int)ActiveEdgeTuple.buckets[j].xofymin;
                ymax2 = ActiveEdgeTuple.buckets[j].ymax;
                FillFlag = 0;
                if (x1==x2){
                    if (((x1==ymax1)&&(x2!=ymax2))||((x1!=ymax1)&&(x2==ymax2))){
                        x1 = x2;
                        ymax1 = ymax2;
                    }
                    else{
                        coordCount++;
                        FillFlag = 1;
                    }
                }
                else{
                        coordCount++;
                        FillFlag = 1;
                }
            if(FillFlag){
                glColor4f(r,g,b,extra);
                midpoint(x1,i,x2,i);
                }
        }
        j++;
    }
    updatexbyslopeinv(&ActiveEdgeTuple);
}
}


void drawPolyDino(int a[],int sizei)
{
    int count = 0,x1,y1,x2,y2;
    int n=0;
    while(n!=sizei){
        count++;
        if (count>2){
            x1 = x2;
            y1 = y2;
            count=2;
        }
        if (count==1){
            x1=a[n];
            n++;
            y1=a[n];
            n++;
        }
        else{
            x2=a[n];
            n++;
            y2=a[n];
            n++;
            storeEdgeInTable(x1, y1, x2, y2);
        }
    }
}


void drawDino(int a[],int n,float r,float g , float b){
    initEdgeTable();
    drawPolyDino(a,n);
    ScanlineFill(r,g,b);
}


void drawDino(int a[],int n,float r,float g , float b,float extra){
    initEdgeTable();
    drawPolyDino(a,n);
    ScanlineFill(r,g,b,extra);
}


void plot(int x,int y,int X1,int Y1,float r,float g,float b,float alpha){
    glColor4f(r,g,b,alpha);
    //glColor3f( 0.80, 0.80,0.80);
    glBegin(GL_POINTS);
    glVertex2i(x+X1,y+Y1);
    glEnd();
}

int x=-150,o=0,xd=-150;
int x1=0,z=0;
float a=0,a1=0,moving,angle=0;
float z5=0,u=0,flag12=0,begin;
float k=0,y2=0;
//flag=0;
int flag55=0,var=0,flag1=0,flag551=0,vari=0,vard=0,varid=0,then=0;
float p=0.0,q=0.0;
#define maxx 10
#define maxy 12
#define dx 27.7
#define dy 12
GLfloat xangle=0.0,yangle=0.0,zangle=0.0;   /* axis angles */

void draw_pixel(int x,int y)
{
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
}

/*-------------------------------------------------------------------*/
/*****************************DECLARATIONS****************************/
/*-------------------------------------------------------------------*/

    GLfloat vertices[][3] ={{160,390,-70},{425,390,-70},
                    {425,510,-70}, {160,520,-70},

                    {135,370,70}, {400,370,70},
                    {400,490,70}, {135,500,70},

                    {135,447,70},{400,447,70},
                    {425,467,-70},{410,520,-70},

                    {385,500,70}, {160,467,-70},
                    {320,467,-70},{320,520,-70},

{380,520,-70},{380,390,-70},{320,390,-70}};

    GLfloat colors[][3] = {{1.0,1.0,0.0},{0.0,0.6,0.7},{.3,.4,.5}};

GLfloat verticesd[][3] ={{160,390-175,-70},{425,390-175,-70},
                    {425,510-175,-70}, {160,520-175,-70},

                    {135,370-175,70}, {400,370-175,70},
                    {400,490-175,70}, {135,500-175,70},

                    {135,447-175,70},{400,447-175,70},
                    {425,467-175,-70},{410,520-175,-70},

                    {385,500-175,70}, {160,467-175,-70},
                    {320,467-175,-70},{320,520-175,-70},

{380,520-175,-70},{380,390-175,-70},{320,390-175,-70}};

    GLfloat colorsd[][3] = {{1.0,1.0,0.0},{0.0,0.6,0.7},{.3,.4,.5}};

            //    FUNCTION wheel
/*-------------------------------------------------------------------*/
void wheel1()
{
    glColor3f(0,0,0);

    glPushMatrix();
    glTranslatef(345,377,-70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(190,377,-70);
    glutSolidTorus(5,15,100,90);//front two wheels tyre
    glPopMatrix();

    glColor3ub(100,100,100);

    glPushMatrix();
    glTranslatef(345,377,-70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(190,377,-70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();// front two wheels
}

void wheel2()
{
    glColor3f(0,0,0);

    glPushMatrix();
    glTranslatef(180,370,70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(335,370,70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();            //back two wheels tyre

    glColor3ub(100,100,100);

    glPushMatrix();
    glTranslatef(335,370,70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(180,370,70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();      //back two wheels
}
/*-------------------------------------------------------------------*/
                //    FUNCTION cube
/*-------------------------------------------------------------------*/
void polygon(int a, int b, int c , int d,int E,int f)
{
    glBegin(GL_POLYGON);
        glColor3fv(colors[E]);
        glVertex3fv(vertices[a]);
        glVertex3fv(vertices[b]);
        glVertex3fv(vertices[c]);
        glVertex3fv(vertices[d]);
        if(f!=0)
        glVertex3fv(vertices[f]);
    glEnd();
}

void colorcube()
{
    int i;
    wheel1();
    polygon(0,1,5,4,0,0);


    polygon(13,14,18,0,0,0);
    polygon(15,16,17,18,2,0);
    polygon(16,11,2,1,0,17);


    polygon(0,4,8,13,0,0);
    polygon(1,10,9,5,0,0);
    polygon(9,10,2,6,1,0);
    polygon(4,5,9,8,0,0);
    polygon(8,9,6,12,1,7);
    glColor3ub(100,40,50);
    for(i=0;i<=180;i+=45)
    {
        glBegin(GL_LINES);
        glVertex3f(180+i,447,70);
        glVertex3f(180+i,500,70);
        glEnd();
    }


    polygon(13,8,7,3,1,0);
    polygon(3,15,14,13,1,0);
    polygon(6,2,11,12,0,0);
    polygon(11,3,7,12,0,0);
}

void road2()
{
/************** left part of road  *********/
int x,y;
//glColor3ub(7,255,13);
int a1[]={0,650,1000,650,1000,0,0,0,0,650};
drawDino(a1,10,0.03,1.0,0.05);
//glColor3ub(30,40,50);
 int a2[]={0,420,1000,420,1000,300,0,300,0,420};
drawDino(a2,10,0.12,0.16,0.2);
 int a3[]={750,650,900,650,1000,0,650,0,750,650};
drawDino(a3,10,0.12,0.16,0.2);
/************  STRIPES  ************/
//glColor3f(1.0,0.9,0.0);
for(x=0;x<1000;x=x+60)
{
int  a4[]={x,352.5+19,x,357.5+19,x+30,357.5+19,x+30,352+19,x,352+19};
drawDino(a4,10,1.0,0.9,0.0);
}

for(y=650;y>0;y=y-60)
{
 int a5[]={822,y,826,y,826,y-30,822,y-30,822,y};
drawDino(a5,10,1.0,0.9,0.0);
}

}
void line()
{
        int  a6[]={400,390,425,410,425,407,400,387,393,393,393,390,400,390};
        drawDino(a6,14,1.0,0.0,1.0);
}
/*-------------------------------------------------------------------*/
            //    FUNCTION Woman
/*-------------------------------------------------------------------*/
void tree12()
{
    //trunk1
    glColor3ub(95,6,5);
    double len=100;
    double thick=20;
    glPushMatrix();
    glTranslated(100+450,150+330,0.0);
    glScaled(thick,len,thick);
    glutSolidCube(1.0);
    glPopMatrix();
//leaves1
    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(100+450,230+310,0.0);
    glutSolidCone(70,140,3,2);
    glPopMatrix();

 //leaves2

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100+450,260+310,0.0);
    glutSolidCone(60,120,3,2);
    glPopMatrix();

// leaves3

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100+450,290+310,0);
    glutSolidCone(50,100,3,2);
    glPopMatrix();
}void tree1()
{
//trunk1
    glColor3ub(95,6,5);
    double len=100;
    double thick=20;
    glPushMatrix();
    glTranslated(100,150-48,0.0);
    glScaled(thick,len,thick);
    glutSolidCube(1.0);
    glPopMatrix();

//leaves1


    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(100,235-48,0.0);
    glutSolidCone(70,140,3,2);
    glPopMatrix();

 //leaves2

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100,270-48,0.0);
    glutSolidCone(60,120,3,2);
    glPopMatrix();
// leaves3
    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100,300-48,0);
    glutSolidCone(50,100,3,2);
    glPopMatrix();
}

void woman()
{
    //face
    glColor3ub(0,0,0);
    glPushMatrix();
    glTranslatef(540,495,0);
    glutSolidTorus(1,10,100,90);
    glPopMatrix();
    glColor3ub(255,191,128);
glPushMatrix();
    glTranslatef(540,494,0);
    glutSolidTorus(7,7,100,90);
    glPopMatrix();
    glColor3ub(0,0,0);
    glBegin(GL_LINES);
         glVertex2i(540,494);
         glVertex2i(540,490); //nose
          glVertex2i(531,498);
        glVertex2i(532,499);
         glVertex2i(532,499);
        glVertex2i(537,498);//eyebrow
          glVertex2i(549,498);
        glVertex2i(548,499);
        glVertex2i(548,499);
         glVertex2i(543,498);//eyebrow
     glEnd();
//ear right
   int  a9[]={540-14,494+1,540-14,490+1,538-14,489+1,538-14,495+1,540-14,494+1};
    drawDino(a9,10,1.0,0.75,0.5);
    //ear left
    int a8[]={554,495,556,496,556,491,554,490,554,495};
    drawDino(a8,10,1.0,0.75,0.5);
    /*int a10[]={539-14,492,542-14,485,536-14,485,539-14,492};
    drawDino(a10,8,1.0,0.33,0.35);*/
//hair
    int a26[]={525,499,549,509,540,512,528,507,525,499};
      drawDino(a26,10,0,0,0);
    int a25[]={540,507,549,509,552,507,555,499,540,507};
      drawDino(a25,10,0,0,0);

// eyes

        int a24[]= {533,496,535,496,535,494,533,494,533,496};
          drawDino(a24,10,0.0,0.0,0.1);

         int a23[]={545,496,547,496,547,494,545,494,545,496};
         drawDino(a23,10,0.0,0.0,0.1);

//mouth
       int a22[]={534,487,540,484,546,487,540,485,534,487};
       drawDino(a22,10,0.6,0.2,0.2);
//shirt
     int a21[]={529,480,551,480,566,469,561,460,556,465,556,445,524,445,524,465,519,460,514,469,529,480};
     drawDino(a21,22,0.63,0.59,1.0);
//neck
     int a20[]={533,480,547,480,545,471,535,471,533,480};
     drawDino(a20,10,1.0,0.75,0.5);
//hands
       int a19[]={565,468,575,453,567,454,562,462,565,468};
       drawDino(a19,10,1.0,0.75,0.5);
       int a18[]={575,453,556,438,556,445,567,454,575,453};
       drawDino(a18,10,1.0,0.75,0.5);
       int a17[]={515,468,505,453,513,454,518,462,515,468};
       drawDino(a17,10,1.0,0.75,0.5);
       int  a16[]= {505,453,524,438,524,445,513,454,505,453};
       drawDino(a16,10,1.0,0.75,0.5);
// belt
    int a15[]={556,445,524,445,524,440,556,440};
    drawDino(a15,10,0.04,0.47,0.51);
    // leg
int a14[]={555,440,525,440,520,405,530,405,533,438,550,405,560,405,555,440};
drawDino(a14,16,1.0,0.75,0.5);
//skirt
   int  a13[]={524,440,556,440,566,410,514,410,524,440};
     drawDino(a13,10,0.71,0.31,0.35);
//shoe left
    int a12[]={530,405,530,396,528,396,528,404,522,396,512,396,520,405,530,405};
    drawDino(a12,16,0.71,0,0);
     //shoe right
    int a11[]={550,405,550,396,552,396,552,404,558,396,568,396,560,405,550,405};
    drawDino(a11,16,0.71,0,0);
}
void wheel1d()
{
    glColor3f(0,0,0);

    glPushMatrix();
    glTranslatef(345,377-175,-70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(190,377-175,-70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();

    glColor3ub(100,100,100);

    glPushMatrix();
    glTranslatef(345,377-175,-70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(190,377-175,-70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();
}

void wheel2d()
{
    glColor3f(0,0,0);

    glPushMatrix();
    glTranslatef(180,370-175,70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(335,370-175,70);
    glutSolidTorus(5,15,100,90);
    glPopMatrix();

    glColor3ub(100,100,100);

    glPushMatrix();
    glTranslatef(335,370-175,70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(180,370-175,70);
    glutSolidTorus(5,5,10,69);
    glPopMatrix();
}
/*-------------------------------------------------------------------*/
                //    FUNCTION cube
/*-------------------------------------------------------------------*/
void polygond(int a, int b, int c , int d,int E,int f)
{
    glBegin(GL_POLYGON);
        glColor3fv(colorsd[E]);
        glVertex3fv(verticesd[a]);
        glVertex3fv(verticesd[b]);
        glVertex3fv(verticesd[c]);
        glVertex3fv(verticesd[d]);
        if(f!=0)
        glVertex3fv(verticesd[f]);
    glEnd();
}

void colorcubed()
{
    int i;
    wheel1d();
    polygond(0,1,5,4,0,0);
    polygond(13,14,18,0,0,0);
    polygond(15,16,17,18,2,0);
    polygond(16,11,2,1,0,17);
    polygond(0,4,8,13,0,0);
    polygond(1,10,9,5,0,0);
    polygond(9,10,2,6,1,0);
    polygond(4,5,9,8,0,0);
    polygond(8,9,6,12,1,7);
    glColor3ub(100,40,50);
    for(i=0;i<=180;i+=45)
    {
        glBegin(GL_LINES);
        glVertex3f(180+i,447-175,70);
        glVertex3f(180+i,500-175,70);
        glEnd();
    }
    polygond(13,8,7,3,1,0);
    polygond(3,15,14,13,1,0);
    polygond(6,2,11,12,0,0);
    polygond(11,3,7,12,0,0);
wheel2d();
}
void womand()
{
  //face
  glColor3ub(0,0,0);
  glPushMatrix();
  glTranslatef(540,495-175,0);
  glutSolidTorus(1,10,100,90);
  glPopMatrix();
  glColor3ub(255,191,128);
glPushMatrix();
  glTranslatef(540,494-175,0);
  glutSolidTorus(7,7,100,90);
  glPopMatrix();
  glColor3ub(0,0,0);
  glBegin(GL_LINES);
   glVertex2i(540,494-175);
   glVertex2i(540,490-175); //nose
     glVertex2i(531,498-175);
   glVertex2i(532,499-175);
   glVertex2i(532,499-175);
   glVertex2i(537,498-175);//eyebrow
      glVertex2i(549,498-175);
   glVertex2i(548,499-175);
   glVertex2i(548,499-175);
   glVertex2i(543,498-175);//eyebrow
 glEnd();

  //ear right
  int b2[]={540-14,494+1-175,540-14,490+1-175,538-14,489+1-175,538-14,495+1-175,540-14,494+1-175};
  drawDino(b2,10,1.0,0.75,0.5);

  //ear left
  int b3[]={554,495-175,556,496-175,556,491-175,554,490-175,554,495-175};
  drawDino(b3,10,1.0,0.75,0.5);
  //hair
  int b6[]={525,499-175,549,509-175,540,512-175,528,507-175,525,499-175};
  drawDino(b6,10,0.0,0.0,0.1);
  int b7[]={540,507-175,549,509-175,552,507-175,555,499-175,540,507-175};
  drawDino(b7,10,0.0,0.0,0.1);
  // eyes
int b8[]={533,496-175,535,496-175,535,494-175,533,494-175,533,496-175};
drawDino(b8,10,0.0,0.0,0.1);
  int b9[]={547,496-175,547,496-175,547,494-175,545,494-175,545,496-175};
  drawDino(b9,10,0.0,0.0,0.1);


//mouth
    //   glColor3ub(150,50,50);
  int b10[]={534,487-175,540,484-175,546,487-175,540,485-175,534,487-175};
  drawDino(b10,10,0.6,0.2,0.2);
//shirt
     // glColor3ub(160,150,250);
  int b11[]={529,480-175,551,480-175,566,469-175,561,460-175,556,465-175,556,445-175,524,445-175,524,465-175,519,460-175,514,469-175,529,480-175};
  drawDino(b11,22,0.63,0.6,1.0);

//neck
   //   glColor3ub(255,190,128);
   int b12[]={533,480-175,547,480-175,545,471-175,535,471-175,533,480-175};
   drawDino(b12,10,0.0,0.0,0.1);
//hands
     glBegin(GL_POLYGON);
        glColor3ub(255,191,128);
         int c6[]={565,468-175,575,453-175,567,454-175,562,462-175,565,468-175};
         drawDino(c6,10,1.0,0.75,0.5);
        int c7[]={575,453-175,556,438-175,556,445-175,567,454-175,575,453-175};
        drawDino(c7,10,1.0,0.75,0.5);
         int c8[]={515,468-175,505,453-175,513,454-175,518,462-175,515,468-175};
     drawDino(c8,10,1.0,0.75,0.5);
        int c9[]={505,453-175,524,438-175,524,445-175,513,454-175,505,453-175};
      drawDino(c9,10,1.0,0.75,0.5);
// belt
        int c5[]={556,445-175,524,445-175,524,440-175,556,440-175,556,445-175};
        drawDino(c5,10,0.04,0.08,0.47);
    /// leg
    int c1[]={555,440-175,525,440-175,520,405-175,530,405-175,533,438-175,550,405-175,560,405-175,555,440-175};
    drawDino(c1,16,1.0,0.75,0.5);
//skirt
        int c2[]={524,440-175,556,440-175,566,410-175,514,410-175,524,440-175};
        drawDino(c2,10,0.71,0.31,0.35);
    //shoe left
    int c3[]={530,405-175,530,396-175,528,396-175,528,404-175,522,396-175,512,396-175,520,405-175,530,405-175};
    drawDino(c3,16,0.71,0.0,0.0);

    //shoe right
    int c4[]={550,405-175,550,396-175,552,396-175,552,404-175,558,396-175,568,396-175,560,405-175,550,405-175};
    drawDino(c4,16,0.71,0.0,0.0);

}
void road2d()
{  /************** left part of road  *********/
int x;
int w1[]={0,650,1000,650,1000,0,0,0,0,650};
drawDino(w1,10,0.03,1.0,0.51);

int w2[]={0,420-175,1000,420-175,1000,300-175,0,300-175,0,420-175};
drawDino(w2,10,0.12,0.16,0.2);
/************  STRIPES  ************/
//glColor3f(1.0,0.9,0.0);
for(x=0;x<1000;x=x+60)
{
  int r1[]={x,352.5+19-175,x,357.5+19-175,x+30,357.5+19-175,x+30,352.5+19-175,x,352.5+19-175};
  drawDino(r1,10,1.0,0.9,0.0);
}
}            //    FUNCTION line
/*-------------------------------------------------------------------*/
void buildingd()
{
    //buliding
    glColor3ub(255,70,20);
    double len=300;
    double thick=380;
    glPushMatrix();
    glTranslatef(650+55,520,70.0);
    glScalef(thick,len,thick);
    glutSolidCube(1.0);
    glPopMatrix();
    //door
    glColor3f(0.0,0.6,0.7);
    double len1=50;
    double thick1=80;
    glPushMatrix();
    glTranslatef(650+55,520-125,70.0);
    glScalef(thick1,len1,thick1);
    glutSolidCube(1.0);
    glPopMatrix();
    glColor3ub(0,0,0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(550+115,550-130);
    glVertex2i(630+115,550-130);
    glVertex2i(630+115,520-150);
    glVertex2i(550+115,520-150);
    glEnd();
    glBegin(GL_LINES);
    glVertex2i(704,550-130);
    glVertex2i(704,520-150);
    glEnd();
    //windows
    glColor3f(0.0,0.6,0.7);
    double len2=30;
    double thick2=30;
    glPushMatrix();
    glTranslatef(650-100,520,70.0);
    glScalef(thick2,len2,thick2);
    glutSolidCube(1.0);
    glPopMatrix();

     glColor3f(0.0,0.6,0.7);
    double len3=30;
    double thick3=30;
    glPushMatrix();
    glTranslatef(650,520,70.0);
    glScalef(thick3,len3,thick3);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.0,0.6,0.7);
    double len4=30;
    double thick4=30;
    glPushMatrix();
    glTranslatef(650+100,520,70.0);
    glScalef(thick4,len4,thick4);
    glutSolidCube(1.0);
    glPopMatrix();

   glColor3f(0.0,0.6,0.7);
    double len5=30;
    double thick5=30;
    glPushMatrix();
    glTranslatef(650+200,520,70.0);
    glScalef(thick5,len5,thick5);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.0,0.6,0.7);
    double len6=30;
    double thick6=30;
    glPushMatrix();
    glTranslatef(650-100,520+100,70.0);
    glScalef(thick6,len6,thick6);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.0,0.6,0.7);
    double len7=30;
    double thick7=30;
    glPushMatrix();
    glTranslatef(650,520+100,70.0);
    glScalef(thick7,len7,thick7);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.0,0.6,0.7);
    double len8=30;
    double thick8=30;
    glPushMatrix();
    glTranslatef(650+100,520+100,70.0);
    glScalef(thick8,len8,thick8);
    glutSolidCube(1.0);
    glPopMatrix();

    glColor3f(0.0,0.6,0.7);
    double len9=30;
    double thick9=30;
    glPushMatrix();
    glTranslatef(650+200,520+100,70.0);
    glScalef(thick9,len9,thick9);
    glutSolidCube(1.0);
    glPopMatrix();

}
void walld()
{
    //wall left
    int a30[]={600+150,433,900+150,433,900+150,300,600+150,300,600+150,433};
    drawDino(a30,10,0.8,0.2,0.2);
     //wall right
    int a31[]={0-50,433,300-50,433,300-50,300,0-50,300,0-50,433};
    drawDino(a31,10,0.8,0.2,0.2);
    int a[]={250,433,380,433,380,300,250,300,250,433};
    drawDino(a,10,0.98,0.96,0.96);
    int a32[]={0+260,423,300+70,423,300+70,310,0+260,310,260,423};
    drawDino(a32,10,0.98,0.78,0.78);

}
void gated()
{
    //gate right
    //glColor3ub(0,0,0);
    int e1[]={750,300,600,300,600,303,750,303,750,300};
    drawDino(e1,10,0,0,0);

    int e2[]={600,300,600,450,597,450,597,303,600,300};
    drawDino(e2,10,0,0,0);

    int e3[]={600,450,750,433,750,430,600,447,600,450};
    drawDino(e3,10,0,0,0);

    int e4[]={650,300,650,442,653,442,653,300,650,300};
    drawDino(e4,10,0,0,0);
    int e5[]={711,300,711,437,714,437,714,300,711,300};
    drawDino(e5,10,0,0,0);

    int e6[]={600,350,750,350,750,345,600,345,600,350};
    drawDino(e6,10,0,0,0);

   //gate left
   int e7[]={380,300,500,340,500,343,380,303,380,300};
   drawDino(e7,10,0,0,0);

    int e8[]={380,433,500,473,500,476,380,436,380,433};
    drawDino(e8,10,0,0,0);

    int e9[]={500-1,340,500-1,473,503-1,476,503-1,343,500-1,340};
    drawDino(e9,10,0,0,0);

    int e10[]={500-41,340-15,500-41,473-15,503-41,476-15,503-41,343-15,500-41,340-15};
    drawDino(e10,10,0,0,0);

    int e11[]={500-81,340-25,500-81,473-25,503-81,476-25,503-81,343-25,500-81,340-25};
    drawDino(e11,10,0,0,0);

    int e12[]={380,433-90,500,473-90,500,478-90,380,438-90,380,433-90};
    drawDino(e12,10,0,0,0);

}
void treed()
{

    //trunk1
    glColor3ub(95,6,5);
    double len=80;
    double thick=15;
    glPushMatrix();
    glTranslated(100+850,150+330,0.0);
    glScaled(thick,len,thick);
    glutSolidCube(1.0);
    glPopMatrix();

//leaves1
    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(100+850,230+290,0.0);
    glutSolidCone(60,120,3,2);
    glPopMatrix();

 //leaves2

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100+850,260+290,0.0);
    glutSolidCone(50,100,3,2);
    glPopMatrix();

// leaves3

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100+850,290+290,0);
    glutSolidCone(40,800,3,2);
    glPopMatrix();
}
void tree1d()
{
    //trunk1
    glColor3ub(95,6,5);
    double len=80;
    double thick=15;
    glPushMatrix();
    glTranslated(100,150+330,0.0);
    glScaled(thick,len,thick);
    glutSolidCube(1.0);
    glPopMatrix();
//leaves1
    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(100,230+290,0.0);
    glutSolidCone(60,120,3,2);
    glPopMatrix();

 //leaves2

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100,260+290,0.0);
    glutSolidCone(50,100,3,2);
    glPopMatrix();

// leaves3

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(100,290+290,0);
    glutSolidCone(40,800,3,2);
    glPopMatrix();
}
void tree2d()
{
    //trunk1
    glColor3ub(95,6,5);
    double len=80;
    double thick=15;
    glPushMatrix();
    glTranslated(200,150+330,0.0);
    glScaled(thick,len,thick);
    glutSolidCube(1.0);
    glPopMatrix();

//leaves1


    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(200,230+290,0.0);
    glutSolidCone(60,120,3,2);
    glPopMatrix();

 //leaves2

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(200,260+290,0.0);
    glutSolidCone(50,100,3,2);
    glPopMatrix();

// leaves3

    glColor3f(0.0,0.2,0.0);
    glPushMatrix();
    glLoadIdentity();
    glTranslated(200,290+290,0);
    glutSolidCone(40,800,3,2);
    glPopMatrix();
}
void bus_move()
{
    if(x<50)
{
        x+=3;
        glPushMatrix();
        glTranslatef(-100,0,-90);
        woman();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(x,0,0);
        wheel1();
        colorcube();
        wheel2();
        line();
        glPopMatrix();
}
    if(x>=50)
        vari=1;


    if(flag55==1 )
    {
        x+=6;
        glPushMatrix();
        glTranslatef(x,0,0);
        wheel1();
        colorcube();
        wheel2();
        line();
//        text1();
        glPopMatrix();
    }
    if(x>=865)
        var=1;
}
void bus_moved()
{
    if(xd>50)
{
        xd+=3;
        glPushMatrix();
        glTranslatef(-100,0,-90);
        womand();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(xd,0,0);
        wheel1d();
        colorcubed();
        wheel2d();
        //lined();
        glPopMatrix();
}
    if(flag551==1)
{
        xd+=5;
        glPushMatrix();
        glTranslatef(xd,0,0);
        wheel1d();
        colorcubed();
        wheel2d();
        //lined();
        //text1d();
        glPopMatrix();
    }
    if(xd>50)
        varid=1;

}
static void SpecialKeyFunc( int Key, int x, int y )
{
    switch ( Key )
{
    case GLUT_KEY_UP:                     /*move to right */
            glutPostRedisplay();
        break;
    case GLUT_KEY_RIGHT:
        //rota();                      /
            glutPostRedisplay();
        break;
    }
}
void display(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1000, 10.0, 650,-2000,1500);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(1.0, 1, 1.0, 1.0);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if(flag)
    {
        glPushMatrix();
        glTranslatef(-1.0,0.0,-3.5);
        glRotatef(xangle+25,1.0,0.0,0.0);
        glRotatef(yangle,0.0,1.0,0.0);
          glRotatef(zangle,0.0,0.0,1.0);
        road2();
        glPushMatrix();
        glTranslatef(0,00,-50);
        glPopMatrix();
        tree1();
        tree12();
        bus_move();
        glPopMatrix();
     }
      else
{
        road2();
        tree1();
        tree12();
        bus_move();

        flag55=1;
    }
    if(vari==1)
    {
        if(x==865)
            vari=0;
    }
    if(var==1)
    {    if(flag1)
    {
        glPushMatrix();
        glTranslatef(-1.0,0.0,-3.5);
        glRotatef(xangle+25,1.0,0.0,0.0);
        glRotatef(yangle,0.0,1.0,0.0);
          glRotatef(zangle,0.0,0.0,1.0);
        road2d();
        buildingd();
        walld();
        gated();
        treed();
        tree2d();
        bus_moved();
        glPopMatrix();
     }
      else
{
        road2d();
        buildingd();
        walld();
        gated();
        treed();
        tree2d();
        bus_moved();
        flag551=1;
       }
    }
    glFlush();
    glutSwapBuffers();

}

void myreshape(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(w<=h)
         glOrtho(-2.0,2.0,-2.0*(GLfloat)h/(GLfloat)w,2.0*(GLfloat)h/(GLfloat)w,-10.0,10.0);
    else
         glOrtho(-2.0*(GLfloat)w/(GLfloat)h,2.0*(GLfloat)w/(GLfloat)h,-2.0,2.0,-10.0,10.0);
    glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char **argv)
 {
       glutInit(&argc, argv);
       glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
       glutInitWindowSize(1000,600);
       glutInitWindowPosition(0,0);
       glutCreateWindow("BUS STOP");
       glutDisplayFunc(display);
       glutSpecialFunc( SpecialKeyFunc );
    glutReshapeFunc(myreshape);

       glutMainLoop();
return 1;
}
