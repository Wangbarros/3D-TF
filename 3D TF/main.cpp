// Nova Iteracao.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define PI 3.14159265

struct  malha {
    double orient;   // graus em rad
    double orient2;
    double orient3;
    double Sresfr;
    double CXrelativo;
    double CYrelativo;
    double CZrelativo;
    double L;
    double fst;
    double Sresfr_crit;
    int ativado; };

double Abs (double N);
double Vcresc (struct malha *Celula);
double Tamanho (double velocidade, double Dt);
void Vertices (struct malha *Celula, double V1[], double V2[], double V3[], double V4[], double V5[], double V6[]);
double distancia (struct malha *Celula, double V[]);
double Truncado (double Lado);
void Calpha (struct malha *Celula, struct malha *Vizinho, double V1[], double V2[], double V3[], double V4[], double V5[], double V6[], double alpha[]);
void nucleacao (double alpha[], struct malha *Celula, double(*Abs)(double), double(*Truncado)(double), double(*distancia)(struct malha *Celula, double A[]), double V1[], double V2[], double V3[], double V4[], double V5[], double V6[], struct malha *Vizinho);
double formacao(double deltaT, double Tsigma, double Tnuc);
double erf(double x);
int RandCentro(int linha,int coluna);
int RandParede(int linha, int coluna);
double round(double n);
double RandOrient();
//Programa principal------------------------------------------
//#pragma argsused
int main(int argc, char* argv[])
{
    int i;
	int linha,coluna, plano, maxlinha, maxcoluna, maxplano;
	struct malha *Celula;
	//malha *Celula = (malha*) malloc (sizeof(malha));
	int CAi, VFi, CAj, VFj, CAz, VFz;
	double tamanhox, tamanhoy, tamanhoz;
	double orientacao, ativado;
	double DxCA, DyCA, DzCA, Dtempo, tempo;
	double Velocidade, G;
	double Ox, Oy, Oz, Lc;
	double V1[3], V2[3], V3[3], V4[3], V5[3], V6[3], alpha[6], T;
	double nmax0, nmax1, linear, area;
	double b, formados_parede, formados_centro, total_parede, e_anterior_parede, e_anterior_centro, total_centro, e;
	int numeroCelula, sorteado_centro, sorteado_parede, sorteado_orient;
	int arquivo;
	char nome[100], nome2[100];
	double xpos,ypos,
    x1,x2,x3,x4,
    y1,y2,y3,y4,
    xa1,xa2,
    c1,c2,c3;
    FILE *filePointer;
    arquivo = 1;
    DxCA = (3.0)/10000;  // = 50um = 5 *10^6 densidade = 400*10^6 densidade 1600*10^6 = 25/10^6 = 25um  multiplica-se por 2 para o DxCA e DyCA
    DyCA = (3.0)/10000;
    DzCA = (3.0)/10000;
    tamanhox = (3.0/100);
    tamanhoy = (3.0/100);
    tamanhoz = (3.0)/100;
	CAi = 10;
	CAj = 10;
    CAz = 10;
	VFi = 10;
	VFj = 10;
    VFz = 10;


	tempo = 0;
	Dtempo = 0.01; //Passo de tempo
	linha = 100;
	coluna = 100;
    plano = 10;
    maxlinha = linha;
    maxcoluna = coluna;
    maxplano = plano;
	G = 0;
	Celula = (struct malha *) malloc ( ((plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)*sizeof(struct malha));
	//Condicoes iniciais de todas as celulas
    for(plano=0;plano<(maxplano);plano++){
    for(linha=0;linha<(maxlinha);linha++){
        for(coluna=0;coluna<(maxcoluna);coluna++){
            //CondiÁoes iniciais
             //   ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr = 0.5+(G*DyCA*((300-linha)/2));
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr = 10.0 +(G*DzCA*((maxplano-plano)));
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->orient2 = 0;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->orient3 = 0.27;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->L = 0;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->fst = 0;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->CXrelativo = coluna*DxCA;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->CYrelativo = linha*DyCA;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->CZrelativo = plano*DzCA;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->ativado = 0;
                ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr_crit = 0;
                ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))-> orient = 2.0;
        }}};

	//Nucleacao da celula central
	((Celula + 44949) -> orient) = (0.26); //0.524 = 30graus 60 = 1.047197550 0.2618 para 15
	((Celula + 44949) -> ativado) = 1.0;
    while (tempo<10){
        //Inicio do programa de crescimento
    for(plano=0;plano<maxplano;plano++){
        for(linha=0;linha<(maxlinha);linha++){
            for(coluna=0;coluna<(maxcoluna);coluna++){

                numeroCelula = (plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna);

                //Ativa a celula se atingido o Sresfr minimo
     /*           if ( ((((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->orient) !=2.0) && ((((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr_crit)<(((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr))){
                    (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->ativado) = 1.0;} */

                //Caso todas as celulas ao redor da selecionada j· estejam nucleadas. A celula selecionada È desativada.
                if ( (linha<(maxlinha-1))&&(linha>0)&&(coluna>0)&&(coluna<(maxcoluna-1)) && (plano>0) && (plano<maxplano-1) ){

                    if (  ((((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))->orient) !=2.0)){
                        if (  ((((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))->orient) !=2.0)){
                          	if (  ((((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))->orient) !=2.0)){
                                if (  ((((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))->orient) !=2.0)){
                                    if(  ((((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))->orient) !=2.0)){
                                        if(  ((((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))->orient) !=2.0)){
                                    (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->orient) = 3.0;
                                    (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->ativado) = 0;
                                        }}}}}}}

               //Arrumar as celulas de canto!


                (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr) =  (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr)+(0.1*Dtempo); //Super resfriamento sem considerar o campo

               // ativado = ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->ativado;


                if (  ((((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->orient) !=2.0)&&((((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->ativado) ==1.0)  ) {

                    numeroCelula = (plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna);


                 //   T = (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->Sresfr);

                    Velocidade = Vcresc (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)));

                    (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->L) =(((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->L)+ (Tamanho (Velocidade, Dtempo)) ;

               //     Ox = (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->CXrelativo);
               //     Oy = (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->CYrelativo);
               //     Oz = (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->CZrelativo);
               //     Lc = (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->L);
               //     orientacao = (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna))->orient);


                    Vertices (((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), V1, V2, V3, V4, V5, V6);

                    //Nucleacao da parede da direita
                    if ( (coluna==(maxcoluna-1))&&(linha!=0)&&(linha!=(maxlinha-1)) ){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1))),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }

                    //Nucleacao da parede da esquerda
                    if ( (coluna ==0) && (linha!=0) && (linha!=maxlinha-1) ){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1))),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }

                    //Nucleacao da parede de baixo
                    if ( (linha==maxlinha-1) && (coluna!=0) &&(coluna !=(maxcoluna-1)) ){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1))),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1))),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }

                    //Nucleacao da parede de cima
                    if ( (linha==0) && (coluna!=0)&& (coluna !=(maxcoluna-1)) ){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1))),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1))),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }
                    //Nucleacao do centro
                    if ( (coluna>0) && (linha>0) && (coluna<maxcoluna) && (linha<maxlinha) ){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1))),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1))),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }

                    }
                    //Nucleacao das celulas de canto
                    if ((coluna==0) && (linha==0)) {

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1))),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6,alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }

                    //Celula "99"
                    if (coluna==(maxcoluna-1)&&(linha==0)){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1))),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha+1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }
                    //Celula "9999"
                    if ( (coluna==(maxcoluna-1))&&(linha==(maxlinha-1)) ){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1))),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna-1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }
                    //Celula de Maior valor
                    if ((coluna==0) && (linha==maxlinha-1)){

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1))),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + (coluna+1)))  );

                        Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                        nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6,((Celula+(plano*maxlinha*maxcoluna)+((linha-1)*maxcoluna) + coluna))  );

                        if (plano>0 && plano<(maxplano-1)){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==0){
                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano+1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                        if (plano==(maxplano-1)){

                            Calpha( ((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna)),V1, V2, V3, V4, V5, V6, alpha);
                            nucleacao(alpha,((Celula+(plano*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna)), Abs, Truncado, distancia, V1, V2, V3, V4, V5, V6, ((Celula+((plano-1)*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))  );
                        }
                    }
                }}}}


        tempo = tempo+Dtempo;

    }
    //Aqui abaxo È a impressao do txt
    i = 0;
    for(i = 0; i<(maxplano); i++){
	sprintf(nome,"/Users/chenwang/Dropbox/TF/3D TF/macro%i.txt",i);
    filePointer = fopen(nome,"w");

    fprintf(filePointer, "set (gca,'PlotBoxAspectRatio',[1 1 1],'Xlim',"); // Aspecto do grafico (isso È um retangulo em que a altura È 3 vezes maior que a base)
    fprintf(filePointer, "[0,%g],'Ylim',[0,%g])\n",tamanhox,tamanhoy); // Tamanho do grafico que vc vai abrir
    fprintf(filePointer, "hold on\n" );
    fprintf(filePointer, "title('Malha(%dX%d) Dt(%g s) tempo(%g s)') ", // Titulo do grafico
            (maxcoluna),(maxlinha),Dtempo,tempo);
    fprintf(filePointer, "\nhold on\n");
    fprintf(filePointer, "ylabel('metros') \nhold on \n"); // Nomes dos eixos
    fprintf(filePointer, "xlabel('metros') \nhold on \n");


    for(linha=0;linha<(maxlinha);linha++){
        for(coluna=0;coluna<(maxcoluna);coluna++){

            if ( (((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) != 2.0)&&(((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) != 3.0)  ){ //2.0 È uma celula n„o nucleada. Essa funcao acaba no fill.

                xpos=(DxCA/2.0) + coluna*DxCA; //DxCA È a distancia entre celulas do automato celular em x
                ypos= tamanhoy - (DyCA/2.0) - (linha*DyCA); //DyCA È a distancia entre celulas do automato celular em y

                x1=xpos+(DxCA/2.0); //x1=x2 entao /*[x1 y1] [x2 y2] [x3 y3] [x4 y4] //Olhe no final do codigo (no fill para entender melhor)
                x2=xpos+(DxCA/2.0);
                x3=xpos-(DxCA/2.0);
                x4=xpos-(DxCA/2.0); //x3=x4
                y1=-(ypos+(DyCA/2.0)); //y1=y4
                y2=-(ypos-(DyCA/2.0));
                y3=-(ypos-(DyCA/2.0)); //y2=y3
                y4=-(ypos+(DyCA/2.0));

                // substituindo fica [x1 y1] [x1 y2] [x3 y2] [x3 y1] forma um quadrado com bases x1 x3 e altura y1 y2 na hora do fill
                // Sendo a diferenÁa entre x1 e x3 1.0DxCA e entre y1 e y2 1.0DyCA
                // Esse programa preenche da esquerda pra direita e de cima para baixo (assim como se le um texto)
                if ( coluna != ((maxcoluna)-1) &&
                    ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) ==
                    ((Celula+((linha*maxcoluna) + coluna + 1) ) -> orient)     ){
                    xa1=x1;
                    xa2=x2;

                    for (coluna=(coluna+1);coluna<(maxcoluna);coluna++){
                        xa1=xa1+DxCA;
                        xa2=xa2+DxCA;

                        if ( coluna == ((maxcoluna)-1) ||
                            ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) !=
                            ((Celula+((linha*maxcoluna) + coluna + 1) ) -> orient)   ){
                            x1=xa1;
                            x2=xa2;
                            xa1=0.0;
                            xa2=0.0;
                            break;
                        }
                    }
                }

                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -1.0 ){
                    c1=0.8; //c1,c2,c3 È uma codificaÁao para a cor a ser usada no quadrado de orientaÁao X
                    c2=0.1;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.76 ){
                    c1=0.2;
                    c2=0.1;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.73 ){
                    c1=0.2;
                    c2=0.1;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.69 ){
                    c1=0.2;
                    c2=0.3;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.67 ){
                    c1=0.2;
                    c2=0.3;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.635 ){
                    c1=0.2;
                    c2=0.3;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.605 ){
                    c1=0.2;
                    c2=0.3;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.57 ){
                    c1=0.2;
                    c2=0.5;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.53 ){
                    c1=0.2;
                    c2=0.5;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.505 ){
                    c1=0.2;
                    c2=0.5;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.47 ){
                    c1=0.2;
                    c2=0.5;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.435 ){
                    c1=0.2;
                    c2=0.7;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.405 ){
                    c1=0.2;
                    c2=0.7;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.375 ){
                    c1=0.2;
                    c2=0.7;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.335 ){
                    c1=0.2;
                    c2=0.7;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.305 ){
                    c1=0.4;
                    c2=0.1;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.275 ){
                    c1=0.4;
                    c2=0.1;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.24 ){
                    c1=0.4;
                    c2=0.1;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.205 ){
                    c1=0.4;
                    c2=0.1;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.175 ){
                    c1=0.4;
                    c2=0.3;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.145 ){
                    c1=0.4;
                    c2=0.3;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.115 ){
                    c1=0.4;
                    c2=0.3;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.08 ){
                    c1=0.4;
                    c2=0.3;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.045 ){
                    c1=0.4;
                    c2=0.5;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > -0.015 ){
                    c1=0.4;
                    c2=0.5;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.015 ){
                    c1=0.4;
                    c2=0.5;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.045 ){
                    c1=0.4;
                    c2=0.5;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.08 ){
                    c1=0.4;
                    c2=0.7;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.115 ){
                    c1=0.4;
                    c2=0.7;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.145 ){
                    c1=0.4;
                    c2=0.7;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.175 ){
                    c1=0.4;
                    c2=0.7;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.205 ){
                    c1=0.6;
                    c2=0.1;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.24 ){
                    c1=0.6;
                    c2=0.1;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.275 ){
                    c1=0.6;
                    c2=0.1;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.305 ){
                    c1=0.6;
                    c2=0.1;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.335 ){
                    c1=0.6;
                    c2=0.3;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.375 ){
                    c1=0.6;
                    c2=0.3;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.405 ){
                    c1=0.6;
                    c2=0.3;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.435 ){
                    c1=0.6;
                    c2=0.3;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.47 ){
                    c1=0.6;
                    c2=0.5;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.505 ){
                    c1=0.6;
                    c2=0.5;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.53 ){
                    c1=0.6;
                    c2=0.5;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.57 ){
                    c1=0.6;
                    c2=0.5;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.605 ){
                    c1=0.6;
                    c2=0.7;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.635 ){
                    c1=0.6;
                    c2=0.7;
                    c3=0.4;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.67 ){
                    c1=0.6;
                    c2=0.7;
                    c3=0.6;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.69 ){
                    c1=0.6;
                    c2=0.7;
                    c3=0.8;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.73 ){
                    c1=0.8;
                    c2=0.1;
                    c3=0.2;
                }
                if ( ((Celula+((i*maxcoluna*maxlinha)+(linha*maxcoluna) + coluna) ) -> orient) > 0.765 ){
                    c1=0.8;
                    c2=0.1;
                    c3=0.4;
                }




                fprintf(filePointer, "fill([%g %g %g %g],[%g %g %g %g],[%g %g %g],'LineStyle','none')\n",x1,x2,x3,x4,y1,y2,y3,y4,c1,c2,c3);

            }

                // Isso aqui ta pintando quadrados
                // Sao pares de pontos
                // Por exemplo [1 3 4 2] [5 6 8 5]
                // Esse fara dois pares de pontos em [1 5], [3 6] e fazer uma linha ligando estes
                // E outro par de pontos [4 8] [2 5] e uma linha ligando esses dois
                //
            }
        }
    }
    sprintf(nome2,"tecplot.txt");
    filePointer = fopen(nome2,"w");
    fprintf(filePointer, "TITLE     = \"Phase Field 3D - Template\"\n");
    fprintf(filePointer, "VARIABLES = \"x\"\n");
    fprintf(filePointer, "\"y\"\n");
    fprintf(filePointer, "\"z\"\n");
    fprintf(filePointer, "\"<greek>f</greek>\"\n");
    fprintf(filePointer, "\"T\"\n");
    fprintf(filePointer, "TEXT\n");
    fprintf(filePointer, "CS=FRAME\n");
    fprintf(filePointer, "X=19.4778098147,Y=94.7994180807\n");
    fprintf(filePointer, "C=BLACK\n");
    fprintf(filePointer, "S=LOCAL\n");
    fprintf(filePointer, "HU=POINT\n");
    fprintf(filePointer, "LS=1 AN=LEFT\n");
    fprintf(filePointer, "BXM=20 LT=0.1 BXO=BLACK BXF=WHITE\n");
    fprintf(filePointer, "F=HELV-BOLD\n");
    fprintf(filePointer, "H=11 A=0\n");
    fprintf(filePointer, "MFC=\"\"\n");
    fprintf(filePointer, "CLIPPING=CLIPTOVIEWPORT\n");
    fprintf(filePointer, "T=\"T\"\n");
    fprintf(filePointer, "TEXT\n");
    fprintf(filePointer, "CS=FRAME\n");
    fprintf(filePointer, "X=20.6222846152,Y=13.6575451228\n");
    fprintf(filePointer, "C=BLACK\n");
    fprintf(filePointer, "S=LOCAL\n");
    fprintf(filePointer, "HU=POINT\n");
    fprintf(filePointer, "LS=1 AN=LEFT\n");
    fprintf(filePointer, "BXM=20 LT=0.1 BXO=BLACK BXF=WHITE\n");
    fprintf(filePointer, "F=HELV-BOLD\n");
    fprintf(filePointer, "H=11 A=0\n");
    fprintf(filePointer, "MFC=\"\"\n");
    fprintf(filePointer, "CLIPPING=CLIPTOVIEWPORT\n");
    fprintf(filePointer, "T=\"t* = ???\"\n");
    fprintf(filePointer, "ZONE T=\"Phase Field 3D\"\n");
    fprintf(filePointer, " STRANDID=0, SOLUTIONTIME=0\n");
    fprintf(filePointer, " I=100, J=100, K=100, ZONETYPE=Ordered\n");
    fprintf(filePointer, " DATAPACKING=POINT\n");
    fprintf(filePointer, " DT=(SINGLE SINGLE SINGLE SINGLE SINGLE )\n");

    for(plano=0;plano<(maxplano);plano++){
        for(linha=0;linha<(maxlinha);linha++){
            for(coluna=0;coluna<(maxcoluna);coluna++){
                fprintf(filePointer, "%d    %d    %d    %.4f    %.4f\n",coluna, linha, plano, ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))-> orient, ((Celula+(plano*maxlinha*maxcoluna)+(linha*maxcoluna) + coluna))-> orient);
            }}}



    fclose (filePointer);


    printf("Acabou");

    free(Celula);
    return 0;
}

