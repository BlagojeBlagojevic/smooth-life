#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<graphics.h>


#define WIDTH 100  //SIRINA I VISINA PROZORA
#define HEIGHT 100



float GRID[WIDTH][HEIGHT]={0.0f};//za prikaz
float GRID_DIFF[WIDTH][HEIGHT]={0.0f};
//parametri
float ra = 21;
float alpha_n = 0.028;
float alpha_m = 0.147;
float b1 = 0.278;
float b2 = 0.365;
float d1 = 0.267;
float d2 = 0.445;
float dt = 0.05f;


//random inicijalizaciaja grida

float rand_float(void)
{
	return (float)rand()/(float)RAND_MAX; 
}

void rand_grid()  //RANDOM INICIJALIZACIJA PARAMETARA 
{
	size_t y,x;
	for(y=0;y<HEIGHT;y++)
	{
		for(x=0;x<WIDTH;x++)
		{
			GRID[y][x]=rand_float();
			//printf("%f ",GRID[y][x]);  //ZA PROVJERU
		}
		//printf("\n");
	}
}
// prikaz konzola
void display_grid(float grid[WIDTH][HEIGHT])
{
   	size_t y,x;
   	for(y=0;y<WIDTH;y++)
   	{
   		for(x=0;x<HEIGHT;x++) 
   		{
   			//level[(int)(grid[y][x]*(level_count-1))];//mozda bi trebalo drugcije
   				{
			   	if(grid[y][x]>0.99)
   					{
   						putpixel(x*2,y*2,GREEN);
					}
   				else if(grid[y][x]<0.01)
   				{
   				
				 	putpixel(x*2,y*2,RED);
				}
				else if(grid[y][x]>0.4f &&grid[y][x]<0.5f)
					{
						putpixel(x*2,y*2,YELLOW);
					}
				}
		}
	}
}

// funkcije iz https://arxiv.org/pdf/1111.1567.pdf

float sigma(float x, float a, float alpha)
{
    return 1.0f/(1.0f + expf(-(x - a)*4/alpha));  
	//s1(x, a) = 1/(1 + exp(-(x - a)4/a))   (3)
}

float sigma_n(float x, float a, float b)
{
    return sigma(x, a, alpha_n)*(1 - sigma(x, b, alpha_n)); 
	//s2(x, a, b) = s1(x, a)*(1 - s1(x, b))  (4)
}

float sigma_m(float x, float y, float m)
{
	//sm(x, y, m) = x(1 - s1(m, 0.5)) + y s1(m, 0.5) (5)

    return x*(1 - sigma(m, 0.5f, alpha_m)) + y*sigma(m, 0.5f, alpha_m);
}

float s(float n, float m)
{
    return sigma_n(n, sigma_m(b1, d1, m), sigma_m(b2, d2, m)); //valjda
    //s(n, m) = s2(n, sm(b1, d1, m), sm(b2, d2, m)) (6)

}

//u  ovom djelu da provjerimo da li se sistem nalazi u krugu(vise kao donat(disk ra=3ri)) ili je izvan
// posto gledamo kontinualno doajemo promjenu na grid za razliku od klasicne(0,1)
//f(x, t + 1) = S[s(n, m)] f(x, t) (7)
//f(x, t + dt) = f(x, t) + dt S[s(n, m)] f(x, t) (8)
// vrijeme diskretno(sampling) dt=0.0001 ili slicno
//s(n, m) = 2sd(n, m)-1 [-1,1]
int emod(int a, int b)   //pomocna za +- mod da radi gleda kao unsigned
{
    return (a%b + b)%b;
}

void compute_grid_diff(void)
{
    for (int i = 0; i < HEIGHT; i++) {   //prolazimo kroz grid
        for (int j = 0; j < WIDTH; j++) {
            float m = 0, M = 0;           //promjenive
            float n = 0, N = 0;
            float ri = ra/3;    //def pap

            for (int dy = -(ra - 1); dy <= (ra - 1); dy++) 
				{
                	for (int dx = -(ra - 1); dx <= (ra - 1); dx++) 
					{
                    	int x = emod(i + dx, WIDTH);
                    	int y = emod(j + dy, HEIGHT);
                    	if (dx*dx + dy*dy <= ri*ri) //jed kruga
							{
        	                	m += GRID[i][j];
            	            	M += 1;
                   		 	} 
							else if (dx*dx + dy*dy <= ra*ra) //jed kruga
								{  
                        			n += GRID[y][x];
                        			N += 1;
                   			 	}
                		}
            	}
            		m /= M;			//normalizacija
            		n /= N;
            		float q = s(n, m);
           			 GRID_DIFF[i][j] = 2*q - 1;
        }
    }
}
void clamp(float *x, float l, float h)
{
    if (*x < l) *x = l;   //DA OGRANICI VRJEDNOST IZMEDJU 0,1 PRIMA PO REF
    if (*x > h) *x = h;
}
void applay(void)
{
	for (size_t y = 0; y < HEIGHT; y++) 
		 {
        	for (size_t x = 0; x < WIDTH; x++) 
			{
            	GRID[y][x] += dt*GRID_DIFF[y][x]; //DODAJ dt*promjena
            	clamp(&GRID[y][x], 0, 1);
        	}
    	}
}
  
int main(void)
{
	srand(time(0));
	rand_grid();
	int gdriver = DETECT, gmode, errorcode;
	initwindow(WIDTH*2,HEIGHT*2);
	printf("COLORS:\nGREEN: ALIVE\nYELLOW: STRUGGLE\nRED:  DEATH\n\n\n\n");
	while(1)
	{
		display_grid(GRID);
		compute_grid_diff();
		applay();	
	}
	//printf("Nesto");
	return 0;
}
