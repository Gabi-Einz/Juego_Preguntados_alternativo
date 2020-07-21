#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#define MAX_CLI 10
 typedef struct{
int num_cli;
int puntaje;
char estado[10];
int usado;
float tiempo;
int conectado;
}t_cliente;

t_cliente clientes[MAX_CLI];
//Variables globales
char opcion[10]="\0";
int cant_resp=0;
int trigger = 10;
float tiempo_total=0;
int sc;
int bandera=0;

int formateo_de_preguntas(char* cad);
int contar_respuestas(char* cad);
int es_numero(char *cad);
int validar_opcion(char* opcion, int cant);
void timer(int trigger);
void *responder_preg(void *data);
void print_resultados(t_cliente *clientes,int cant_cli);

int main(int argc, char *argv[]){

  if(argc==1 || argc>3){
    printf("ERROR, cantidad de parametros incorrecta, para consultar la ayuda ingrese como parametro -help.\n");
     return -1;
    }
//Agregrando la ayuda
 if(strcmp(argv[1],"-help")==0 && argc==2){
   printf("TP3 Ejercicio 4\n");
   printf("Primer parametro: IP del servidor.\n");
   printf("Segundo parametro: Numero de puerto del servidor.\n");
   printf("Ejemplo de ejecución: ./Client 127.0.0.1 7200\n");
  printf("\nINTEGRANTES\nCereijo, Marcela DNI 18.102.057\nCéspedes Arias, Facundo Ezequiel DNI 37.688.243\nColque, Amelia Soledad DNI 34.095.247\nVargas, Juan Gabriel DNI 38.059.006\n");
   return;
 }
/************************/
  

 char* ip;
  ip=argv[1];
 int puerto,cant_preg=0;
 int puntos=0;
 int conexion,cantBytes;
 char buff[100];
 char buff_preg[300];
 //char opcion[10]; 
 char msj[50];
 int cant_cli=0;

struct hostent *he;
 /* estructura que recibira informacion sobre el nodo remoto */
 struct sockaddr_in server;
 /* informacion sobre la direccion del servidor */
 
//Uso la variable he
 he=gethostbyname(ip);
if (he == NULL){
 printf("La direccion ip del host es errónea.\n");
 return -2;
 }
if(!es_numero(argv[2])){
   printf("Error, el puerto ingresado no es valido. El rango de puertos validos son: (1024;65535).\n");
   return;
 }
 puerto=atoi(argv[2]);

 sc=socket(PF_INET,SOCK_STREAM,0);
  if( sc == -1){
     printf("Error al abrir socket.\n");
      return -3;
     }
//Datos del servidor, uso la variable server
 server.sin_family = AF_INET;
 server.sin_port = htons(puerto);
 server.sin_addr = *((struct in_addr *)he->h_addr);
 bzero(&(server.sin_zero),8);

//Conectandose al servidor
   conexion = connect(sc,(struct sockaddr *)&server,sizeof(struct sockaddr));
if(conexion == -1){
  printf("Error al intentar conectarse al servidor.\n");
  return -4;
  }

printf("Conectado exitosamente\n");
printf("Período de aceptación de clientes. Esperando al servidor...\n");
bzero(buff,100);
cantBytes=recv(sc,buff,100,0);
if(cantBytes == -1){
 printf("Error en recv()\n");
 return -5;
  }
buff[cantBytes]='\0';
printf("Mensaje del server: %s\n",buff);

if(strcmp(buff,"El servidor se ha caido.")==0)
   return;

int nro;//ADD
recv(sc,&nro,4,0);//ADD

//printf("Cantidad de bytes recibidos: %d\n",cantBytes);

printf("\nTu numero de cliente es: %d\n",nro);//ADD
/*printf("\nPresione Enter para comenzar el juego!\n");
  while ( getchar() != '\n');*/
 send(sc,"OK\n\0",4,0);

 cantBytes=recv(sc,&cant_preg,sizeof(cant_preg),0);//recivo cant_preg
 //printf("cantBytes: %d\n",cantBytes);
 printf("cant_preg: %d\n",cant_preg);
 printf("\nPuntaje inicial: %d\n\n",puntos);
int i;
char aux[10];
for(i=0;i<cant_preg;i++){

   //printf("antes de recv\n");
  bandera=0;
  bzero(buff_preg,300);
  buff_preg[0]='\0';
  cantBytes=recv(sc,buff_preg,300,0);
   //printf("despues de recv\n");
     if(cantBytes == -1){
       printf("Error en recv()\n");
      return -6;
       }
   buff_preg[cantBytes]='\0';
   //printf("Cantidad de bytes recibidos: %d\n",cantBytes);

   cant_resp=contar_respuestas(buff_preg);
   //printf("\ncant_resp: %d\n",cant_resp);

   formateo_de_preguntas(buff_preg);
   //system("clear");
   //fflush(stdin);
   puts(buff_preg);
    
    pthread_t hilo;   
    pthread_create(&hilo,NULL,&responder_preg,NULL);
    timer(trigger);
    pthread_cancel(hilo);
    //pthread_kill(hilo,-9);
    if(bandera==0)
      tiempo_total+=trigger;
 
      
      send(sc,opcion,10,0);
      //system("clear");
      bzero(msj,50);
      cantBytes=recv(sc,msj,50,0);
      if(cantBytes == -1){
       printf("Error en recv()\n");
      return -6;
       }
    msj[cantBytes]='\0';
    printf("\n\n\t%s\n\n",msj);
    //sleep(1);
     // para salir del for
     
     bzero(aux,10);
     cantBytes=recv(sc,aux,10,0);
     aux[cantBytes]='\0';

     if(strcmp(aux,"end")==0){
          printf("\nEl Server ha finalizado el juego.\n");
          break;}
       //printf("%.2f\n",tiempo_total);

}//llave del for
 send(sc,"Fin\n\0",5,0);
 cantBytes=recv(sc,&puntos,4,0);
 //printf("cantBytes: %d\n",cantBytes);
 printf("Puntaje final: %d\n",puntos);
 //bzero(ganadores,50);
 printf("Tiempo total:%.2f segundos.\n",tiempo_total);
 send(sc,&tiempo_total,sizeof(float),0);
 recv(sc,&cant_cli,sizeof(cant_cli),0);
 cantBytes=recv(sc,clientes,sizeof(t_cliente)*MAX_CLI,0);
 print_resultados(clientes,cant_cli);
 
 

close(sc);

return 0;
}