//Final Programa principal----------------------------------

double Abs (double N){
    if (N<0){
        N = (0-N);}
    if (N>0){
        N = N; }
    if (N==0){
        N = N;}
    return N;}

double Vcresc (struct malha *Celula){
	double a2;
	double a3;
	double V;
	double Sresfr;
	Sresfr = (Celula->Sresfr);
	a2 = ((1.0)/1000000);  //8.26*10^-6 ou 2.9/10^6 para 7%Si ou 10^-4
	a3 = ((0.0)/1000000); //8.18/10^5  ou 1.49/10^6 ou 0

	V = (a2*Sresfr*Sresfr) + (a3*Sresfr*Sresfr*Sresfr);
	return V;
}

double Tamanho (double velocidade, double Dt) {

    double Tamanho;

    Tamanho = velocidade*Dt;
    Tamanho = (Tamanho);

    return Tamanho; }

void Vertices (struct malha *Celula, double V1[], double V2[], double V3[], double V4[], double V5[], double V6[]){
    double seno, seno2, seno3;
    double cosseno, cosseno2, cosseno3, A[10];
    double V10, V11, V12, V20, V21, V22, V30, V31, V32, V40, V41, V42, V50, V51, V52, V60, V61, V62;

    //V1 ao V4 sao os verticies no "quadrado" V5 e V6 sao os verticies de cima e de baixo

    seno = sin(Celula->orient); //psi
    cosseno = cos(Celula->orient);
    seno2 = sin(Celula->orient2); //phi
    cosseno2 = cos(Celula->orient2);
    seno3 = sin(Celula->orient3); //theta
    cosseno3 = cos(Celula->orient3);

    seno = Abs(seno);
    cosseno = Abs(cosseno);
    seno2 = Abs(seno2);
    cosseno2 = Abs(cosseno2);
    seno3 = Abs(seno3);
    cosseno3 = Abs(cosseno3);

    V10 = 0 + ((Celula -> L));
    V11 = 0;// + ((Celula -> L));
    V12 = 0;

    V20 = 0 - ((Celula -> L));
    V21 = 0;// + ((Celula -> L));
    V22 = 0;

    V30 = 0;// - ((Celula -> L));
    V31 = 0 + ((Celula -> L));
    V32 = 0;

    V40 = 0;// + ((Celula -> L));
    V41 = 0 - ((Celula -> L)); //- ((Celula -> L));
    V42 = 0;

    V50 = 0;
    V51 = 0;
    V52 = 0 + (Celula -> L);


    V60 = 0;
    V61 = 0;
    V62 = 0 - (Celula -> L);

    A[0] = cosseno3*cosseno;
    A[1] = cosseno2*seno + seno2*seno3*cosseno;
    A[2] = seno2*seno - cosseno2*seno3*cosseno;

    A[3] = -cosseno3*seno;
    A[4] = cosseno2*cosseno - seno2*seno3*seno;
    A[5] = seno2*cosseno + cosseno2*seno3*seno;

    A[6] = seno3;
    A[7] = -seno2*cosseno3;
    A[8] = cosseno2*cosseno3;

    V1[0] = (V10*A[0]) + (V11*A[1]) + (V12*A[2]) + (Celula -> CXrelativo);
    V1[1] = (V10*A[3]) + (V11*A[4]) + (V12*A[5]) + (Celula -> CYrelativo);
    V1[2] = (V10*A[6]) + (V11*A[7]) + (V12*A[8]) + (Celula -> CZrelativo);

    V2[0] = (V20*A[0]) + (V21*A[1]) + (V22*A[2]) + (Celula -> CXrelativo);
    V2[1] = (V20*A[3]) + (V21*A[4]) + (V22*A[5]) + (Celula -> CYrelativo);
    V2[2] = (V20*A[6]) + (V21*A[7]) + (V22*A[8]) + (Celula -> CZrelativo);

    V3[0] = (V30*A[0]) + (V31*A[1]) + (V32*A[2]) + (Celula -> CXrelativo);
    V3[1] = (V30*A[3]) + (V31*A[4]) + (V32*A[5]) + (Celula -> CYrelativo);
    V3[2] = (V30*A[6]) + (V31*A[7]) + (V32*A[8]) + (Celula -> CZrelativo);

    V4[0] = (V40*A[0]) + (V41*A[1]) + (V42*A[2]) + (Celula -> CXrelativo);
    V4[1] = (V40*A[3]) + (V41*A[4]) + (V42*A[5]) + (Celula -> CYrelativo);
    V4[2] = (V40*A[6]) + (V41*A[7]) + (V42*A[8]) + (Celula -> CZrelativo);

    V5[0] = (V50*A[0]) + (V51*A[1]) + (V52*A[2]) + (Celula -> CXrelativo);
    V5[1] = (V50*A[3]) + (V51*A[4]) + (V52*A[5]) + (Celula -> CYrelativo);
    V5[2] = (V50*A[6]) + (V51*A[7]) + (V52*A[8]) + (Celula -> CZrelativo);

    V6[0] = (V60*A[0]) + (V61*A[1]) + (V62*A[2]) + (Celula -> CXrelativo);
    V6[1] = (V60*A[3]) + (V61*A[4]) + (V62*A[5]) + (Celula -> CYrelativo);
    V6[2] = (V60*A[6]) + (V61*A[7]) + (V62*A[8]) + (Celula -> CZrelativo);

}
double distancia (struct malha *Celula, double A[]){
    double dist;
    double Dx, Dy, Dz;

    Dx = (Celula->CXrelativo) - A[0];

    Dx = Dx*Dx;

    Dy = (Celula->CYrelativo) - A[1];

    Dy = Dy*Dy;

    Dz = (Celula->CZrelativo) - A[2];

    Dz = Dz*Dz;

    dist = Dx + Dy + Dz;
    dist = sqrt(dist);

    return dist;}

