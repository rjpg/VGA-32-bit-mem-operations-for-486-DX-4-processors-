#include <time.h>
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include "vga.cpp"

void setpal (void)
{
int i,x;
char col;
col=0;
x=16;
for (i=0;i<x;i++)   //preto
        {
        setregister(i,0,0,0);
        col+=3;
        }

col=0;
x+=16;
for (;i<x;i++)   //vermelho
        {
        setregister(i,col,0,0);
        col+=3;
        }

col=0;
x+=16;
for (;i<x;i++)   //verde
        {
        setregister(i,0,col,0);
        col+=3;
        }

col=0;
x+=16;
for (;i<x;i++)   //castanho
        {
        setregister(i,col,col,0);
        col+=3;
        }

col=0;
x+=16;
for (;i<x;i++)   //azul
        {
        setregister(i,0,0,col);
        col+=3;
        }

col=0;
x+=16;
for (;i<x;i++)   //cor de roza
        {
        setregister(i,col,0,col);
        col+=3;
        }

col=0;
x+=16;
for (;i<x;i++)   //magenta
        {
        setregister(i,0,col,col);
        col+=3;
        }


col=0;
x+=16;
for (;i<x;i++)   //branco
        {
        setregister(i,col,col,col);
        col+=4;
        }

col=0;
x+=16;
for (;i<x;i++)   //vermelho claro
        {
        setregister(i,col,0,0);
        col+=4;
        }

col=0;
x+=16;
for (;i<x;i++)   //verde claro
        {
        setregister(i,0,col,0);
        col+=4;
        }

col=0;
x+=16;
for (;i<x;i++)   //amarelo
        {
        setregister(i,col,col,0);
        col+=4;
        }

col=0;
x+=16;
for (;i<x;i++)   //azul claro
        {
        setregister(i,0,0,col);
        col+=4;
        }

col=0;
x+=16;
for (;i<x;i++)   //cor de roza claro
        {
        setregister(i,col,0,col);
        col+=4;
        }

col=0;
x+=16;
for (;i<x;i++)   //magenta claro
        {
        setregister(i,0,col,col);
        col+=4;
        }
}





void unload_map (text_map &texture)
{
delete texture.map;
}

text_map load_map (char name[]) //*.tga
{
int x,y,total;
text_map text;
FILE *Data=fopen (name,"rb");
int a[2];
fseek (Data,12,SEEK_SET);
fread (a,sizeof(int),2,Data);
text.X_size=a[0];
text.Y_size=a[1];
x=a[0];
y=a[1];
total=x*y;
fseek (Data,16+768,SEEK_SET);
text.map=new unsigned char[total];
fread ((byte*)text.map,1,total,Data);
fclose (Data);
long int j;
/*for (j=0;j<total;j++)
        text.map[j]=(text.map[j]<<4)+15;*/
return text;
}



void main (void)
{
/*        int fpx=1,fps=1,s[2],v;
	struct  time t;
        gettime(&t);*/


        text_map sonic;
        sonic=load_map ("sonic.tga");
        printf("sizeX:%X , sizeY:%X",sonic.X_size,sonic.Y_size);


        if (set_vga()!=NULL)
        {printf("\n set_vga() - ERROR\n");return;}

        if (ACTIVATE_3D()!=NULL)
        {printf("\n ACTIVATE() - ERROR\n");return;}

        setpal();

        triangle t1;
        // T-coord.      M-coord.
        t1.A=point (100,10);   t1.mA=point (0,0);
        t1.B=point (100,137);  t1.mB=point (0,sonic.Y_size-1);
        t1.C=point (163,137);  t1.mC=point (sonic.X_size,sonic.Y_size-1);

        t1.image=sonic; //texture
        t1.lightT=0xF; // luz recebida
        t1.color=4; // cor

        triangle t2;
        // T-coord.      M-coord.
        t2.A=point (100,10);   t2.mA=point (0,0);
        t2.B=point (163,10);   t2.mB=point (sonic.X_size,0);
        t2.C=point (163,137);  t2.mC=point (sonic.X_size,sonic.Y_size-1);

        t2.image=sonic; //texture
        t2.lightT=0xF;   // luz recebida no triangulo
        t2.color=4; // cor

        putwindow(sonic.map,sonic.X_size,sonic.Y_size,50,10);
        getchar();

char ch=0;
int i;
while (ch!='q')
{
       while (!kbhit())
	{

        back_clean(0x1F);
//        putwindowmem(VIRTUAL_SCREEN,320,200,
//                        sonic.map,sonic.X_size,sonic.Y_size,200,10);
        fill_flat_tex_triangle (t1);
        fill_flat_tex_triangle (t2);


        back_to_screen();

/*                gotoxy (1,23);
                printf ("FPS:%d\n",fpx);
                s[0]=t.ti_sec;;
		if (s[0] != s[1])
		{
			fpx=fps;
			fps=0;
		}
                s[1]=t.ti_sec;;
		gettime(&t);
		fps ++;
                gotoxy (1,22);
                printf ("image:%d\n",fps);*/
                
        }

        ch=getch();
        switch (ch)
		{
                case '5' : t1.A.y++;
                           t2.A.y++; break;
                case '8' : t1.A.y--;
                           t2.A.y--; break;
                case '4' : t1.A.x--;
                           t2.A.x--; break;
                case '6' : t1.A.x++;
                           t2.A.x++; break;
                case '+' : t2.lightT=++t1.lightT; break;
                }
}

        getchar();
        unload_map(sonic);

        if (set_text()!=NULL)                  //set_text chama por si DESACTIVATE_3D
                printf ("nÆo estava activado VGA");

}