int formateo_de_preguntas(char* cad){
 char* aux;
 int cant=0;
 char i='1';
  if(cad==NULL){
   printf("Error, puntero a NULL\n");
   return 0;
   }
  aux=cad;

   while(*aux){
  
      aux=strchr(aux,'\n'); 

       if(aux==NULL){
          printf("No se encontro salto de linea.\n");
          return 0;}  
  
      aux++;
      if(*aux=='\0')
        return 1;

      *aux=i;
      i=i+1;
        
         if(*(aux+1)=='C'){
             cant=strlen(aux+2);
             memcpy(aux+1,aux+2,cant+1);}

      }//while
}//funcion

int contar_respuestas(char* cad){
int contador=0;
while(*cad){
 cad=strchr(cad,'\n');
 cad++;
 if(*cad=='\0' || cad==NULL)
    return contador;

 contador++;
 }
return contador-1;
}

int es_numero(char *cad){
 if(cad==NULL)
    return 0;

while(*cad){
 if(*cad<'0' || *cad>'9')
    return 0;
 cad++;
 }
 return 1;
}

int validar_opcion(char* opcion, int cant){
   int valor=0;
   if(!es_numero(opcion)){
    printf("Ingrese una opción valida. Las mismas aparecen con un valor numerico al inicio de cada respuesta.\n");
    return 0;
   }
   valor=atoi(opcion);

   if(valor>cant || valor<=0){
    printf("Ingrese una opción valida. Las mismas aparecen con un valor numerico al inicio de cada respuesta.\n");
    return 0;
   }
   return 1;
}

void timer(int trigger){

int sec = 0;
printf("\nTienes %d segundos para responder!\n",trigger);

clock_t before = clock();
 
do{
  clock_t difference = clock() - before;
  sec = difference / CLOCKS_PER_SEC;
  }while ( sec < trigger );

   printf("\nTiempo finalizado.\n");

}

void *responder_preg(void *data){

   time_t inicio,fin;
   time(&inicio);
   float dif=0;
   opcion[0]=' ';
   do{
     printf("\nIngresar opción:");
     fflush(stdin);
     scanf("%s",opcion);
      }while(!validar_opcion(opcion,cant_resp));

    time(&fin);
          dif=difftime(fin,inicio);
          //printf("tiempo: %.2f",dif);
          tiempo_total+=dif;//Calcula la dif de tiempo, devuelve en seg.
          bandera=1;
      printf("\nEsperando al servidor...\n");
}

void print_resultados(t_cliente *clientes,int cant_cli)
{  int i;
   for(i=0;i<cant_cli;i++){
   printf("Cliente N°%d Puntaje final:%d Estado:%s Tiempo total de respuesta:%.2f segundos.\n",clientes[i].num_cli,clientes[i].puntaje,clientes[i].estado,clientes[i].tiempo);
   }

}
/*
void cliente_crashed(int signum){

char pid[]="";
 
  send(sc,"quit",4,0);
  close(sc);
   sprintf(pid,"kill %d",getpid());
   system(pid);
}*/