double Truncado (double Lado) {
    double aLado;
    double NovoLado;
    double c;
    double d;
    double l;
    aLado = Lado;
    c = aLado/2;
    d = aLado/2;
    l = (3.0)/10000; //EspaÁo entre dois centros de duas celulas
    l = l*(sqrt(3.0));
    if (l<c){
        c = l;}
    if (l<d) {
        d = l;}
    NovoLado = (sqrt(2.0)*(c+d)/(sqrt(3.0)));
    NovoLado = NovoLado/2;

    return NovoLado;
}
void Calpha (struct malha *Celula, struct malha *Vizinho, double V1[], double V2[], double V3[], double V4[], double V5[], double V6[], double alpha[]){
	double L1[3], L2[3], L3[3], L4[3], L5[3], L6[3], dR[3];
	int i;

	L1[0] = V1[0]-(Celula->CXrelativo);
	L1[1] = V1[1]-(Celula->CYrelativo);
    L1[2] = V1[2]-(Celula->CZrelativo);

	L2[0] = V2[0]-(Celula->CXrelativo);
	L2[1] = V2[1]-(Celula->CYrelativo);
    L2[2] = V2[2]-(Celula->CZrelativo);

	L3[0] = V3[0]-(Celula->CXrelativo);
	L3[1] = V3[1]-(Celula->CYrelativo);
    L3[2] = V3[2]-(Celula->CZrelativo);

	L4[0] = V4[0]-(Celula->CXrelativo);
	L4[1] = V4[1]-(Celula->CYrelativo);
    L4[2] = V4[2]-(Celula->CZrelativo);

    L5[0] = V5[0]-(Celula->CXrelativo);
	L5[1] = V5[1]-(Celula->CYrelativo);
    L5[2] = V5[2]-(Celula->CZrelativo);

    L6[0] = V6[0]-(Celula->CXrelativo);
	L6[1] = V6[1]-(Celula->CYrelativo);
    L6[2] = V6[2]-(Celula->CZrelativo);

	dR[0] = (Vizinho->CXrelativo)-(Celula->CXrelativo);
	dR[1] = (Vizinho->CYrelativo)-(Celula->CYrelativo);
    dR[2] = (Vizinho->CZrelativo)-(Celula->CZrelativo);

    alpha[0] = (dR[0]*L1[0]+dR[1]*L1[1]+dR[2]*L1[2])/(L1[0]*L1[0]+L1[1]*L1[1]+L1[2]*L1[2]);
    alpha[1] = (dR[0]*L2[0]+dR[1]*L2[1]+dR[2]*L2[2])/(L2[0]*L2[0]+L2[1]*L2[1]+L2[2]*L2[2]);
    alpha[2] = (dR[0]*L3[0]+dR[1]*L3[1]+dR[2]*L3[2])/(L3[0]*L3[0]+L3[1]*L3[1]+L3[2]*L3[2]);
    alpha[3] = (dR[0]*L4[0]+dR[1]*L4[1]+dR[2]*L4[2])/(L4[0]*L4[0]+L4[1]*L4[1]+L4[2]*L4[2]);
    alpha[4] = (dR[0]*L5[0]+dR[1]*L5[1]+dR[2]*L5[2])/(L5[0]*L5[0]+L5[1]*L5[1]+L5[2]*L5[2]);
    alpha[5] = (dR[0]*L6[0]+dR[1]*L6[1]+dR[2]*L6[2])/(L6[0]*L6[0]+L6[1]*L6[1]+L6[2]*L6[2]);

    for (i=0;i<6;i++){
        if (alpha[i]<0){
            alpha[i] =2;}

    }
}

void nucleacao (double alpha[], struct malha *Celula, double(*Abs)(double), double(*Truncado)(double), double(*distancia)(struct malha *Celula, double A[]), double V1[], double V2[], double V3[], double V4[], double V5[], double V6[], struct malha *Vizinho){
	double Decentrado, dist[6], Salpha, dR[3], Lv[3], Lu[3];

	dist[0] = distancia(Vizinho,V1);
	dist[1] = distancia(Vizinho,V2);
	dist[2] = distancia(Vizinho,V3);
	dist[3] = distancia(Vizinho,V4);
    dist[4] = distancia(Vizinho,V5);
    dist[5] = distancia(Vizinho,V6);

    Salpha = 2.0;
    //Combinacao de distancias. 4 em grupos de 2 tem total 6. 6 em grupos de 3 tem total 20.
    //{0,1} {0,2} {0,3} {1,2} {1,3} {2,3} para 2D

    //{0,1,2}, {0,1,3}, {0,1,4}, {0,1,5}, {0,2,3}, {0,2,4}, {0,2,5}, {0,3,4}, {0,3,5}, {0,4,5}, {1,2,3}, {1,2,4}, {1,2,5}, {1,3,4}, {1,3,5}, {1,4,5}, {2,3,4}, {2,3,5}, {2,4,5}, {3,4,5} sets a serem feitos. Estamos tentando achar os 3 vertices mais proximos do Vizinho.

	if (dist[0]<dist[3] && dist[0]<dist[4] && dist[0]<dist[5] && dist[1]<dist[3] && dist[1]<dist[4] && dist[1]<dist[5] && dist[2]<dist[3] && dist[2]<dist[4] && dist[2]<dist[5]){

		Salpha = alpha[0] + alpha[1] + alpha[2];} //Combinacao de 0,1,2

	if (dist[0]<dist[2] && dist[0]<dist[4] && dist[0]<dist[5] && dist[1]<dist[2] && dist[1]<dist[4] && dist[1]<dist[5] && dist[3]<dist[2] && dist[3]<dist[4] && dist[3]<dist[5]){

		Salpha = alpha[0] + alpha[1] + alpha[3];} //Combinacao de 0,1,3

    if (dist[0]<dist[2] && dist[0]<dist[3] && dist[0]<dist[5] && dist[1]<dist[2] && dist[1]<dist[3] && dist[1]<dist[5] && dist[4]<dist[2] && dist[4]<dist[3] && dist[4]<dist[5]){

		Salpha = alpha[0] + alpha[1] + alpha[4];} //Combinacao de 0,1,4

    if (dist[0]<dist[2] && dist[0]<dist[3] && dist[0]<dist[4] && dist[1]<dist[2] && dist[1]<dist[3] && dist[1]<dist[4] && dist[5]<dist[2] && dist[5]<dist[3] && dist[5]<dist[4]){

		Salpha = alpha[0] + alpha[1] + alpha[5];} //Combinacao de 0,1,5

    if (dist[0]<dist[1] && dist[0]<dist[4] && dist[0]<dist[5] && dist[2]<dist[1] && dist[2]<dist[4] && dist[2]<dist[5] && dist[3]<dist[1] && dist[3]<dist[4] && dist[3]<dist[5]){

		Salpha = alpha[0] + alpha[2] + alpha[3];} //Combinacao de 0,2,3

    if (dist[0]<dist[1] && dist[0]<dist[3] && dist[0]<dist[5] && dist[2]<dist[1] && dist[2]<dist[3] && dist[2]<dist[5] && dist[4]<dist[1] && dist[4]<dist[3] && dist[4]<dist[5]){

		Salpha = alpha[0] + alpha[2] + alpha[4];} //Combinacao de 0,2,4

    if (dist[0]<dist[1] && dist[0]<dist[3] && dist[0]<dist[4] && dist[2]<dist[1] && dist[2]<dist[3] && dist[2]<dist[4] && dist[5]<dist[1] && dist[5]<dist[3] && dist[5]<dist[4]){

		Salpha = alpha[0] + alpha[2] + alpha[5];} //Combinacao de 0,2,5

    if (dist[0]<dist[1] && dist[0]<dist[2] && dist[0]<dist[5] && dist[3]<dist[1] && dist[3]<dist[2] && dist[3]<dist[5] && dist[4]<dist[1] && dist[4]<dist[2] && dist[4]<dist[5]){

		Salpha = alpha[0] + alpha[3] + alpha[4];} //Combinacao de 0,3,4

    if (dist[0]<dist[1] && dist[0]<dist[2] && dist[0]<dist[4] && dist[3]<dist[1] && dist[3]<dist[2] && dist[3]<dist[4] && dist[5]<dist[1] && dist[5]<dist[2] && dist[5]<dist[4]){

		Salpha = alpha[0] + alpha[3] + alpha[5];} //Combinacao de 0,3,5

    if (dist[0]<dist[1] && dist[0]<dist[2] && dist[0]<dist[3] && dist[4]<dist[1] && dist[4]<dist[2] && dist[4]<dist[3] && dist[5]<dist[1] && dist[5]<dist[2] && dist[5]<dist[3]){

		Salpha = alpha[0] + alpha[4] + alpha[5];} //Combinacao de 0,4,5

    if (dist[1]<dist[0] && dist[1]<dist[4] && dist[1]<dist[5] && dist[2]<dist[0] && dist[2]<dist[4] && dist[2]<dist[5] && dist[3]<dist[0] && dist[3]<dist[4] && dist[3]<dist[5]){

		Salpha = alpha[1] + alpha[2] + alpha[3];} //Combinacao de 1,2,3

    if (dist[1]<dist[0] && dist[1]<dist[3] && dist[1]<dist[5] && dist[2]<dist[0] && dist[2]<dist[3] && dist[2]<dist[5] && dist[4]<dist[0] && dist[4]<dist[3] && dist[4]<dist[5]){

		Salpha = alpha[1] + alpha[2] + alpha[4];} //Combinacao de 1,2,4

    if (dist[1]<dist[0] && dist[1]<dist[3] && dist[1]<dist[4] && dist[2]<dist[0] && dist[2]<dist[3] && dist[2]<dist[4] && dist[5]<dist[0] && dist[5]<dist[3] && dist[5]<dist[4]){

		Salpha = alpha[1] + alpha[2] + alpha[5];} //Combinacao de 1,2,5

    if (dist[1]<dist[0] && dist[1]<dist[2] && dist[1]<dist[5] && dist[3]<dist[0] && dist[3]<dist[2] && dist[3]<dist[5] && dist[4]<dist[0] && dist[4]<dist[2] && dist[4]<dist[5]){

		Salpha = alpha[1] + alpha[3] + alpha[4];} //Combinacao de 1,3,4

    if (dist[1]<dist[0] && dist[1]<dist[2] && dist[1]<dist[4] && dist[3]<dist[0] && dist[3]<dist[2] && dist[3]<dist[4] && dist[5]<dist[0] && dist[5]<dist[2] && dist[5]<dist[4]){

		Salpha = alpha[1] + alpha[3] + alpha[5];} //Combinacao de 1,3,5

    if (dist[1]<dist[0] && dist[1]<dist[2] && dist[1]<dist[3] && dist[4]<dist[0] && dist[4]<dist[2] && dist[4]<dist[3] && dist[5]<dist[0] && dist[5]<dist[2] && dist[5]<dist[3]){

		Salpha = alpha[1] + alpha[4] + alpha[5];} //Combinacao de 1,4,5

    if (dist[2]<dist[0] && dist[2]<dist[1] && dist[2]<dist[5] && dist[3]<dist[0] && dist[3]<dist[1] && dist[3]<dist[5] && dist[4]<dist[0] && dist[4]<dist[1] && dist[4]<dist[5]){

		Salpha = alpha[2] + alpha[3] + alpha[4];} //Combinacao de 2,3,4

    if (dist[2]<dist[0] && dist[2]<dist[1] && dist[2]<dist[4] && dist[3]<dist[0] && dist[3]<dist[1] && dist[3]<dist[4] && dist[5]<dist[0] && dist[5]<dist[1] && dist[5]<dist[4]){

		Salpha = alpha[2] + alpha[3] + alpha[5];} //Combinacao de 2,3,5

    if (dist[2]<dist[0] && dist[2]<dist[1] && dist[2]<dist[3] && dist[4]<dist[0] && dist[4]<dist[1] && dist[4]<dist[3] && dist[5]<dist[0] && dist[5]<dist[1] && dist[5]<dist[3]){

		Salpha = alpha[2] + alpha[4] + alpha[5];} //Combinacao de 2,4,5

    if (dist[3]<dist[0] && dist[3]<dist[1] && dist[3]<dist[2] && dist[4]<dist[0] && dist[4]<dist[1] && dist[4]<dist[2] && dist[5]<dist[0] && dist[5]<dist[1] && dist[5]<dist[2]){

		Salpha = alpha[3] + alpha[4] + alpha[5];} //Combinacao de 3,4,5

	//if (dist[2]<dist[0] && dist[2]<dist[1] && dist[3]<dist[0] && dist[3]<dist[1]){
	//	Salpha = alpha[2] + alpha[3];}

	if ((Vizinho->orient ==2.0) && (Salpha<=1)){

        Vizinho->orient = Celula->orient;
        Vizinho->orient2 = Celula->orient2;
        Vizinho->orient3 = Celula->orient3;

        Decentrado = Celula->L;
        Decentrado = Truncado(Decentrado);
        Vizinho->L = Decentrado;
        dR[0] = (Vizinho->CXrelativo)-(Celula->CXrelativo);
        dR[1] = (Vizinho->CYrelativo)-(Celula->CYrelativo);
        dR[2] = (Vizinho->CZrelativo)-(Celula->CZrelativo);
        Vizinho->ativado = 1.0;

        if (dist[0]<dist[1] && dist[0]<dist[2] && dist[0]<dist[3] && dist[0]<dist[4] && dist[0]<dist[5]){

            Lu[0] = V1[0]-Celula->CXrelativo;
            Lu[1] = V1[1]-Celula->CYrelativo;
            Lu[2] = V1[2]-Celula->CZrelativo;
            Lv[0] = ((Vizinho->L)/(Celula->L))*Lu[0];
            Lv[1] = ((Vizinho->L)/(Celula->L))*Lu[1];
            Lv[2] = ((Vizinho->L)/(Celula->L))*Lu[2];

            Vizinho->CXrelativo = Vizinho->CXrelativo + (Lu[0]-dR[0]-Lv[0]);
            Vizinho->CYrelativo = Vizinho->CYrelativo + (Lu[1]-dR[1]-Lv[1]);
            Vizinho->CZrelativo = Vizinho->CZrelativo + (Lu[2]-dR[2]-Lv[2]);

        };

        if (dist[1]<dist[0] && dist[1]<dist[2] && dist[1]<dist[3] && dist[1]<dist[4] && dist[1]<dist[5]){

            Lu[0] = V2[0]-Celula->CXrelativo;
            Lu[1] = V2[1]-Celula->CYrelativo;
            Lu[2] = V2[2]-Celula->CZrelativo;
            Lv[0] = ((Vizinho->L)/(Celula->L))*Lu[0];
            Lv[1] = ((Vizinho->L)/(Celula->L))*Lu[1];
            Lv[2] = ((Vizinho->L)/(Celula->L))*Lu[2];

            Vizinho->CXrelativo = Vizinho->CXrelativo + (Lu[0]-dR[0]-Lv[0]);
            Vizinho->CYrelativo = Vizinho->CYrelativo + (Lu[1]-dR[1]-Lv[1]);
            Vizinho->CZrelativo = Vizinho->CZrelativo + (Lu[2]-dR[2]-Lv[2]);

        };

        if (dist[2]<dist[0] && dist[2]<dist[1] && dist[2]<dist[3] && dist[2]<dist[4] && dist[2]<dist[5]){

            Lu[0] = V3[0]-Celula->CXrelativo;
            Lu[1] = V3[1]-Celula->CYrelativo;
            Lu[2] = V3[2]-Celula->CZrelativo;
            Lv[0] = ((Vizinho->L)/(Celula->L))*Lu[0];
            Lv[1] = ((Vizinho->L)/(Celula->L))*Lu[1];
            Lv[2] = ((Vizinho->L)/(Celula->L))*Lu[2];

            Vizinho->CXrelativo = Vizinho->CXrelativo + (Lu[0]-dR[0]-Lv[0]);
            Vizinho->CYrelativo = Vizinho->CYrelativo + (Lu[1]-dR[1]-Lv[1]);
            Vizinho->CZrelativo = Vizinho->CZrelativo + (Lu[2]-dR[2]-Lv[2]);

        };

        if (dist[3]<dist[0] && dist[3]<dist[1] && dist[3]<dist[2] && dist[3]<dist[4] && dist[3]<dist[5]){

            Lu[0] = V4[0]-Celula->CXrelativo;
            Lu[1] = V4[1]-Celula->CYrelativo;
            Lu[2] = V4[2]-Celula->CZrelativo;
            Lv[0] = ((Vizinho->L)/(Celula->L))*Lu[0];
            Lv[1] = ((Vizinho->L)/(Celula->L))*Lu[1];
            Lv[2] = ((Vizinho->L)/(Celula->L))*Lu[2];

            Vizinho->CXrelativo = Vizinho->CXrelativo + (Lu[0]-dR[0]-Lv[0]);
            Vizinho->CYrelativo = Vizinho->CYrelativo + (Lu[1]-dR[1]-Lv[1]);
            Vizinho->CZrelativo = Vizinho->CZrelativo + (Lu[2]-dR[2]-Lv[2]);

        };

        if (dist[4]<dist[0] && dist[4]<dist[1] && dist[4]<dist[2] && dist[4]<dist[3] && dist[4]<dist[5]){

            Lu[0] = V5[0]-Celula->CXrelativo;
            Lu[1] = V5[1]-Celula->CYrelativo;
            Lu[2] = V5[2]-Celula->CZrelativo;
            Lv[0] = ((Vizinho->L)/(Celula->L))*Lu[0];
            Lv[1] = ((Vizinho->L)/(Celula->L))*Lu[1];
            Lv[2] = ((Vizinho->L)/(Celula->L))*Lu[2];

            Vizinho->CXrelativo = Vizinho->CXrelativo + (Lu[0]-dR[0]-Lv[0]);
            Vizinho->CYrelativo = Vizinho->CYrelativo + (Lu[1]-dR[1]-Lv[1]);
            Vizinho->CZrelativo = Vizinho->CZrelativo + (Lu[2]-dR[2]-Lv[2]);

        };

        if (dist[5]<dist[0] && dist[5]<dist[1] && dist[5]<dist[2] && dist[5]<dist[3] && dist[5]<dist[4]){

            Lu[0] = V6[0]-Celula->CXrelativo;
            Lu[1] = V6[1]-Celula->CYrelativo;
            Lu[2] = V6[2]-Celula->CZrelativo;
            Lv[0] = ((Vizinho->L)/(Celula->L))*Lu[0];
            Lv[1] = ((Vizinho->L)/(Celula->L))*Lu[1];
            Lv[2] = ((Vizinho->L)/(Celula->L))*Lu[2];

            Vizinho->CXrelativo = Vizinho->CXrelativo + (Lu[0]-dR[0]-Lv[0]);
            Vizinho->CYrelativo = Vizinho->CYrelativo + (Lu[1]-dR[1]-Lv[1]);
            Vizinho->CZrelativo = Vizinho->CZrelativo + (Lu[2]-dR[2]-Lv[2]);

        };

    }


}
double formacao(double deltaT, double Tsigma, double Tnuc){
	double e;

	e = 0.5*(( erf(Tnuc/(sqrt(2.0)*Tsigma)) )- ( erf((Tnuc-deltaT)/(sqrt(2.0)*Tsigma)) ));


	return e;


}
double erf(double x)
{
    // constantes
    double a1 =  0.254829592;
    double a2 = -0.284496736;
    double a3 =  1.421413741;
    double a4 = -1.453152027;
    double a5 =  1.061405429;
    double p  =  0.3275911;

    // Sinal de x
    int sinal = 1;
    if (x < 0)
        sinal = -1;
    x = fabs(x);

    // Handbock of Mathematics, formula 7.1.26
    double t = 1.0/(1.0 + p*x);
    double y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return sinal*y;
}
int RandCentro(int maxlinha,int maxcoluna, int maxplano) {
	int a,b, c;

	a = rand() % (maxlinha-1);
	b = rand() % (maxcoluna-1);
    c = rand() % (maxplano-1);

	a = ((c+1)*maxlinha*maxcoluna) + ((a+1)*maxcoluna) + (b+1);

	return a;
}
int RandParede(int maxlinha, int maxcoluna, int maxplano){
	int vetor[1000000], total, i, j, celula, plano, coluna, linha;
    
    i = 0;
    for (plano=0;plano<maxplano;plano++){
        for (linha=0;linha<maxlinha;linha++){
            for (coluna=0;coluna<maxcoluna;coluna++){
                celula = (plano*maxlinha*maxcoluna) + (linha*maxcoluna) + coluna;
                if ( (plano == 0) || (coluna == 0) || (linha == 0) ){
                    vetor[i] = celula;
                    i = i+1;
                }
                
            }
            
        }
        
    }

	total = (2*maxcoluna*maxlinha) + (2*maxcoluna*maxplano) + (2*maxlinha*maxplano);
    
    j = rand() % (total);
	return vetor[j];
}
double RandOrient (){
	double vetor[45];
	int a;

	vetor[0]  = -1.0; vetor[1] = -0.76;vetor[2] = -0.73; vetor[3] = -0.69; vetor[4] = -0.67; vetor[5] = -0.635; vetor[6] = -0.605;
	vetor[7] = -0.57; vetor[8] = -0.53; vetor[9] = -0.505; vetor[10] = -0.47; vetor[11] = -0.405; vetor[12]=-0.375; vetor[13]=-0.305;
	vetor[14]=-0.275; vetor[15] = -0.24; vetor[16]=-0.205;vetor[17]=-0.175;vetor[18]=-0.145;vetor[19]=-0.115;vetor[20]=-0.08;
	vetor[21]=-0.045;vetor[22]=-0.015;

	vetor[44] = 0.76;vetor[43] = 0.73; vetor[42] = 0.69; vetor[41] = 0.67; vetor[40] = 0.635; vetor[39] = 0.605;
	vetor[38] = 0.57; vetor[37] = 0.53; vetor[36] = 0.505; vetor[35] = 0.47; vetor[34] = 0.405; vetor[33]=0.375; vetor[32]=0.305;
	vetor[31]=0.275; vetor[30] = 0.24; vetor[29]=0.205;vetor[28]=0.175;vetor[27]=0.145;vetor[26]=0.115;vetor[25]=0.08;
	vetor[24]=0.045;vetor[23]=0.015;

	a = rand() % 45;

    //	printf("%f\n",vetor[a]);

	return vetor[a];
}

